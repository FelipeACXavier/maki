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

// ------------------------------------------------------------------
// Line mapping helpers
// ------------------------------------------------------------------

void Compiler::startFile(const std::string& filename)
{
  mLineCounters[filename] = 0;
}

void Compiler::emitLine(std::ofstream& stream, const std::string& filename,
                        const std::string& text, const std::string& srcId)
{
  stream << text << "\n";
  int line = ++mLineCounters[filename];
  if (!srcId.empty())
    mLineMappings[filename][line] = srcId;
}

void Compiler::writeLineMapping()
{
  std::string path = mOptions.outputDir + "/koda_line_mapping.json";
  std::ofstream out(path);
  if (!out.is_open())
  {
    LOG_ERROR("Failed to write line mapping to %s", path.c_str());
    return;
  }

  nlohmann::json j;
  for (const auto& [file, mapping] : mLineMappings)
    for (const auto& [line, src] : mapping)
      j[file][std::to_string(line)] = src;

  out << j.dump(2);
  out.close();
  LOG_INFO("Line mapping written to %s", path.c_str());
}

// ------------------------------------------------------------------
// Public generation entry
// ------------------------------------------------------------------

VoidResult Compiler::generate()
{
  mGeneratedFiles.clear();
  mLineMappings.clear();

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
      MirrorNode componentMirror = *safeChild(mMirrorRoot, "components", compIdx);
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
      MirrorNode componentMirror = *safeChild(mMirrorRoot, "components", compIdx);
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

  writeLineMapping();

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

  size_t stmntIdx = 0;
  for (auto& statement : task->statements)
  {
    if (safeChild(*safeChild(node, "statements", stmntIdx), "node", 0)->ASTtype == "VarsBlock" &&
        safeChild(*safeChild(node, "statements", stmntIdx), "node", 0)->group("vars").empty()) {
      stmntIdx++;
      LOG_DEBUG("Skipping empty vars block at index %zu", stmntIdx - 1);
    }
    RETURN_ON_FAILURE(generateStatement(statement, *safeChild(node, "statements", stmntIdx), env));
    stmntIdx++;
  }

  for (const auto& f : env.flows)
  {
    const auto& flow = f.second;
    const auto flowName = flow.name;

    // Async calls (each occurrence gets its own port)
    for (const auto& [identifier, srcId] : flow.asyncCalls)
    {
      auto index = identifier.find_first_of("_");
      auto capName = identifier.substr(0, index);
      auto cap = env.getCapability(capName);
      std::string name = "";
      std::string trigger = "";
      if (!cap)
      {
        if (!env.flows.contains(capName))
          return Result<koda::ReturnValue>::Failed("Could not find async capability: " + identifier);
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
          trigger = std::format("{}_{}", cap->trigger->name, identifier.substr(index + 1));
      }
      PortRef in = {toFlowVariable(flowName), identifier};
      PortRef out = {toFilename(name), trigger};
      env.system.connections.emplace_back(Connection{in, out, Connection::Type::Action, flow.srcId});
    }

    // Sync calls (unique – map stores {count, srcId})
    for (const auto& [fullName, countSrcPair] : flow.syncCalls)
    {
      const auto& srcId = countSrcPair.second;   // first seen srcId
      auto [instance, port] = portFromString(fullName);
      auto cap = env.getCapability(instance);
      std::string name = "";
      if (!cap)
      {
        if (!env.flows.contains(instance))
          return Result<koda::ReturnValue>::Failed("Could not find sync capability: " + fullName);
        auto tmp = env.flows.at(instance);
        name = tmp.name;
      }
      else
        name = cap->name;

      PortRef in = {toFlowVariable(flowName), std::format("{}_{}", instance, port)};
      PortRef out = {toFilename(cap->name), port};
      env.system.connections.emplace_back(Connection{in, out, Connection::Type::Action, flow.srcId});
    }

    // Signal calls (unique)
    for (const auto& [fullName, countSrcPair] : flow.signalCalls)
    {
      const auto& srcId = countSrcPair.second;
      auto [instance, port] = portFromString(fullName);
      auto cap = env.getCapability(instance);
      if (!cap)
        return Result<koda::ReturnValue>::Failed("  signal capability: " + fullName);

      PortRef in = {toFlowVariable(flowName), std::format("{}_{}", instance, port)};
      PortRef out = {toFilename(cap->name), port};
      env.system.connections.emplace_back(Connection{in, out, Connection::Type::Signal, flow.srcId});
    }

    // Strategies (unique)
    for (const auto& [strategyName, countSrcPair] : flow.strategies)
    {
      const auto& srcId = countSrcPair.second;
      PortRef in = {toFlowVariable(flowName), strategyName};
      PortRef out = {toFlowVariable(strategyName), "api"};
      env.system.connections.emplace_back(Connection{in, out, Connection::Type::Action, flow.srcId});
    }
  }

  env.includes = {};
  connectWithArbiter(env);

  if (mOptions.dryRun)
    return koda::ReturnValue();

  // Write task component file
  std::string filename = std::format("{}/{}_task.dzn", mOptions.outputDir, toFilename(task->name));
  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  startFile(filename);

  bool needsSep = false;

  emitLine(file, filename, "import iaction.dzn;", node.srcId);
  emitLine(file, filename, "import isignal.dzn;", node.srcId);
  needsSep = true;

  if (env.alarm > 0)
  {
    if (needsSep) { emitLine(file, filename, "", node.srcId); needsSep = false; }
    emitLine(file, filename, "import alarm.dzn;", node.srcId);
    needsSep = true;
  }

  if (!env.includes.empty())
  {
    if (needsSep) { emitLine(file, filename, "", node.srcId); needsSep = false; }
    for (const auto& inc : env.includes)
      emitLine(file, filename, std::format("import {};", inc), node.srcId);
    needsSep = true;
  }

  if (!env.capabilities.empty())
  {
    if (needsSep) { emitLine(file, filename, "", node.srcId); needsSep = false; }
    for (const auto& cap : env.capabilities)
      emitLine(file, filename, std::format("import a_{}.dzn;", toFilename(cap.second.name)), cap.second.srcId);
    needsSep = true;
  }

  if (!env.flows.empty())
  {
    if (needsSep) { emitLine(file, filename, "", node.srcId); needsSep = false; }
    for (const auto& flow : env.flows)
      emitLine(file, filename, std::format("import {}.dzn;", toFilename(flow.second.name)), node.srcId);
    needsSep = true;
  }

  if (needsSep)
    emitLine(file, filename, "", node.srcId);

  emitLine(file, filename, std::format("component {} {{", componentName(task->name)), node.srcId);
  emitLine(file, filename, "  provides iaction api;", node.srcId);
  emitLine(file, filename, "", node.srcId);
  emitLine(file, filename, "  system {", node.srcId);

  for (const auto& instance : env.system.instances)
    emitLine(file, filename, std::format("    {} {};", instance.type, instance.name), instance.srcId);

  emitLine(file, filename, "", node.srcId);
  emitLine(file, filename, "    api <=> main.api;", node.srcId);
  emitLine(file, filename, "", node.srcId);

  std::string currentInstance;
  for (const auto& conn : env.system.connections)
  {
    if (conn.lhs.instance != currentInstance)
    {
      if (!currentInstance.empty())
        emitLine(file, filename, "", conn.srcId);
      currentInstance = conn.lhs.instance;
    }
    emitLine(file, filename,
             std::format("    {}.{} <=> {}.{};", conn.lhs.instance, conn.lhs.port, conn.rhs.instance, conn.rhs.port),
             conn.srcId);
  }

  emitLine(file, filename, "  }", node.srcId);
  emitLine(file, filename, "}", node.srcId);

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

  size_t stmntIdx = 0;
  for (const auto& statement : capability->statements)
  {
    RETURN_ON_FAILURE(generateStatement(statement, *safeChild(node, "statements", stmntIdx), env));
    stmntIdx++;
  }

  env.capabilities[capability->name] = env.currentCapability;
  env.system.instances.insert({componentName(capability->name), toFilename(capability->name), node.srcId});

  return koda::ReturnValue();
}

Result<ReturnValue> Compiler::emitCapability(PComponent capability, Environment& env)
{
  if (mOptions.dryRun)
    return koda::ReturnValue();

  std::string filename = std::format("{}/a_{}.dzn", mOptions.outputDir, toFilename(capability->name));
  std::ofstream file;
  file.open(filename);
  if (!file.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  startFile(filename);

  auto cap = env.capabilities[capability->name];

  emitLine(file, filename, "import iaction.dzn;", cap.srcId);
  emitLine(file, filename, "", cap.srcId);
  emitLine(file, filename, std::format("component {} {{", componentName(capability->name)), cap.srcId);

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
        emitLine(file, filename, createPort(action, true), action.srcId);
      }
    }
    else
    {
      LOG_ERROR("No such capability in the counter map: %s", name.c_str());
    }
  }
  if (cap.abort)
    emitLine(file, filename, createPort(*cap.abort, true), cap.abort->srcId);
  for (const auto& in : cap.ins)
    emitLine(file, filename, createPort(in, true), in.srcId);
  for (const auto& out : cap.outs)
    emitLine(file, filename, createPort(out, false), out.srcId);

  emitLine(file, filename, "}", cap.srcId);

  file.close();
  mGeneratedFiles.push_back(filename);

  return koda::ReturnValue();
}

Result<ReturnValue> Compiler::generateArgument(PArgument argument, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{argument->a, argument->b, {}, node.srcId};
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
  size_t flowIdx = 0;
  for (auto& flow : strategy->flows)
  {
    MirrorNode flowMirror = *safeChild(node, "flows", flowIdx);
    if (!(flowMirror.ASTtype == "Flow") || !(flowMirror.name == flow->name))
      LOG_ERROR("Mirror AST flow mismatch at index %zu: expected Flow with name '%s', found '%s' with name '%s'",
                flowIdx, flow->name.c_str(), flowMirror.ASTtype.c_str(), flowMirror.name.c_str());
    RETURN_ON_FAILURE(generateFlow(flow, flowMirror, env));
    flowIdx++;
  }

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateActionDef(PActionDef action, const MirrorNode& node, Environment& env)
{
  env.currentCapability.title = action->label1;
  env.currentCapability.message = action->label2;
  size_t rdIdx = 0;
  for (const auto& def : action->rosDefs)
  {
    RETURN_ON_FAILURE(generateRosDef(def, *safeChild(node, "rosDefs", rdIdx), env));
    rdIdx++;
  }
  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateRosDef(PRosDef ros, const MirrorNode& node, Environment& env)
{
  ReturnValue result;
  const auto& defMirror = *safeChild(node, "def", 0);
  if (!(defMirror.ASTtype == "EventDef") || !(defMirror.name == ros->def->name))
    LOG_ERROR("Mirror AST event definition mismatch: expected EventDef with name '%s', found '%s' with name '%s'",
              ros->def->name.c_str(), defMirror.ASTtype.c_str(), defMirror.name.c_str());
  ASSIGN_OR_RETURN_ON_FAILURE(result, generateEventDef(ros->def, defMirror, env));

  Action action;
  action.name = result.call;
  action.args = result.args;
  action.srcId = node.srcId;  // RosDef's srcId

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
  value.srcId = node.srcId;
  size_t argIdx = 0;
  for (const auto& arg : event->args)
  {
    koda::ReturnValue argValue;
    ASSIGN_OR_RETURN_ON_FAILURE(argValue, generateArgument(arg, *safeChild(node, "args", argIdx), env));
    value.args[argValue.call] = argValue.name;
    argIdx++;
  }

  return value;
}

Result<ReturnValue> Compiler::generateVarsBlock(PVarsBlock varsBlock, const MirrorNode& node, Environment& env)
{
  size_t varsIdx = 0;
  for (const auto& var : varsBlock->vars)
  {
    MirrorNode varMirror = *safeChild(node, "vars", varsIdx);
    if (!(varMirror.ASTtype == "Vars") || !(varMirror.name == var->name))
      LOG_ERROR("Mirror AST vars definition mismatch at index %zu: expected Vars with name '%s', found '%s' with name '%s'",
                varsIdx, var->name.c_str(), varMirror.ASTtype.c_str(), varMirror.name.c_str());
    RETURN_ON_FAILURE(generateVarsDef(var, varMirror, env));
    varsIdx++;
  }

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateVarsDef(PVarDef var, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{};
}

void Compiler::connectWithArbiter(const std::map<std::string, uint32_t>& connections,
                                  Connection::Type connectionType, Environment& env)
{
  for (auto it = connections.cbegin(); it != connections.cend(); ++it)
  {
    auto occurences = it->second;
    if (occurences < 2)
      continue;

    createActionArbiterComponent(occurences);

    int count = 0;
    env.includes.insert(std::format("action_arbiter{}.dzn", occurences));

    auto id = env.arbiter++;
    env.definitions.push_back({std::format("caction_arbiter{} arbitrer{}", occurences, id), ""});

    auto [instance, port] = portFromString(it->first);
    if (mOptions.verbose > 0)
      LOG_RAW("Arbiter evaluation, {} {} has {} connections", instance, port, it->second);

    const auto toReplace = connectionType == Connection::Type::Signal ? std::format("{}_{}", instance, port) : instance;
    for (auto& lineSrc : env.core)
    {
      auto index0 = lineSrc.text.find(toReplace);
      if (index0 != std::string::npos)
        lineSrc.text.replace(index0, toReplace.size(), std::format("arbitrer{}.client{}", id, count++));
    }

    env.core.push_back({std::format("arbitrer{}.resource <=> {}", id, toReplace), ""});
  }
}

Result<koda::ReturnValue> Compiler::generateFlow(PFlow flow, const MirrorNode& node, Environment& env)
{
  if (mOptions.verbose > 0)
    LOG_RAW("Generating flow: {}", flow->name);

  env.clear();
  const auto& strategyMirror = *safeChild(node, "strategy", 0);
  auto ret = generateStrategy(flow->strategy, strategyMirror, env);

  Flow f;
  f.name = flow->name;
  f.srcId = node.srcId;
  f.asyncCalls = std::move(env.asyncCallsWithSrc);

  // Build unique call maps using the first seen srcId
  for (const auto& [key, count] : env.syncCallsCountMap)
    f.syncCalls[key] = {count, env.syncCallsSrcMap[key]};
  for (const auto& [key, count] : env.signalCallsCountMap)
    f.signalCalls[key] = {count, env.signalCallsSrcMap[key]};
  for (const auto& [key, count] : env.strategiesCountMap)
    f.strategies[key] = {count, env.strategiesSrcMap[key]};

  env.flows[flow->name] = f;
  env.system.instances.insert({flowName(flow->name), toFlowVariable(toFilename(flow->name)), node.srcId});

  if (mOptions.verbose > 0)
    env.print();

  connectWithArbiter(env.strategiesCountMap, Connection::Type::Action, env);
  connectWithArbiter(env.syncCallsCountMap, Connection::Type::Signal, env);
  connectWithArbiter(env.signalCallsCountMap, Connection::Type::Signal, env);

  env.core.push_front({std::format("api <=> {}", ret.Value().name), ret.Value().srcId});
  env.includes.insert("iaction.dzn");

  if (mOptions.dryRun)
    return ret;

  std::string filename = std::format("{}/{}.dzn", mOptions.outputDir, flow->name);
  mCurrentFile.open(filename);
  if (!mCurrentFile.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  startFile(filename);

  bool needsSep = false;
  if (!env.includes.empty())
  {
    for (const auto& i : env.includes)
      emitLine(mCurrentFile, filename, "import " + i + ";", node.srcId);
    needsSep = true;
  }

  if (needsSep)
    emitLine(mCurrentFile, filename, "", node.srcId);

  emitLine(mCurrentFile, filename, std::format("component {} {{", flowName(flow->name)), node.srcId);
  emitLine(mCurrentFile, filename, "  provides iaction api;", node.srcId);
  emitLine(mCurrentFile, filename, "", node.srcId);

  for (const auto& c : env.requiresPorts)
    emitLine(mCurrentFile, filename, "  requires " + c + ";", node.srcId);

  emitLine(mCurrentFile, filename, "", node.srcId);
  emitLine(mCurrentFile, filename, "  system {", node.srcId);

  for (const auto& def : env.definitions)
    emitLine(mCurrentFile, filename, "    " + def.text + ";", def.srcId);

  emitLine(mCurrentFile, filename, "", node.srcId);
  for (const auto& lineSrc : env.core)
{
    LOG_DEBUG("Writing line [%s] with srcId [%s]", lineSrc.text.c_str(), lineSrc.srcId.c_str());
    emitLine(mCurrentFile, filename, "    " + lineSrc.text + ";", lineSrc.srcId);
}

  emitLine(mCurrentFile, filename, "  }", node.srcId);
  emitLine(mCurrentFile, filename, "}", node.srcId);

  mCurrentFile.close();
  mGeneratedFiles.push_back(filename);

  return ret;
}

Result<koda::ReturnValue> Compiler::generateStrategy(PStrategy strategy, const MirrorNode& node, Environment& env)
{
  const MirrorNode* childMirrorPtr;
  if (skipStrategy) {
    // second part of hacky fix introduced in generateParen. Might be needed for other strategies too, but currently not known.
    skipStrategy = false;
    childMirrorPtr = &node;
  } else {
    childMirrorPtr = safeChild(node, "v", 0);
  }
  const MirrorNode& childMirror = *childMirrorPtr;
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
    if (std::get_if<koda::PContinue>(&(*it)->v))
      it = strategy->alts.erase(it);
    else
      ++it;
  }

  auto instances = strategy->alts.size();
  if (instances == 1)
    return generateStrategy(strategy->alts.at(0), *safeChild(node, "alts", 0), env);

  auto id = env.sequence++;
  auto created = createSequenceComponent(instances);
  if (!created.IsSuccess())
    return Result<koda::ReturnValue>::Failed(created.ErrorMessage());

  env.includes.insert(std::format("sequence{}.dzn", instances));
  env.definitions.push_back({std::format("csequence{} s{}", instances, id), node.srcId});

  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], *safeChild(node, "alts", i), env));
    env.core.push_back({std::format("s{}.action{} <=> {}", id, i, expr.name), expr.srcId});
  }

  return koda::ReturnValue{std::format("s{}.api", id),  "", {}, node.srcId};
}

Result<koda::ReturnValue> Compiler::generateJoin(PJoin strategy, const MirrorNode& node, Environment& env)
{
  auto id = env.join++;
  env.includes.insert("parallel.dzn");
  env.definitions.push_back({std::format("cparallel p{}", id), node.srcId});

  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], *safeChild(node, "alts", i), env));
    env.core.push_back({std::format("p{}.action{} <=> {}", id, i, expr.name), expr.srcId});
  }

  return koda::ReturnValue{std::format("p{}.api", id),  "", {}, node.srcId};
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
  env.definitions.push_back({std::format("cwithin w{}", id), node.srcId});

  env.system.instances.insert({"calarm", std::format("alarm{}", alarmId), node.srcId});

  env.core.push_back({std::format("w{}.action1 <=> {}", id, exprDo.name), exprDo.srcId});
  env.core.push_back({std::format("w{}.action2 <=> {}", id, exprElse.name), exprElse.srcId});
  env.core.push_back({std::format("w{}.alarm <=> alarm{}", id, alarmId), node.srcId});

  INCREMENT_MAP(env.strategiesCountMap, std::format("alarm{}", alarmId));
  env.requiresPorts.insert(std::format("ialarm alarm{}", alarmId));

  return koda::ReturnValue{std::format("w{}.api", id),  "", {}, node.srcId};
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
  env.definitions.push_back({std::format("crepeat r{}", id), node.srcId});

  size_t handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
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
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
    handlerIdx++;
  }

  env.core.push_back({std::format("r{}.action1 <=> {}", id, expr.name), expr.srcId});

  return koda::ReturnValue{std::format("r{}.api", id), node.srcId};
}

Result<koda::ReturnValue> Compiler::generateGuard(PGuard strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateEvery(PRepeat strategy, const MirrorNode& node, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->a, *safeChild(node, "a", 0), env));

  auto id = env.every++;
  auto alarmId = env.alarm++;

  env.includes.insert("every.dzn");
  env.includes.insert("ialarm.dzn");
  env.definitions.push_back({std::format("cevery e{}", id), node.srcId});

  env.system.instances.insert({"calarm", std::format("alarm{}", alarmId), node.srcId});

  INCREMENT_MAP(env.strategiesCountMap, std::format("alarm{}", alarmId));
  env.requiresPorts.insert(std::format("ialarm alarm{}", alarmId));

  size_t handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
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
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
    handlerIdx++;
  }

  env.core.push_back({std::format("e{}.action1 <=> {}", id, expr.name), expr.srcId});
  env.core.push_back({std::format("e{}.alarm <=> alarm{}", id, alarmId), node.srcId});

  return koda::ReturnValue{std::format("e{}.api", id),  "", {}, node.srcId};
}

Result<koda::ReturnValue> Compiler::generateEnd(PEnd strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{"end", "", {},  node.srcId};
}

Result<ReturnValue> Compiler::generateContinue(PContinue strategy, const MirrorNode& node, Environment& env)
{
  return koda::ReturnValue{"continue", "", {},  node.srcId};
}

Result<koda::ReturnValue> Compiler::generateRef(PRef strategy, const MirrorNode& node, Environment& env)
{
  INCREMENT_MAP(env.strategiesCountMap, strategy->name)
  if (env.strategiesSrcMap.find(strategy->name) == env.strategiesSrcMap.end())
    env.strategiesSrcMap[strategy->name] = node.srcId;
  env.requiresPorts.insert(std::format("iaction {}", strategy->name));
  return koda::ReturnValue{strategy->name, "", {},  node.srcId};
}

Result<koda::ReturnValue> Compiler::generateTaskCall(PTaskCall strategy, const MirrorNode& node, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(strategy->call, *safeChild(node, "call", 0), env, false));

  if (strategy->handlers.empty())
    return expr;

  size_t handlerIdx = 0;
  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
    {
      handlerIdx++;
      continue;
    }
    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
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
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, *safeChild(node, "handlers", handlerIdx), env));
    handlerIdx++;
  }

  return expr;
}

Result<koda::ReturnValue> Compiler::generateParen(PParen strategy, const MirrorNode& node, Environment& env)
{
  if (node.ASTtype != "Strategy::Paren") {
    // Hacky fix to make sure that we can skip the Paren node and its child strategy, given that these are not present in the original AST due to optimizations.
    LOG_DEBUG("Skipping Parentheses in mirror AST.");
    skipStrategy = true;
    return generateStrategy(strategy->a, node, env);
  }
  return generateStrategy(strategy->a, *safeChild(node, "a", 0), env);
}

Result<ReturnValue> Compiler::generateStrategyHandler(PStrategyHandler handler, const MirrorNode& node, Environment& env)
{
  uint32_t id = 0;
  if (handler->kind == koda::StrategyHandler::Kind::OnAbort)
  {
    id = env.abortHandler++;
    env.includes.insert("abort_handler.dzn");
    env.definitions.push_back({std::format("cabort_handler ah{}", id), node.srcId});

    if (handler->emitter)
    {
      ReturnValue expr;
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, false));
      LOG_DEBUG("Emitter srcId for %s: %s", expr.name.c_str(), expr.srcId.c_str());
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));
      env.core.push_back({std::format("ah{}.action <=> {}", id, env.previousCall), node.srcId});
      env.core.push_back({std::format("ah{}.handler <=> {}", id, strat.name), strat.srcId});
    }

    return koda::ReturnValue{std::format("ah{}.api", id), "", {},  node.srcId};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnError)
  {
    id = env.errorHandler++;
    env.includes.insert("error_handler.dzn");
    env.definitions.push_back({std::format("cerror_handler fh{}", id), node.srcId});

    if (handler->emitter)
    {
      ReturnValue expr;
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, false));
      LOG_DEBUG("Emitter srcId for %s: %s", expr.name.c_str(), expr.srcId.c_str());
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));
      env.core.push_back({std::format("fh{}.action <=> {}", id, env.previousCall), node.srcId});
      env.core.push_back({std::format("fh{}.handler <=> {}", id, strat.name), strat.srcId});
    }

    return koda::ReturnValue{std::format("fh{}.api", id), "", {}, node.srcId};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
  {
    id = env.signalHandler++;
    env.includes.insert("signal_handler.dzn");
    env.definitions.push_back({std::format("csignal_handler sh{}", id), node.srcId});

    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, true));
    LOG_DEBUG("Emitter srcId for %s: %s", expr.name.c_str(), expr.srcId.c_str());

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.name);

    ReturnValue strat;
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, *safeChild(node, "body", 0), env));
    env.core.push_back({std::format("sh{}.signal <=> {}", id, expr.name), expr.srcId});
    env.core.push_back({std::format("sh{}.action <=> {}", id, env.previousCall), node.srcId});
    env.core.push_back({std::format("sh{}.handler <=> {}", id, strat.name), strat.srcId});

    return koda::ReturnValue{std::format("sh{}.api", id), "", {}, node.srcId};
  }
  else if (handler->kind == koda::StrategyHandler::Kind::OnEmitterContinue)
  {
    MirrorNode bodyCorrected;
    if (safeChild(*safeChild(node, "body", 0), "v", 0)->ASTtype=="Strategy::Seq") {
      bodyCorrected = *safeChild(*safeChild(*safeChild(node, "body", 0), "v", 0), "alts", 0);
    } else {
      bodyCorrected = *safeChild(node, "body", 0);
    }
    id = env.signalHandler++;
    std::string srcId = node.srcId;
    env.includes.insert("signal_continue.dzn");
    env.definitions.push_back({std::format("csignal_continue sh{}", id), srcId});

    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, *safeChild(node, "emitter", 0), env, true));
    LOG_DEBUG("Emitter srcId for %s: %s", expr.name.c_str(), srcId);

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.name);

    ReturnValue strat;
    // Hacky fix to mirror optimization in mirror nodes.
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, bodyCorrected, env));
    env.core.push_back({std::format("sh{}.signal <=> {}", id, expr.name), srcId});
    env.core.push_back({std::format("sh{}.action <=> {}", id, env.previousCall), srcId});
    env.core.push_back({std::format("sh{}.handler <=> {}", id, strat.name), srcId});

    return koda::ReturnValue{std::format("sh{}.api", id), "", {}, node.srcId};
  }

  return koda::ReturnValue{};
}

Result<koda::ReturnValue> Compiler::generateEventCall(PEventCall call, const MirrorNode& node, Environment& env, bool isSignal)
{
  if (mOptions.verbose > 1)
    LOG_RAW("Generating event call: {} receiver: {}", call->name, call->receiver);

  std::string srcId = node.srcId;
  LOG_DEBUG("EventCall %s.%s → srcId: %s", call->receiver.c_str(), call->name.c_str(), srcId.c_str());

  if (call->receiver.empty())
  {
    INCREMENT_MAP(env.asyncCallsCounter, call->name)
    auto identifier = std::format("{}_{}", call->name, env.asyncCallsCounter[call->name]);
    env.asyncCallsWithSrc.emplace_back(identifier, srcId);
    env.requiresPorts.insert(std::format("iaction {}", identifier));

    size_t argIdx = 0;
    for (const auto& expr : call->args)
    {
      auto ret = generateExpr(expr, *safeChild(node, "args", argIdx), env);
      if (!ret.IsSuccess())
        LOG_ERROR(ret.ErrorMessage());
      argIdx++;
    }

    return koda::ReturnValue{identifier, "", {}, srcId};
  }
  else
  {
    auto event = std::format("{}.{}", call->receiver, call->name);
    if (isSignal)
    {
      INCREMENT_MAP(env.signalCallsCountMap, event)
      if (env.signalCallsSrcMap.find(event) == env.signalCallsSrcMap.end())
        env.signalCallsSrcMap[event] = srcId;
      env.requiresPorts.insert(std::format("isignal {}_{}", call->receiver, call->name));
    }
    else
    {
      INCREMENT_MAP(env.syncCallsCountMap, event)
      if (env.syncCallsSrcMap.find(event) == env.syncCallsSrcMap.end())
        env.syncCallsSrcMap[event] = srcId;
      env.requiresPorts.insert(std::format("iaction {}_{}", call->receiver, call->name));
    }
    return koda::ReturnValue{std::format("{}_{}", call->receiver, call->name),"", {}, srcId};
  }
}

Result<ReturnValue> Compiler::generateExpr(PExpr node, const MirrorNode& mirror, Environment& env)
{
  const auto& vMirror = *safeChild(mirror, "v", 0);
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
  return ReturnValue{expr->value, {}, {}, node.srcId};
}

Result<ReturnValue> Compiler::generateStr(PStr expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{expr->value, {}, {}, node.srcId};
}

Result<ReturnValue> Compiler::generateInt(PInt expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{std::to_string(expr->value), {}, {}, node.srcId};
}

Result<ReturnValue> Compiler::generateFloat(PFloat expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{std::to_string(expr->value), {}, {}, node.srcId};
}

Result<ReturnValue> Compiler::generateCall(PCall expr, const MirrorNode& node, Environment& env)
{
  return ReturnValue{};
}

Result<ReturnValue> Compiler::generateNeg(PNeg expr, const MirrorNode& node, Environment& env)
{
  auto result = generateExpr(expr->value, *safeChild(node, "value", 0), env);
  RETURN_ON_FAILURE(result);
  return ReturnValue{"-" + result.Value().name, {}, {}, node.srcId};
}

Result<ReturnValue> Compiler::generateNot(PNot expr, const MirrorNode& node, Environment& env)
{
  auto result = generateExpr(expr->value, *safeChild(node, "value", 0), env);
  RETURN_ON_FAILURE(result);
  return ReturnValue{"!" + result.Value().name, {}, {}, node.srcId};
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

  auto setExpression = [&](const std::string& op, bool unary) -> Result<ReturnValue> {
    if (!unary && bSide.empty())
      return Result<ReturnValue>::Failed("No right side of expression");
    return ReturnValue{
      (unary ? op : "") + aSide + (unary ? "" : " " + op + " " + bSide),
      {}, {}, node.srcId
    };
  };

  switch (expr->operation)
  {
    case koda::Expr::BinOp::Kind::Equal:            return setExpression("=", false);
    case koda::Expr::BinOp::Kind::NotEqual:         return setExpression("!=", false);
    case koda::Expr::BinOp::Kind::GreaterThan:      return setExpression(">", false);
    case koda::Expr::BinOp::Kind::GreaterEqual:     return setExpression(">=", false);
    case koda::Expr::BinOp::Kind::LessThan:         return setExpression("<", false);
    case koda::Expr::BinOp::Kind::LessEqual:        return setExpression("<=", false);
    case koda::Expr::BinOp::Kind::Addition:         return setExpression("+", false);
    case koda::Expr::BinOp::Kind::Subtraction:      return setExpression("-", false);
    case koda::Expr::BinOp::Kind::Multiplication:   return setExpression("*", false);
    case koda::Expr::BinOp::Kind::Division:         return setExpression("/", false);
    case koda::Expr::BinOp::Kind::Negation:         return setExpression("!", true);
    case koda::Expr::BinOp::Kind::Unary:            return setExpression("-", true);
    case koda::Expr::BinOp::Kind::Disjunction:      return setExpression("||", false);
    case koda::Expr::BinOp::Kind::Conjunction:      return setExpression("&&", false);
    default:                                        return Result<ReturnValue>::Failed("Unknown operation");
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

  // Synthetic component – not mapped to source lines
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

  // Synthetic component – not mapped
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
      if (j == i) continue;
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

void Compiler::createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances,
                                           std::ofstream& file, const std::string& indent)
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
  return std::format("  provides {} {};", in ? "iaction" : "isignal", action.name);
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
  connectWithArbiter(env.strategiesCountMap, Connection::Type::Action, env);
  connectWithArbiter(env.syncCallsCountMap, Connection::Type::Signal, env);
  connectWithArbiter(env.signalCallsCountMap, Connection::Type::Signal, env);
}

Compiler::PortRef Compiler::portFromString(const std::string& ref) const
{
  auto index = ref.find_first_of(".");
  if (index == std::string::npos)
    return PortRef{ref, ""};

  return PortRef{ref.substr(0, index), ref.substr(index + 1, ref.size() - index)};
}

}  // namespace koda