#include "koda_compiler.h"

#include <format>
#include <fstream>
#include <iterator>
#include <string>

#include "KodaLexer.h"
#include "KodaParser.h"
#include "antlr4-runtime.h"
#include "ast.h"
#include "cst2ast.h"
#include "error_listener.h"
#include "result.h"

#define IF_ALT(ALT, OBJ, CALL, ...)     \
  if (std::holds_alternative<ALT>(OBJ)) \
  {                                     \
    if (auto obj = std::get<ALT>(OBJ))  \
      return CALL(obj, __VA_ARGS__);    \
  }

#define ELSE_IF_ALT(ALT, OBJ, CALL, ...)     \
  else if (std::holds_alternative<ALT>(OBJ)) \
  {                                          \
    if (auto obj = std::get<ALT>(OBJ))       \
      return CALL(obj, __VA_ARGS__);         \
  }

#define INCREMENT_MAP(MAP, KEY)     \
  do                                \
  {                                 \
    if (MAP.find(KEY) == MAP.end()) \
      MAP[KEY] = 1;                 \
    else                            \
      MAP[KEY] = MAP[KEY] + 1;      \
  } while (false);

namespace koda
{

std::string toFlowVariable(const std::string& name)
{
  if (name.find("alarm") != std::string::npos)
    return name;

  return name == "main" ? "main" : "f_" + name;
};

Compiler::Compiler()
{
}

VoidResult Compiler::parse(const CompilerOptions& options)
{
  mOptions = options;

  LOG_DEBUG("Compiling %s", options.inputFile.c_str());

  std::ifstream stream;
  stream.open(mOptions.inputFile);
  if (!stream.is_open())
    return VoidResult::Failed("Failed to open: " + mOptions.inputFile);

  antlr4::ANTLRInputStream input(stream);
  KodaLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  KodaParser parser(&tokens);

  CollectingErrorListener errorListener;
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);

  KodaCST2AST visitor;
  KodaParser::SystemContext* tree = parser.system();
  if (errorListener.hasErrors() || tree == nullptr)
  {
    for (const auto& err : errorListener.errors)
      LOG_WARNING(err);

    return VoidResult::Failed("Failed to parse input file");
  }

  try
  {
    mAST = std::any_cast<koda::System>(visitor.visitSystem(tree));
  } catch (const std::bad_any_cast& e)
  {
    return VoidResult::Failed(std::string("Failed to parse. Issue: ") + e.what());
  } catch (const std::invalid_argument& e)
  {
    return VoidResult::Failed(std::string("Failed to parse. Issue: ") + e.what());
  }

  if (mOptions.verbose > 0)
    printAST();

  return VoidResult();
}

VoidResult Compiler::runPlugins()
{
  for (const auto& plugin : mPlugins)
    RETURN_ON_FAILURE(plugin.second->generate(mOptions, mAST));

  return VoidResult();
}

std::vector<std::string> Compiler::generatedFiles() const
{
  return mGeneratedFiles;
}

void Compiler::mapToSrcId(const std::string& dezyneName, const std::string& srcId) {
  if (!srcId.empty())
    mAstMap[dezyneName] = srcId;
  else
    LOG_WARNING("No srcId for '%s'", dezyneName.c_str());
}

VoidResult Compiler::generate()
{
  mGeneratedFiles.clear();

  std::string mirrorPath = mOptions.outputDir + "/../maki_ast.json";
  try
  {
    mMirrorRoot = loadMirrorAST(mirrorPath);
  } catch (const std::exception& e)
  {
    LOG_ERROR("Failed to load mirror AST: %s", e.what());
  }

  LOG_DEBUG("Loaded mirror AST.");

  if (mOptions.pluginRule == CompilerOptions::PluginOption::PluginsOnly)
    return runPlugins();

  Environment env;
  const auto& compChildren = mMirrorRoot.group("components");
  LOG_DEBUG("SYSTEM MIRROR AST: %s, %d, %s", mMirrorRoot.toString().c_str(), compChildren.size(), compChildren[0].name.c_str());
  if (!(mMirrorRoot.ASTtype == "System"))
    LOG_ERROR("Mirror AST root is not a System node, found: %s", mMirrorRoot.ASTtype.c_str());

  size_t compIdx = 0;
  for (auto& component : mAST.components)
  {
    if (component->kind == Component::Kind::Capability)
    {
      MirrorNode componentMirror = compChildren[compIdx];
      if (!(componentMirror.ASTtype == "Component") || !(componentMirror.name == component->name))
        LOG_ERROR("Mirror AST component mismatch at index %zu: expected Component with name '%s', found '%s' with name '%s'", compIdx, component->name.c_str(), componentMirror.ASTtype.c_str(), componentMirror.name.c_str());
      RETURN_ON_FAILURE(generateCapability(component, componentMirror, env));
    }
    compIdx++;
  }

  compIdx = 0;
  for (auto& component : mAST.components)
  {
    if (component->kind == Component::Kind::Task)
    {
      MirrorNode componentMirror = compChildren[compIdx];
      if (!(componentMirror.ASTtype == "Component") || !(componentMirror.name == component->name))
        LOG_ERROR("Mirror AST component mismatch at index %zu: expected Component with name '%s', found '%s' with name '%s'", compIdx, component->name.c_str(), componentMirror.ASTtype.c_str(), componentMirror.name.c_str());
      RETURN_ON_FAILURE(generateTask(component, componentMirror, env));
    }
    compIdx++;
  }

  for (auto& component : mAST.components)
  {
    if (component->kind == Component::Kind::Capability)
      RETURN_ON_FAILURE(emitCapability(component, env));
  }

  mEnv = env;

  if (mOptions.pluginRule == CompilerOptions::PluginOption::RunAll)
    return runPlugins();

  return VoidResult();
}

VoidResult Compiler::addPlugin(std::shared_ptr<KodaPlugin> plugin)
{
  if (!plugin)
    return VoidResult::Failed("Invalid, null plugin");
  else if (mPlugins.contains(plugin->id()))
    return VoidResult::Failed("Plugin already exists");

  // Set callbacks
  plugin->toFilename = [this](const std::string& name) { return toFilename(name); };
  plugin->componentName = [this](const std::string& name) { return componentName(name); };
  plugin->flowName = [this](const std::string& name) { return flowName(name); };

  mPlugins[plugin->id()] = plugin;

  return VoidResult();
}

void Compiler::printAST() const
{
  mAST.print();
}

System Compiler::getAST() const
{
  return mAST;
}

Compiler::Environment Compiler::getIR() const
{
  return mEnv;
}

// =========================================================================================
// Generation methods

Result<koda::ReturnValue> Compiler::generateTask(PComponent task, const MirrorNode& node, Environment& env)
{
  for (const auto& arg : task->args)
  {
    auto name = arg->a;
    auto type = arg->b;
    LOG_INFO("Looking for %s", type.c_str());
    if (env.capabilities.contains(type))
      env.capabilityMap[name] = type;
  }

  const auto& statementChildren = node.group("statements");
  size_t stmntIdx = 0;
  for (auto& statement : task->statements)
  {
    if (*safeChild(*safeChild(node, "statements", stmntIdx), "node", 0)->ASTtype == "VarsBlock" && *safeChild(*safeChild(node, "statements", stmntIdx), "node", 0)->group("vars").empty()) {
      stmntIdx++; // Skip empty vars block
      LOG_DEBUG("Skipping empty vars block at index %zu", stmntIdx - 1);
    }
    RETURN_ON_FAILURE(generateStatement(statement, statementChildren[stmntIdx], env));
    stmntIdx++;
  }

  for (const auto& f : env.flows)
  {
    const auto flow = f.second;
    const auto flowName = flow.name;
    for (const auto& c : flow.asyncCalls)
    {
      auto index = c.find_first_of("_");
      auto capName = c.substr(0, index);
      auto cap = env.getCapability(capName);
      std::string name = "";
      std::string trigger = "";
      if (!cap)
      {
        // Sometimes strategies are parsed as async calls...
        if (!env.flows.contains(capName))
          return Result<koda::ReturnValue>::Failed("Could not find async capability: " + c);

        auto tmp = env.flows.at(capName);
        name = toFlowVariable(tmp.name);
        trigger = "api";
      }
      else
      {
        name = cap->name;
        if (index == std::string::npos)
          trigger = cap->trigger->name;
        else
          trigger = std::format("{}_{}", cap->trigger->name, c.substr(index + 1));
      }

      PortRef in = {toFlowVariable(flowName), c};
      PortRef out = {toFilename(name), trigger};

      if (mOptions.verbose > 1)
        LOG_RAW("asyncCalls {} - In: {} Out: {}", c, in, out);

      env.system.connections.push_back(Connection{in, out, Connection::Type::Action});
    }

    for (const auto& c : flow.syncCalls)
    {
      auto [instance, port] = portFromString(c.first);
      auto cap = env.getCapability(instance);
      std::string name = "";
      if (!cap)
      {
        // Sometimes strategies are parsed as async calls...
        if (!env.flows.contains(instance))
          return Result<koda::ReturnValue>::Failed("Could not find sync capability: " + c.first);

        auto tmp = env.flows.at(instance);
        name = tmp.name;
      }
      else
      {
        name = cap->name;
      }

      PortRef in = {toFlowVariable(flowName), std::format("{}_{}", instance, port)};
      PortRef out = {toFilename(cap->name), port};

      if (mOptions.verbose > 1)
        LOG_RAW("syncCalls {} - In: {} Out: {}", c.first, in, out);
      env.system.connections.push_back(Connection{in, out, Connection::Type::Action});
    }

    for (const auto& c : flow.signalCalls)
    {
      auto [instance, port] = portFromString(c.first);
      auto cap = env.getCapability(instance);
      if (!cap)
        return Result<koda::ReturnValue>::Failed("  signal capability: " + c.first);

      PortRef in = {toFlowVariable(flowName), std::format("{}_{}", instance, port)};
      PortRef out = {toFilename(cap->name), port};

      if (mOptions.verbose > 1)
        LOG_RAW("signalCalls - In: {} Out: {}", in, out);
      env.system.connections.push_back(Connection{in, out, Connection::Type::Signal});
    }

    for (const auto& c : flow.strategies)
    {
      PortRef in = {toFlowVariable(flowName), c.first};
      PortRef out = {toFlowVariable(c.first), "api"};

      if (mOptions.verbose > 1)
        LOG_RAW("strategy - In: {} Out: {}", in, out);
      env.system.connections.push_back(Connection{in, out, Connection::Type::Action});
    }
  }

  env.includes = {};
  connectWithArbiter(env);

  if (mOptions.dryRun)
    return koda::ReturnValue();

  // With the flows for this task defined, we can now connect all flows into a complete strategy
  std::string filename = std::format("{}/{}_task.dzn", mOptions.outputDir, toFilename(task->name));
  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  // Imports
  file << "import iaction.dzn;\n";
  file << "import isignal.dzn;\n\n";

  if (env.alarm > 0)
    file << "import alarm.dzn;\n\n";

  for (const auto& inc : env.includes)
    file << std::format("import {};\n", inc);
  file << "\n";

  // Capabilities
  for (const auto& cap : env.capabilities)
    file << std::format("import a_{}.dzn;\n", toFilename(cap.second.name));
  file << "\n";

  // Flows
  for (const auto& flow : env.flows)
    file << std::format("import {}.dzn;\n", toFilename(flow.second.name));
  file << "\n";

  file << std::format("component {} {{\n", componentName(task->name));
  file << "  provides iaction api;\n\n";
  file << "  system {\n";

  for (const auto& instance : env.system.instances)
    file << std::format("    {} {};\n", instance.type, instance.name);

  // Helpers

  file << "\n    api <=> main.api;\n\n";

  // Connections
  std::string s = env.system.connections.at(0).lhs.instance;
  for (const auto& conn : env.system.connections)
  {
    if (conn.lhs.instance != s)
    {
      file << "\n";
      s = conn.lhs.instance;
    }

    file << std::format("    {}.{} <=> {}.{};\n", conn.lhs.instance, conn.lhs.port, conn.rhs.instance, conn.rhs.port);
  }

  file << "  }\n";
  file << "}";

  // Component
  file.close();
  mGeneratedFiles.push_back(filename);

  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateCapability(PComponent capability, const MirrorNode& node, Environment& env)
{
  LOG_DEBUG("Generating capability: %s, %s", capability->name.c_str(), node.name.c_str());
  if (mOptions.verbose > 0)
    LOG_RAW("Compiling capability: {}", capability->name);

  env.currentCapability = Capability{};
  env.currentCapability.name = capability->name;
  env.currentCapability.srcId = node.srcId;

  const auto& statementChildren = node.group("statements");
  size_t stmntIdx = 0;
  for (const auto& statement : capability->statements)
  {
    RETURN_ON_FAILURE(generateStatement(statement, statementChildren[stmntIdx], env));
    stmntIdx++;
  }

  env.capabilities[capability->name] = env.currentCapability;
  env.system.instances.insert({componentName(capability->name), toFilename(capability->name)});

  return koda::ReturnValue();
}

Result<ReturnValue> Compiler::emitCapability(PComponent capability, Environment& env)
{
  if (mOptions.dryRun)
    return koda::ReturnValue();

  // Then we proceed with the creation of the file itself
  std::string filename = std::format("{}/a_{}.dzn", mOptions.outputDir, toFilename(capability->name));
  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  auto cap = env.capabilities[capability->name];

  file << "import iaction.dzn;\n\n";
  file << std::format("component {} {{\n", componentName(capability->name));
  if (cap.trigger)
  {
    auto name = toFilename(capability->name);
    if (env.asyncCallsCounter.contains(name))
    {
      LOG_INFO("Capability %s with %d entries", name.c_str(), env.asyncCallsCounter[name]);
      for (uint32_t i = 0; i < env.asyncCallsCounter[name]; ++i)
      {
        Action action = cap.trigger.value();
        action.name = std::format("{}_{}", action.name, i + 1);
        file << createPort(action, true);
      }
    }
    else
    {
      LOG_ERROR("No such capability in the counter map: %s", name.c_str());
    }
  }
  if (cap.abort)
    file << createPort(*cap.abort, true);
  for (const auto& in : cap.ins)
    file << createPort(in, true);
  for (const auto& out : cap.outs)
    file << createPort(out, false);

  file << "}";
  file.close();

  mGeneratedFiles.push_back(filename);

  return koda::ReturnValue();
}

Result<ReturnValue> Compiler::generateArgument(PArgument argument, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{argument->a, argument->b};
}

Result<ReturnValue> Compiler::generateStatement(PStatement statement, const MirrorNode& node, Environment& env)
{
  const auto& nodeMirror = *safeChild(node, "node", 0);
  IF_ALT(PStrategyBlock, statement->node, generateStrategyBlock, nodeMirror, env)
  ELSE_IF_ALT(PActionDef, statement->node, generateActionDef, nodeMirror, env)
  ELSE_IF_ALT(PRosDef, statement->node, generateRosDef, nodeMirror, env)
  ELSE_IF_ALT(PVarsBlock, statement->node, generateVarsBlock, nodeMirror, env)
  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateStrategyBlock(PStrategyBlock strategy, const MirrorNode& node, Environment& env)
{
  const auto& flowsMirror = node.group("flows");
  size_t flowIdx = 0;
  for (auto& flow : strategy->flows)
  {
    LOG_DEBUG("BEFORE %d", flowIdx);
    MirrorNode flowMirror = flowsMirror[flowIdx];
    if (!(flowMirror.ASTtype == "Flow") || !(flowMirror.name == flow->name))
      LOG_ERROR("Mirror AST flow mismatch at index %zu: expected Flow with name) '%s', found '%s' with name '%s'", flowIdx, flow->name.c_str(), flowMirror.ASTtype.c_str(), flowMirror.name.c_str());
    RETURN_ON_FAILURE(generateFlow(flow, flowMirror, env));
    flowIdx++;
    LOG_DEBUG("AFTER");
  }

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateActionDef(PActionDef action, const MirrorNode& node, Environment& env)
{
  env.currentCapability.title = action->label1;
  env.currentCapability.message = action->label2;
  const auto& adMirror = node.group("rosDefs");
  size_t rdIdx = 0;
  for (const auto& def : action->rosDefs)
  {
    RETURN_ON_FAILURE(generateRosDef(def, adMirror[rdIdx], env));
    rdIdx++;
  }
  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateRosDef(PRosDef ros, const MirrorNode& node, Environment& env)
{
  ReturnValue result;
  const auto& defMirror = *safeChild(node, "def", 0);
  if (!(defMirror.ASTtype == "EventDef") || !(defMirror.name == ros->def->name))
    LOG_ERROR("Mirror AST event definition mismatch: expected EventDef with name '%s', found '%s' with name '%s'", ros->def->name.c_str(), defMirror.ASTtype.c_str(), defMirror.name.c_str());
  ASSIGN_OR_RETURN_ON_FAILURE(result, generateEventDef(ros->def, defMirror, env));

  Action action;
  action.name = result.call;
  action.args = result.args;
  action.srcId = node.srcId;

  if (ros->kind == koda::RosDef::Kind::Trigger)
    env.currentCapability.trigger = action;
  else if (ros->kind == koda::RosDef::Kind::Return)
    env.currentCapability.success = action;
  else if (ros->kind == koda::RosDef::Kind::Abort)
    env.currentCapability.abort = action;
  else if (ros->kind == koda::RosDef::Kind::Error)
    env.currentCapability.error = action;
  else if (ros->kind == koda::RosDef::Kind::In)
    env.currentCapability.ins.push_back(action);
  else if (ros->kind == koda::RosDef::Kind::Out)
    env.currentCapability.outs.push_back(action);

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateEventDef(PEventDef event, const MirrorNode& node, Environment& env)
{
  koda::ReturnValue value;
  value.call = event->name;
  value.name = event->typeName;
  const auto& argsMirror = node.group("args");
  size_t argIdx = 0;
  for (const auto& arg : event->args)
  {
    koda::ReturnValue argValue;
    ASSIGN_OR_RETURN_ON_FAILURE(argValue, generateArgument(arg, argsMirror[argIdx], env));
    value.args[argValue.call] = argValue.name;
    argIdx++;
  }

  return value;
}

Result<ReturnValue> Compiler::generateVarsBlock(PVarsBlock varsBlock, const MirrorNode& node, Environment& env)
{
  const auto& varsMirror = node.group("vars");
  size_t varsIdx = 0;
  for (const auto& var : varsBlock->vars) {
    MirrorNode varMirror = varsMirror[varsIdx];
    if (!(varMirror.ASTtype == "Vars") || !(varMirror.name == var->name))
      LOG_ERROR("Mirror AST vars definition mismatch at index %zu: expected Vars with name '%s', found '%s' with name '%s'", varsIdx, var->name.c_str(), varMirror.ASTtype.c_str(), varMirror.name.c_str());
    RETURN_ON_FAILURE(generateVarsDef(var, varMirror, env));
    varsIdx++;
  }

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateVarsDef(PVarDef var, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{};
}

void Compiler::connectWithArbiter(const std::map<std::string, uint32_t>& connections, Connection::Type connectionType, Environment& env)
{
  for (auto it = connections.cbegin(); it != connections.cend(); ++it)
  {
    // Create arbitrer for n strategies
    auto occurences = it->second;
    if (occurences < 2)
      continue;

    // Create arbitrer
    createActionArbiterComponent(occurences);

    int count = 0;
    env.includes.insert(std::format("action_arbiter{}.dzn", occurences));

    auto id = env.arbiter++;
    env.definitions.push_back(std::format("caction_arbiter{} arbitrer{}", occurences, id));

    auto [instance, port] = portFromString(it->first);
    if (mOptions.verbose > 0)
      LOG_RAW("Arbiter evaluation, {} {} has {} connections", instance, port, it->second);

    const auto toReplace = connectionType == Connection::Type::Signal ? std::format("{}_{}", instance, port) : instance;
    // Change current links to the arbitrer
    for (auto& statement : env.core)
    {
      auto index0 = statement.find(toReplace);
      if (index0 != std::string::npos)
        statement.replace(index0, toReplace.size(), std::format("arbitrer{}.client{}", id, count++));
    }

    // Connect arbitrer to the component
    env.core.push_back(std::format("arbitrer{}.resource <=> {}", id, toReplace));
  }
}

Result<koda::ReturnValue> Compiler::generateFlow(PFlow flow, const MirrorNode& node, Environment& env)
{
  if (mOptions.verbose > 0)
    LOG_RAW("Generating flow: {}", flow->name);

  // Compile the different connections
  env.clear();
  const auto& strategyMirror = *safeChild(node, "strategy", 0);
  auto ret = generateStrategy(flow->strategy, strategyMirror, env);

  env.flows[flow->name] = Flow{flow->name, env.syncCalls, env.asyncCalls, env.signalCalls, env.strategies};
  env.system.instances.insert({flowName(flow->name), toFlowVariable(toFilename(flow->name))});

  if (mOptions.verbose > 0)
    env.print();

  // ------------------------------------------------------------
  // Check the need for arbitrers and create them
  connectWithArbiter(env.strategies, Connection::Type::Action, env);
  connectWithArbiter(env.syncCalls, Connection::Type::Signal, env);
  connectWithArbiter(env.signalCalls, Connection::Type::Signal, env);

  // ------------------------------------------------------------
  // Print everything to a file
  env.core.push_front(std::format("api <=> {}", ret.Value().name));
  env.includes.insert("iaction.dzn");

  if (mOptions.dryRun)
    return ret;

  // There is one file per flow, so here we create a new file
  std::string filename = std::format("{}/{}.dzn", mOptions.outputDir, flow->name);
  mCurrentFile.open(filename);
  if (!mCurrentFile.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  for (const auto& i : env.includes)
    mCurrentFile << "import " + i + ";\n";

  mCurrentFile << "\n";
  mCurrentFile << std::format("component {} {{\n", flowName(flow->name));
  mCurrentFile << "  provides iaction api;\n\n";
  for (const auto& c : env.requiresPorts)
    mCurrentFile << "  requires " + c + ";\n";

  // Print the system
  // First the definitions
  mCurrentFile << "\n  system {\n";
  for (const auto& c : env.definitions)
    mCurrentFile << "    " + c + ";\n";

  // Then the connections
  mCurrentFile << "\n";
  for (const auto& c : env.core)
    mCurrentFile << "    " + c + ";\n";

  mCurrentFile << "  }\n";
  mCurrentFile << "}";

  // Compose
  mCurrentFile.close();
  mGeneratedFiles.push_back(filename);

  return ret;
}

Result<koda::ReturnValue> Compiler::generateStrategy(PStrategy strategy, const MirrorNode& node, Environment& env)
{
  const auto& childMirror = *safeChild(node, "v", 0);
  IF_ALT(PSeq, strategy->v, generateSequence, childMirror, env)
  ELSE_IF_ALT(PJoin, strategy->v, generateJoin, childMirror, env)
  ELSE_IF_ALT(PEither, strategy->v, generateEither, childMirror, env)
  ELSE_IF_ALT(PLet, strategy->v, generateLet, childMirror, env)
  ELSE_IF_ALT(PWithin, strategy->v, generateWithin, childMirror, env)
  ELSE_IF_ALT(PIfElse, strategy->v, generateIfElse, childMirror, env)
  ELSE_IF_ALT(PRepeat, strategy->v, generateRepeat, childMirror, env)
  ELSE_IF_ALT(PGuard, strategy->v, generateGuard, childMirror, env)
  ELSE_IF_ALT(PEnd, strategy->v, generateEnd, childMirror, env)
  ELSE_IF_ALT(PContinue, strategy->v, generateContinue, childMirror, env)
  ELSE_IF_ALT(PRef, strategy->v, generateRef, childMirror, env)
  ELSE_IF_ALT(PTaskCall, strategy->v, generateTaskCall, childMirror, env)
  ELSE_IF_ALT(PParen, strategy->v, generateParen, childMirror, env)
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateSequence(PSeq strategy, const MirrorNode& node, Environment& env)
{
  for (auto it = strategy->alts.begin(); it != strategy->alts.end();)
  {
    // Before moving with the generation, we must check whether we have Continue blocks
    if (std::get_if<koda::PContinue>(&(*it)->v))
    {
      it = strategy->alts.erase(it);
    }
    else
    {
      ++it;
    }
  }

  auto instances = strategy->alts.size();
  // No point in creating the sequence component if there is only one action in the sequence
  if (instances == 1)
  {
    return generateStrategy(strategy->alts.at(0), *safeChild(node, "alts", 0), env);
  }

  auto id = env.sequence++;
  auto created = createSequenceComponent(instances);
  if (!created.IsSuccess())
    return Result<koda::ReturnValue>::Failed(created.ErrorMessage());

  env.includes.insert(std::format("sequence{}.dzn", instances));
  env.definitions.push_back(std::format("csequence{} s{}", instances, id));

  const auto& altsMirror = node.group("alts");
  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], altsMirror[i], env));
    env.core.push_back(std::format("s{}.action{} <=> {}", id, i, expr.name));
  }

  return koda::ReturnValue{std::format("s{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateJoin(PJoin strategy, const MirrorNode& node, Environment& env)
{
  auto id = env.join++;
  env.includes.insert("parallel.dzn");
  env.definitions.push_back(std::format("cparallel p{}", id));

  const auto& altsMirror = node.group("alts");
  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], altsMirror[i], env));
    env.core.push_back(std::format("p{}.action{} <=> {}", id, i, expr.name));
  }

  return koda::ReturnValue{Format("p%d.api", id)};
}

Result<koda::ReturnValue> Compiler::generateEither(PEither strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateLet(PLet strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateWithin(PWithin strategy, const MirrorNode& node, Environment& env)
{
  ReturnValue exprDo;
  ASSIGN_OR_RETURN_ON_FAILURE(exprDo, generateStrategy(strategy->a, *safeChild(node, "a", 0), env));

  ReturnValue exprElse;
  ASSIGN_OR_RETURN_ON_FAILURE(exprElse, generateStrategy(strategy->b, *safeChild(node, "b", 0), env));

  auto id = env.within++;
  auto alarmId = env.alarm++;

  env.includes.insert("within.dzn");
  env.includes.insert("ialarm.dzn");
  env.definitions.push_back(std::format("cwithin w{}", id));

  env.system.instances.insert({"calarm", std::format("alarm{}", alarmId)});

  env.core.push_back(std::format("w{}.action1 <=> {}", id, exprDo.name));
  env.core.push_back(std::format("w{}.action2 <=> {}", id, exprElse.name));
  env.core.push_back(std::format("w{}.alarm <=> alarm{}", id, alarmId));

  INCREMENT_MAP(env.strategies, std::format("alarm{}", alarmId));
  env.requiresPorts.insert(std::format("ialarm alarm{}", alarmId));

  return koda::ReturnValue{std::format("w{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateIfElse(PIfElse strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateRepeat(PRepeat strategy, const MirrorNode& node, Environment& env)
{
  if (strategy->iterations > 0)
    return generateEvery(strategy, node, env);

  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->a, *safeChild(node, "a", 0), env));

  auto id = env.repeat++;
  env.includes.insert("repeat.dzn");
  env.definitions.push_back(std::format("crepeat r{}", id));

  const auto& handlersMirror = node.group("handlers");
  size_t handlerIdx = 0;

  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  env.core.push_back(std::format("r{}.action1 <=> {}", id, expr.name));

  return koda::ReturnValue{std::format("r{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateGuard(PGuard strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateEvery(PRepeat strategy, const MirrorNode& node, Environment& env)
{
  ReturnValue expr;
  // Since Every does not exist on the AST, we are still in the Repeat node.
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->a, *safeChild(node, "a", 0), env));

  auto id = env.every++;
  auto alarmId = env.alarm++;

  env.includes.insert("every.dzn");
  env.includes.insert("ialarm.dzn");
  env.definitions.push_back(std::format("cevery e{}", id));

  env.system.instances.insert({"calarm", std::format("alarm{}", alarmId)});

  INCREMENT_MAP(env.strategies, std::format("alarm{}", alarmId));
  env.requiresPorts.insert(std::format("ialarm alarm{}", alarmId));

  const auto& handlersMirror = node.group("handlers");
  size_t handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  env.core.push_back(std::format("e{}.action1 <=> {}", id, expr.name));
  env.core.push_back(std::format("e{}.alarm <=> alarm{}", id, alarmId));

  return koda::ReturnValue{std::format("e{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateEnd(PEnd strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{"end"};
}

Result<ReturnValue> Compiler::generateContinue(PContinue strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{"continue"};
}

Result<koda::ReturnValue> Compiler::generateRef(PRef strategy, const MirrorNode& node, Environment& env)
{
  INCREMENT_MAP(env.strategies, strategy->name)
  env.requiresPorts.insert(std::format("iaction {}", strategy->name));

  return koda::ReturnValue{strategy->name};
}

Result<koda::ReturnValue> Compiler::generateTaskCall(PTaskCall strategy, const MirrorNode& node, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(strategy->call, *safeChild(node, "call", 0), env, false));

  if (strategy->handlers.empty())
    return expr;

  const auto& handlersMirror = node.group("handlers");
  size_t handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, handlersMirror[handlerIdx], env));
    handlerIdx++;
  }

  return expr;
}

Result<koda::ReturnValue> Compiler::generateParen(PParen strategy, const MirrorNode& node, Environment& env)
{
  return generateStrategy(strategy->a, *safeChild(node, "a", 0), env);
}

Result<ReturnValue> Compiler::generateStrategyHandler(PStrategyHandler handler, const MirrorNode& node, Environment& env)
{
  uint32_t id = 0;
  if (handler->kind == koda::StrategyHandler::Kind::OnAbort)
  {
    id = env.abortHandler++;
    env.includes.insert("abort_handler.dzn");
    env.definitions.push_back(std::format("cabort_handler ah{}", id));

    if (handler->emitter)
    {
      ReturnValue expr;
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, false));
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));

      env.core.push_back(std::format("ah{}.action <=> {}", id, env.previousCall));
      env.core.push_back(std::format("ah{}.handler <=> {}", id, strat.name));
    }

    return koda::ReturnValue{Format("ah%d.api", id)};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnError)
  {
    id = env.errorHandler++;
    env.includes.insert("error_handler.dzn");
    env.definitions.push_back(std::format("cerror_handler fh{}", id));

    if (handler->emitter)
    {
      ReturnValue expr;
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, false));
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));

      env.core.push_back(std::format("fh{}.action <=> {}", id, env.previousCall));
      env.core.push_back(std::format("fh{}.handler <=> {}", id, strat.name));
    }

    return koda::ReturnValue{Format("fh%d.api", id)};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
  {
    id = env.signalHandler++;
    env.includes.insert("signal_handler.dzn");
    env.definitions.push_back(std::format("csignal_handler sh{}", id));

    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, true));

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.name);

    ReturnValue strat;
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));

    env.core.push_back(std::format("sh{}.signal <=> {}", id, expr.name));
    env.core.push_back(std::format("sh{}.action <=> {}", id, env.previousCall));
    env.core.push_back(std::format("sh{}.handler <=> {}", id, strat.name));

    return koda::ReturnValue{std::format("sh{}.api", id)};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnEmitterContinue)
  {
    id = env.signalHandler++;
    env.includes.insert("signal_continue.dzn");
    env.definitions.push_back(std::format("csignal_continue sh{}", id));

    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, true));

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.name);

    ReturnValue strat;
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));

    env.core.push_back(std::format("sh{}.signal <=> {}", id, expr.name));
    env.core.push_back(std::format("sh{}.action <=> {}", id, env.previousCall));
    env.core.push_back(std::format("sh{}.handler <=> {}", id, strat.name));

    return koda::ReturnValue{std::format("sh{}.api", id)};
  }

  return koda::ReturnValue{};
}

Result<koda::ReturnValue> Compiler::generateEventCall(PEventCall call, const MirrorNode& node, Environment& env, bool isSignal)
{
  if (mOptions.verbose > 1)
    LOG_RAW("Generating event call: {} receiver: {}", call->name, call->receiver);

  if (call->receiver.empty())
  {
    INCREMENT_MAP(env.asyncCallsCounter, call->name)
    auto identifier = std::format("{}_{}", call->name, env.asyncCallsCounter[call->name]);
    env.asyncCalls.push_back(identifier);
    env.requiresPorts.insert(std::format("iaction {}", identifier));
    std::vector<std::string> args = {};
    LOG_DEBUG("Generating expr %s (%d) with call: %ld", call->name.c_str(), env.asyncCallsCounter[call->name], call->args.size());
    const auto& argsMirror = node.group("args");
    size_t argIdx = 0;
    for (const auto& expr : call->args)
    {
      auto ret = generateExpr(expr, argsMirror[argIdx], env);
      if (ret.IsSuccess() && !ret.Value().name.empty())
        args.push_back(ret.Value().name);
      else
        LOG_ERROR(ret.ErrorMessage());
      argIdx++;
    }

    env.capabilityCalls[call->name].push_back(CapabilityCall{
        .count = env.asyncCallsCounter[call->name],
        .args = args,
    });

    return koda::ReturnValue{identifier};
  }
  else
  {
    auto event = std::format("{}.{}", call->receiver, call->name);
    if (isSignal)
    {
      INCREMENT_MAP(env.signalCalls, event)
      env.requiresPorts.insert(std::format("isignal {}_{}", call->receiver, call->name));
    }
    else
    {
      INCREMENT_MAP(env.syncCalls, event)
      env.requiresPorts.insert(std::format("iaction {}_{}", call->receiver, call->name));
    }

    auto identifier = std::format("{}_{}", call->receiver, call->name);

    return koda::ReturnValue{identifier};
  }
}

Result<ReturnValue> Compiler::generateExpr(PExpr node, const MirrorNode& mirror, Environment& env)
{
  const auto& vMirror = *safeChild(mirror, "v", 0)
  IF_ALT(PId, node->v, generateId, vMirror, env)
  ELSE_IF_ALT(PStr, node->v, generateStr, vMirror, env)
  ELSE_IF_ALT(PInt, node->v, generateInt, vMirror, env)
  ELSE_IF_ALT(PFloat, node->v, generateFloat, vMirror, env)
  ELSE_IF_ALT(PCall, node->v, generateCall, vMirror, env)
  ELSE_IF_ALT(PNeg, node->v, generateNeg, vMirror, env)
  ELSE_IF_ALT(PNot, node->v, generateNot, vMirror, env)
  ELSE_IF_ALT(PBinOp, node->v, generateBinOp, vMirror, env)
  ELSE_IF_ALT(PEParen, node->v, generateParen, vMirror, env)

  return ReturnValue{};
}

Result<ReturnValue> Compiler::generateId(PId expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{
      .name = expr->value,
  };
}

Result<ReturnValue> Compiler::generateStr(PStr expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{
      .name = expr->value,
  };
}

Result<ReturnValue> Compiler::generateInt(PInt expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{
      .name = std::to_string(expr->value),
  };
}

Result<ReturnValue> Compiler::generateFloat(PFloat expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{
      .name = std::to_string(expr->value),
  };
}

Result<ReturnValue> Compiler::generateCall(PCall expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{};
}

Result<ReturnValue> Compiler::generateNeg(PNeg expr, const MirrorNode& node, Environment& env)
{
  auto result = generateExpr(expr->value, *safeChild(node, "value", 0), env);
  RETURN_ON_FAILURE(result);

  return ReturnValue{
      .name = "-" + result.Value().name,
  };
}

Result<ReturnValue> Compiler::generateNot(PNot expr, const MirrorNode& node, Environment& env)
{
  auto result = generateExpr(expr->value, *safeChild(node, "value", 0), env);
  RETURN_ON_FAILURE(result);

  return ReturnValue{
      .name = "!" + result.Value().name,
  };
}

Result<ReturnValue> Compiler::generateBinOp(PBinOp expr, const MirrorNode& node, Environment& env)
{
  auto resultA = generateExpr(expr->a, *safeChild(node, "a", 0), env);
  RETURN_ON_FAILURE(resultA);

  std::string aSide = resultA.Value().name;
  std::string bSide = "";
  if (expr->b)
  {
    auto resultB = generateExpr(expr->b, *safeChild(node, "b", 0), env);
    RETURN_ON_FAILURE(resultB);
    bSide = resultB.Value().name;
  }

  auto setExpression = [aSide, bSide](const std::string& op, bool unary) {
    if (!unary && bSide.empty())
      return Result<ReturnValue>::Failed("No left side of expression");

    return Result<ReturnValue>({
        .name = (unary ? op : "") + aSide + (unary ? "" : " " + op + " " + bSide),
    });
  };

  switch (expr->operation)
  {
    case koda::Expr::BinOp::Kind::Equal:
      return setExpression("=", false);
    case koda::Expr::BinOp::Kind::NotEqual:
      return setExpression("!=", false);
    case koda::Expr::BinOp::Kind::GreaterThan:
      return setExpression(">", false);
    case koda::Expr::BinOp::Kind::GreaterEqual:
      return setExpression(">=", false);
    case koda::Expr::BinOp::Kind::LessThan:
      return setExpression("<", false);
    case koda::Expr::BinOp::Kind::LessEqual:
      return setExpression("<=", false);
    case koda::Expr::BinOp::Kind::Addition:
      return setExpression("+", false);
    case koda::Expr::BinOp::Kind::Subtraction:
      return setExpression("-", false);
    case koda::Expr::BinOp::Kind::Multiplication:
      return setExpression("*", false);
    case koda::Expr::BinOp::Kind::Division:
      return setExpression("/", false);
    case koda::Expr::BinOp::Kind::Negation:
      return setExpression("!", true);
    case koda::Expr::BinOp::Kind::Unary:
      return setExpression("-", true);
    case koda::Expr::BinOp::Kind::Disjunction:
      return setExpression("||", true);
    case koda::Expr::BinOp::Kind::Conjunction:
      return setExpression("&&", true);
    default:
      return Result<ReturnValue>::Failed("Unknown operation");
  }
}

Result<ReturnValue> Compiler::generateParen(PEParen expr, const MirrorNode& node, Environment& env)
{
  return generateExpr(expr->value, *safeChild(node, "value", 0), env);
}

VoidResult Compiler::createSequenceComponent(uint32_t instances)
{
  if (mOptions.dryRun)
    return VoidResult();

  std::string filename = std::format("{}/sequence{}.dzn", mOptions.outputDir, instances);

  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return VoidResult::Failed("Failed to open: " + filename);

  file << "import types.dzn;\n";
  file << "import iaction.dzn;\n\n";

  file << std::format("component csequence{} {{\n", instances);
  file << "  provides iaction api;\n\n";

  for (uint32_t i = 0; i < instances; ++i)
    file << std::format("  requires iaction action{};\n", i);

  file << "\n";

  file << "  behaviour {\n";
  file << "    enum State { Idle, ";
  for (uint32_t i = 0; i < instances; ++i)
    file << std::format("Action{}, ", i);
  file << "Error };\n";
  file << "    State state = State.Idle;\n\n";

  file << "    [state.Idle] {\n";
  file << "      on api.trigger(): {\n";
  file << "        Result ret = action0.trigger();\n";
  file << "        if (ret.Success) {\n";
  file << "          state = State.Action0;\n";
  file << "        } else if (ret.Done) {\n";
  createSequenceDoneRecursion(true, 1, instances, file, "          ");
  file << "        } else {\n";
  file << "          state = State.Error;\n";
  file << "        }\n";
  file << "        reply(ret);\n";
  file << "      }\n";
  file << "    }\n\n";

  for (uint32_t i = 0; i < instances; ++i)
  {
    file << std::format("    [state.Action{}] {{\n", i);
    file << std::format("      on action{}.success(): {{\n", i);
    if (i + 1 == instances)
    {
      file << "        api.success();\n";
      file << "        state = State.Idle;\n";
    }
    else
    {
      file << std::format("        Result ret = action{}.trigger();\n", i + 1);
      file << "        if (ret.Success) {\n";
      file << std::format("          state = State.Action{};\n", i + 1);
      file << "        } else if (ret.Done) {\n";
      createSequenceDoneRecursion(false, i + 2, instances, file, "          ");
      file << "        } else {\n";
      file << "          api.failure();\n";
      file << "          state = State.Error;\n";
      file << "        }\n";
    }
    file << "      }\n\n";

    file << std::format("      on action{}.failure(): {{\n", i);
    file << "        api.failure();\n";
    file << "        state = State.Error;\n";
    file << "      }\n\n";

    file << "      on api.abort(): {\n";
    file << std::format("        Result ret = action{}.abort();\n", i);
    file << "        if (ret.Success)\n";
    file << "          state = State.Idle;\n";
    file << "        else if (ret.Failure)\n";
    file << "          state = State.Error;\n";
    file << "        reply(ret);\n";
    file << "      }\n";
    file << "    }\n\n";
  }

  file << "    [state.Error] {\n";
  file << "      on api.reset(): {\n";
  for (uint32_t i = 0; i < instances; ++i)
  {
    if (i == 0)
      file << std::format("        if (action{}.state.Error) {{\n", i);
    else
      file << std::format("        else if (action{}.state.Error) {{\n", i);
    file << std::format("          Result ret = action{}.reset();\n", i);
    file << "          if (ret.Success)\n";
    file << "            state = State.Idle;\n";
    file << "          reply(ret);\n";
    file << "        }\n";
  }
  file << "      }\n\n";

  file << "      on api.abort(): {\n";
  for (uint32_t i = 0; i < instances; ++i)
  {
    if (i == 0)
      file << std::format("        if (action{}.state.Error)\n", i);
    else
      file << std::format("        else if (action{}.state.Error)\n", i);

    file << std::format("          reply(action{}.abort());\n", i);
  }
  file << "      }\n";
  file << "    }\n";
  file << "  }\n";
  file << "}\n";

  file.close();
  mGeneratedFiles.push_back(filename);

  return VoidResult();
}

VoidResult Compiler::createActionArbiterComponent(uint32_t instances)
{
  if (mOptions.dryRun)
    return VoidResult();

  std::string filename = std::format("{}/action_arbiter{}.dzn", mOptions.outputDir, instances);

  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return VoidResult::Failed("Failed to open: " + filename);

  file << "import types.dzn;\n";
  file << "import iaction.dzn;\n\n";

  file << std::format("component caction_arbiter{} {{\n", instances);
  for (uint32_t i = 0; i < instances; ++i)
    file << std::format("  provides iaction client{};\n", i);

  file << "\n  requires iaction resource;\n\n";

  file << "  behaviour {\n";
  file << "    enum Owner { None, ";
  for (uint32_t i = 0; i < instances; ++i)
    file << std::format("C{}{}", i, (i + 1 == instances ? "" : ", "));
  file << "};\n";
  file << "    Owner owner = Owner.None;\n";
  file << "    Owner pending = Owner.None;\n";
  file << "    bool erroring = false;\n";
  file << "    bool succeeding = false;\n\n";

  file << "    Result handleAbort()\n";
  file << "    {\n";
  file << "      Result ret = resource.abort();\n";
  file << "      if (ret.Success)\n";
  file << "      {\n";
  file << "        owner = Owner.None;\n";
  file << "        pending = Owner.None;\n";
  file << "      }\n\n";

  file << "      return ret;\n";
  file << "    }\n\n";

  file << "    [owner.None] {\n";
  for (uint32_t i = 0; i < instances; ++i)
  {
    file << std::format("      on client{}.trigger(): {{\n", i);
    file << "        if (erroring) {\n";
    file << "          reply(Result.Failure);\n";
    file << "        } else {\n";
    file << "          Result ret = resource.trigger();\n";
    file << "          if (!ret.Done)\n";
    file << std::format("            owner = Owner.C{};\n", i);
    file << "          reply(ret);\n";
    file << "        }\n";
    file << "      }\n";
    file << std::format("      on client{}.abort(): {{\n", i);
    file << std::format("        if (client{}.state.Error)\n", i);
    file << "          reply(Result.Error);\n";
    file << "        else\n";
    file << "          reply(Result.Success);\n";
    file << "      }\n";
    file << std::format("      on client{}.reset(): {{ reply(Result.Success); }}\n\n", i);
  }
  file << "    }\n\n";

  file << "    on resource.success(): {\n";
  file << "      succeeding = true;\n";
  file << "      defer () {\n";
  for (uint32_t i = 0; i < instances; ++i)
  {
    file << std::format("        if (owner.C{} || pending.C{})\n", i, i);
    file << std::format("          client{}.success();\n", i);
  }
  file << "\n";
  file << "        owner = Owner.None;\n";
  file << "        pending = Owner.None;\n";
  file << "        succeeding = false;\n";
  file << "      }\n";
  file << "    }\n\n";

  file << "    on resource.failure(): {\n";
  file << "      erroring = true;\n";
  file << "      defer () {\n";
  for (uint32_t i = 0; i < instances; ++i)
  {
    file << std::format("        if (owner.C{} || pending.C{})\n", i, i);
    file << std::format("          client{}.failure();\n", i);
  }
  file << "\n";
  file << "        pending = Owner.None;\n";
  file << "        erroring = false;\n";
  file << "      }\n";
  file << "    }\n\n";

  for (uint32_t i = 0; i < instances; ++i)
  {
    file << std::format("    [owner.C{}] {{\n", i);
    file << std::format("      on client{}.abort(): {{\n", i);
    file << "        if (erroring)\n";
    file << "          reply(Result.Error);\n";
    file << "        else if (succeeding)\n";
    file << "          reply(Result.Success);\n";
    file << "        else\n";
    file << "          reply(handleAbort());\n";
    file << "      }\n\n";

    file << std::format("      on client{}.reset(): {{\n", i);
    file << "        Result ret = resource.reset();\n";
    file << "        if (ret.Success)\n";
    file << "        {\n";
    file << "          owner = Owner.None;\n";
    file << "          pending = Owner.None;\n";
    file << "        }\n";
    file << "        reply(ret);\n";
    file << "      }\n\n";

    for (uint32_t j = 0; j < instances; ++j)
    {
      if (j == i)
        continue;

      file << std::format("      on client{}.abort(): {{\n", j);
      file << std::format("        if (client{}.state.Error)\n", j);
      file << "          reply(Result.Error);\n";
      file << std::format("        else if (client{}.state.Idle)\n", j);
      file << "          reply(Result.Success);\n";
      file << "        else\n";
      file << "          reply(Result.Running);\n";
      file << "      }\n\n";

      file << std::format("      on client{}.reset(): {{ reply(Result.Failure); }}\n\n", j);

      file << std::format("      on client{}.trigger(): {{\n", j);
      file << "        if (resource.state.Error) {\n";
      file << "          reply(Result.Failure);\n";
      file << "        } else {\n";
      file << std::format("          pending = Owner.C{};\n", j);
      file << "          reply(Result.Success);\n";
      file << "        }\n";
      file << "      }\n";
    }
    file << "    }\n";
  }

  file << "  }\n";
  file << "}\n";

  file.close();
  mGeneratedFiles.push_back(filename);

  return VoidResult();
}

void Compiler::createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances, std::ofstream& file, const std::string& indent)
{
  if (start >= instances)
  {
    if (fromIdle)
    {
      file << std::format("{}state = State.Idle;\n", indent);
    }
    else
    {
      file << std::format("{}api.success();\n", indent);
      file << std::format("{}state = State.Idle;\n", indent);
    }
  }
  else
  {
    file << std::format("{}ret = action{}.trigger();\n", indent, start);
    file << std::format("{}if (ret.Success) {{\n", indent);
    file << std::format("{}  state = State.Action{};\n", indent, start);
    file << std::format("{}}} else if (ret.Done) {{\n", indent);
    createSequenceDoneRecursion(fromIdle, start + 1, instances, file, indent + "  ");
    file << std::format("{}}} else if (ret.Failure) {{\n", indent);
    if (!fromIdle)
      file << std::format("{}  api.failure();\n", indent);

    file << std::format("{}  state = State.Error;\n", indent);
    file << std::format("{}}}\n", indent);
  }
}

std::string Compiler::createPort(const Action& action, bool in) const
{
  return std::format("  provides {} {};\n", in ? "iaction" : "isignal", action.name);
}

std::string Compiler::toFilename(const std::string& name) const
{
  return ToLowerCase(name, 0, name.size() - 1);
}

std::string Compiler::componentName(const std::string& name) const
{
  return "c" + toFilename(name);
}

std::string Compiler::flowName(const std::string& name) const
{
  return "f" + toFilename(name);
}

void Compiler::connectWithArbiter(Environment& env)
{
  struct PortRefType
  {
    std::vector<PortRef> ports;
    Connection::Type type;
  };

  std::map<std::string, PortRefType> refs;
  for (const auto& conn : env.system.connections)
  {
    auto in = conn.lhs;
    auto out = conn.rhs;

    auto outId = std::format("{}.{}", out.instance, out.port);
    if (refs.contains(outId))
      refs[outId].ports.push_back(in);
    else
      refs[outId] = {{in}, conn.type};
  }

  uint32_t arbiterId = 0;
  for (const auto& ref : refs)
  {
    // If there is only one connection, we don't need to create an arbiter
    auto occurences = ref.second.ports.size();
    if (occurences < 2)
      continue;

    auto id = arbiterId++;
    auto arbiter = std::format("arbiter{}_{}", occurences, id);
    if (ref.second.type == Connection::Type::Action)
    {
      createActionArbiterComponent(occurences);
      env.system.instances.insert({std::format("caction_arbiter{}", occurences), arbiter});
      env.includes.insert(std::format("action_arbiter{}.dzn", occurences));
    }
    else if (ref.second.type == Connection::Type::Signal)
    {
      env.system.instances.insert({std::format("csignal_arbiter{}", ""), arbiter});
      env.includes.insert(std::format("signal_arbiter{}.dzn", ""));
    }

    auto [instance, port] = portFromString(ref.first);

    uint32_t clientId = 0;
    for (auto& conn : env.system.connections)
    {
      if (conn.rhs.instance == instance && conn.rhs.port == port)
      {
        conn.rhs.instance = arbiter;
        conn.rhs.port = std::format("client{}", clientId++);
      }
    }

    env.system.connections.push_back({{arbiter, "resource"}, {instance, port}});
  }
}

Compiler::PortRef Compiler::portFromString(const std::string& ref) const
{
  auto index = ref.find_first_of(".");
  if (index == std::string::npos)
    return PortRef{ref, ""};

  return PortRef{ref.substr(0, index), ref.substr(index + 1, ref.size() - index)};
}

}  // namespace koda