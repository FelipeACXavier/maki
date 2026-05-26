#include "koda_compiler.h"

#include <format>
#include <fstream>

#include "KodaLexer.h"
#include "KodaParser.h"
#include "antlr4-runtime.h"
#include "cst2ast.h"
#include "error_listener.h"

#define IF_ALT(ALT, OBJ, CALL, ARGS)    \
  if (std::holds_alternative<ALT>(OBJ)) \
  {                                     \
    if (auto obj = std::get<ALT>(OBJ))  \
      return CALL(obj, ARGS);           \
  }

#define ELSE_IF_ALT(ALT, OBJ, CALL, ARGS)    \
  else if (std::holds_alternative<ALT>(OBJ)) \
  {                                          \
    if (auto obj = std::get<ALT>(OBJ))       \
      return CALL(obj, ARGS);                \
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

void Compiler::recordFileMapping(const std::string& generated_file, const std::string& koda_symbol, const std::string& kind)
{
  mFileMappings.push_back(FileMapping{
  generated_file,
  koda_symbol,
  kind,
  mOptions.inputFile
  });

  if (mOptions.verbose > 0)
    LOG_DEBUG("Mapped KODA:%s -> Dezyne:%s (kind:%s)", koda_symbol.c_str(), generated_file.c_str(), kind.c_str());
}

std::string Compiler::jsonEscape(const std::string& str)
{
  std::ostringstream o;
  for (char c : str)
  {
    switch (c)
    {
      case '"':  o << "\\\""; break;
      case '\\': o << "\\\\"; break;
      case '\b': o << "\\b"; break;
      case '\f': o << "\\f"; break;
      case '\n': o << "\\n"; break;
      case '\r': o << "\\r"; break;
      case '\t': o << "\\t"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20)
          o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c << std::dec;
        else
          o << c;
    }
  }
  return o.str();
}

VoidResult Compiler::writeFileMappingFile() const
{
  if (mFileMappings.empty())
  {
    LOG_DEBUG("No file mappings to write");
    return VoidResult();
  }

  std::string mappingPath = std::format("{}/trace_mapping.json", mOptions.outputDir);
  std::ofstream outfile(mappingPath, std::ios::out | std::ios::trunc);

  if (!outfile.is_open())
    return VoidResult::Failed(std::string("Could not open trace mapping file: ") + mappingPath);

  auto now = std::time(nullptr);
  auto tm = *std::gmtime(&now);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

  outfile << "{\n";
  outfile << "  \"generated_at\": \"" << oss.str() << "\",\n";
  outfile << "  \"source_koda\": \"" << jsonEscape(mOptions.inputFile) << "\",\n";
  outfile << "  \"output_directory\": \"" << jsonEscape(mOptions.outputDir) << "\",\n";
  outfile << "  \"mappings\": [\n";

  for (size_t i = 0; i < mFileMappings.size(); ++i)
  {
    const auto& m = mFileMappings[i];
    outfile << "    {\n";
    outfile << "      \"koda_symbol\": \"" << jsonEscape(m.koda_symbol) << "\",\n";
    outfile << "      \"kind\": \"" << jsonEscape(m.kind) << "\",\n";
    outfile << "      \"generated_file\": \"" << jsonEscape(m.generated_file) << "\"\n";
    outfile << "    }";

    if (i + 1 < mFileMappings.size())
      outfile << ",";
    outfile << "\n";
  }

  outfile << "  ]\n";
  outfile << "}\n";
  outfile.close();

  LOG_INFO("Trace mapping written to %s (%zu entries)", mappingPath.c_str(), mFileMappings.size());
  return VoidResult();
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

VoidResult Compiler::generate()
{
  mFileMappings.clear();
  Environment env;
  for (auto& component : mAST.components)
  {
    if (component->kind == Component::Kind::Capability)
      RETURN_ON_FAILURE(generateCapability(component, env));
  }

  for (auto& component : mAST.components)
  {
    if (component->kind == Component::Kind::Task)
      RETURN_ON_FAILURE(generateTask(component, env));
  }

  RETURN_ON_FAILURE(writeFileMappingFile());

  return VoidResult();
}

void Compiler::printAST() const
{
  mAST.print();
}

// =========================================================================================
// Generation methods

Result<koda::ReturnValue> Compiler::generateTask(PComponent task, Environment& env)
{
  for (const auto& arg : task->args)
  {
    auto name = arg->a;
    auto type = arg->b;
    if (env.capabilities.contains(type))
      env.capabilityMap[name] = type;
  }

  for (auto& statement : task->statements)
    RETURN_ON_FAILURE(generateStatement(statement, env));

  for (const auto& f : env.flows)
  {
    const auto flow = f.second;
    const auto flowName = flow.name;
    for (const auto& c : flow.asyncCalls)
    {
      auto cap = env.getCapability(c.first);
      std::string name = "";
      std::string trigger = "";
      if (!cap)
      {
        // Sometimes strategies are parsed as async calls...
        if (!env.flows.contains(c.first))
          return Result<koda::ReturnValue>::Failed("Could not find async capability: " + c.first);

        auto tmp = env.flows.at(c.first);
        name = toFlowVariable(tmp.name);
        trigger = "api";
      }
      else
      {
        name = cap->name;
        trigger = cap->trigger->name;
      }

      PortRef in = {toFlowVariable(flowName), c.first};
      PortRef out = {toFilename(name), trigger};

      if (mOptions.verbose > 1)
        LOG_RAW("asyncCalls - In: {} Out: {}", in, out);

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

      PortRef in = {toFlowVariable(flowName), instance};
      PortRef out = {toFilename(cap->name), port};

      if (mOptions.verbose > 1)
        LOG_RAW("syncCalls - In: {} Out: {}", in, out);
      env.system.connections.push_back(Connection{in, out, Connection::Type::Action});
    }

    for (const auto& c : flow.signalCalls)
    {
      auto [instance, port] = portFromString(c.first);
      auto cap = env.getCapability(instance);
      if (!cap)
        return Result<koda::ReturnValue>::Failed("Could not find signal capability: " + c.first);

      PortRef in = {toFlowVariable(flowName), port};
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
  recordFileMapping(filename, task->name, "task");

  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateCapability(PComponent capability, Environment& env)
{
  // Here, we must create the external components that will be implemented in C++, e.g.:
  // First, we go through the AST to build the strategy environment
  if (mOptions.verbose > 0)
    LOG_RAW("Compiling capability: {}", capability->name);

  env.currentCapability = Capability{};
  env.currentCapability.name = capability->name;

  for (const auto& statement : capability->statements)
    RETURN_ON_FAILURE(generateStatement(statement, env));

  env.capabilities[capability->name] = env.currentCapability;
  env.system.instances.insert({componentName(capability->name), toFilename(capability->name)});

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
    file << createPort(*cap.trigger, true);
  if (cap.abort)
    file << createPort(*cap.abort, true);
  for (const auto& in : cap.ins)
    file << createPort(in, true);
  for (const auto& out : cap.outs)
    file << createPort(out, false);

  file << "}";
  file.close();
  recordFileMapping(filename, capability->name, "capability");

  return koda::ReturnValue();
}

Result<ReturnValue> Compiler::generateArgument(PArgument argument, Environment& env)
{
  return koda::ReturnValue{argument->a, argument->b};
}

Result<ReturnValue> Compiler::generateStatement(PStatement statement, Environment& env)
{
  IF_ALT(PStrategyBlock, statement->node, generateStrategyBlock, env)
  ELSE_IF_ALT(PActionDef, statement->node, generateActionDef, env)
  ELSE_IF_ALT(PRosDef, statement->node, generateRosDef, env)
  ELSE_IF_ALT(PVarsBlock, statement->node, generateVarsBlock, env)

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateStrategyBlock(PStrategyBlock strategy, Environment& env)
{
  for (auto& flow : strategy->flows)
  {
    RETURN_ON_FAILURE(generateFlow(flow, env));
  }

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateActionDef(PActionDef action, Environment& env)
{
  env.currentCapability.title = action->label1;
  env.currentCapability.message = action->label2;

  for (const auto& def : action->rosDefs)
    RETURN_ON_FAILURE(generateRosDef(def, env));

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateRosDef(PRosDef ros, Environment& env)
{
  ReturnValue result;
  ASSIGN_OR_RETURN_ON_FAILURE(result, generateEventDef(ros->def, env));

  Action action;
  action.name = result.call;
  action.args = result.args;

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

Result<ReturnValue> Compiler::generateEventDef(PEventDef event, Environment& env)
{
  koda::ReturnValue value;
  value.call = event->name;
  value.name = event->typeName;
  for (const auto& arg : event->args)
  {
    koda::ReturnValue argValue;
    ASSIGN_OR_RETURN_ON_FAILURE(argValue, generateArgument(arg, env));
    value.args[argValue.call] = argValue.name;
  }

  return value;
}

Result<ReturnValue> Compiler::generateVarsBlock(PVarsBlock varsBlock, Environment& env)
{
  for (const auto& var : varsBlock->vars)
    RETURN_ON_FAILURE(generateVarsDef(var, env));

  return koda::ReturnValue{};
}

Result<ReturnValue> Compiler::generateVarsDef(PVarDef var, Environment& env)
{
  return koda::ReturnValue{};
}

void Compiler::connectWithArbiter(const std::map<std::string, uint32_t>& connections, Environment& env)
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

    // Change current links to the arbitrer
    for (auto& statement : env.core)
    {
      auto index0 = statement.find(instance);
      if (index0 != std::string::npos)
        statement.replace(index0, instance.size(), std::format("arbitrer{}.client{}", id, count++));
    }

    // Connect arbitrer to the component
    env.core.push_back(std::format("arbitrer{}.resource <=> {}", id, instance));
  }
}

Result<koda::ReturnValue> Compiler::generateFlow(PFlow flow, Environment& env)
{
  if (mOptions.verbose > 0)
    LOG_RAW("Generating flow: {}", flow->name);

  // There is one file per flow, so here we create a new file
  // The name of the file matches the flow + c.
  // For example, loop = cloop
  std::string filename = std::format("{}/{}.dzn", mOptions.outputDir, flow->name);
  mCurrentFile.open(filename);
  if (!mCurrentFile.is_open())
    return Result<koda::ReturnValue>::Failed("Failed to open: " + filename);

  // Compile the different connections
  env.clear();
  auto ret = generateStrategy(flow->strategy, env);

  env.flows[flow->name] = Flow{flow->name, env.syncCalls, env.asyncCalls, env.signalCalls, env.strategies};
  env.system.instances.insert({flowName(flow->name), toFlowVariable(toFilename(flow->name))});
  if (mOptions.verbose > 0)
    env.print();

  // ------------------------------------------------------------
  // Check the need for arbitrers and create them
  connectWithArbiter(env.strategies, env);
  connectWithArbiter(env.asyncCalls, env);
  connectWithArbiter(env.syncCalls, env);
  connectWithArbiter(env.signalCalls, env);

  // ------------------------------------------------------------
  // Print everything to a file
  env.core.push_front(std::format("api <=> {}", ret.Value().name));
  env.includes.insert("iaction.dzn");

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
  recordFileMapping(filename, flow->name, "flow");

  return ret;
}

Result<koda::ReturnValue> Compiler::generateStrategy(PStrategy strategy, Environment& env)
{
  IF_ALT(PSeq, strategy->v, generateSequence, env)
  ELSE_IF_ALT(PJoin, strategy->v, generateJoin, env)
  ELSE_IF_ALT(PEither, strategy->v, generateEither, env)
  ELSE_IF_ALT(PLet, strategy->v, generateLet, env)
  ELSE_IF_ALT(PWithin, strategy->v, generateWithin, env)
  ELSE_IF_ALT(PIfElse, strategy->v, generateIfElse, env)
  ELSE_IF_ALT(PRepeat, strategy->v, generateRepeat, env)
  ELSE_IF_ALT(PGuard, strategy->v, generateGuard, env)
  ELSE_IF_ALT(PEvery, strategy->v, generateEvery, env)
  ELSE_IF_ALT(PEnd, strategy->v, generateEnd, env)
  ELSE_IF_ALT(PContinue, strategy->v, generateContinue, env)
  ELSE_IF_ALT(PRef, strategy->v, generateRef, env)
  ELSE_IF_ALT(PTaskCall, strategy->v, generateTaskCall, env)
  ELSE_IF_ALT(PParen, strategy->v, generateParen, env)

  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateSequence(PSeq strategy, Environment& env)
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
    return generateStrategy(strategy->alts.at(0), env);

  auto id = env.sequence++;
  auto created = createSequenceComponent(instances);
  if (!created.IsSuccess())
    return Result<koda::ReturnValue>::Failed(created.ErrorMessage());

  env.includes.insert(std::format("sequence{}.dzn", instances));
  env.definitions.push_back(std::format("csequence{} s{}", instances, id));

  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], env));
    env.core.push_back(std::format("s{}.action{} <=> {}", id, i, expr.name));
  }

  return koda::ReturnValue{std::format("s{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateJoin(PJoin strategy, Environment& env)
{
  auto id = env.join++;
  env.includes.insert("parallel.dzn");
  env.definitions.push_back(std::format("cparallel p{}", id));

  for (uint32_t i = 0; i < strategy->alts.size(); ++i)
  {
    ReturnValue expr;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->alts[i], env));
    env.core.push_back(std::format("p{}.action{} <=> {}", id, i, expr.name));
  }

  return koda::ReturnValue{Format("p%d.api", id)};
}

Result<koda::ReturnValue> Compiler::generateEither(PEither strategy, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateLet(PLet strategy, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateWithin(PWithin strategy, Environment& env)
{
  ReturnValue exprDo;
  ASSIGN_OR_RETURN_ON_FAILURE(exprDo, generateStrategy(strategy->a, env));

  ReturnValue exprElse;
  ASSIGN_OR_RETURN_ON_FAILURE(exprElse, generateStrategy(strategy->b, env));

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

Result<koda::ReturnValue> Compiler::generateIfElse(PIfElse strategy, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateRepeat(PRepeat strategy, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->a, env));

  auto id = env.repeat++;
  env.includes.insert("repeat.dzn");
  env.definitions.push_back(std::format("crepeat r{}", id));

  env.core.push_back(std::format("r{}.action1 <=> {}", id, expr.name));

  return koda::ReturnValue{std::format("r{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateGuard(PGuard strategy, Environment& env)
{
  return koda::ReturnValue();
}

Result<koda::ReturnValue> Compiler::generateEvery(PEvery strategy, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategy(strategy->a, env));

  auto id = env.every++;
  auto alarmId = env.alarm++;

  env.includes.insert("every.dzn");
  env.includes.insert("ialarm.dzn");
  env.definitions.push_back(std::format("cevery e{}", id));

  env.system.instances.insert({"calarm", std::format("alarm{}", alarmId)});

  INCREMENT_MAP(env.strategies, std::format("alarm{}", alarmId));
  env.requiresPorts.insert(std::format("ialarm alarm{}", alarmId));

  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
      continue;

    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, env));
  }

  for (auto& handler : strategy->handlers)
  {
    if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
      continue;

    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, env));
  }

  env.core.push_back(std::format("e{}.action1 <=> {}", id, expr.name));
  env.core.push_back(std::format("e{}.alarm <=> alarm{}", id, alarmId));

  return koda::ReturnValue{std::format("e{}.api", id)};
}

Result<koda::ReturnValue> Compiler::generateEnd(PEnd strategy, Environment& env)
{
  return koda::ReturnValue{"end"};
}

Result<ReturnValue> Compiler::generateContinue(PContinue strategy, Environment& env)
{
  return koda::ReturnValue{"continue"};
}

Result<koda::ReturnValue> Compiler::generateRef(PRef strategy, Environment& env)
{
  INCREMENT_MAP(env.strategies, strategy->name)
  env.requiresPorts.insert(std::format("iaction {}", strategy->name));

  return koda::ReturnValue{strategy->name};
}

Result<koda::ReturnValue> Compiler::generateTaskCall(PTaskCall strategy, Environment& env)
{
  ReturnValue expr;
  ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(strategy->call, env, false));

  if (strategy->handlers.empty())
    return expr;

  for (auto& handler : strategy->handlers)
  {
    if (handler->kind != koda::StrategyHandler::Kind::OnEmitter)
      continue;

    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, env));
  }

  for (auto& handler : strategy->handlers)
  {
    if (handler->kind == koda::StrategyHandler::Kind::OnEmitter)
      continue;

    env.previousCall = expr.name;
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateStrategyHandler(handler, env));
  }

  return expr;
}

Result<koda::ReturnValue> Compiler::generateParen(PParen strategy, Environment& env)
{
  return generateStrategy(strategy->a, env);
}

Result<ReturnValue> Compiler::generateStrategyHandler(PStrategyHandler handler, Environment& env)
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
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, env, false));
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, env));

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
      ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, env, false));
    }

    if (handler->body)
    {
      ReturnValue strat;
      ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, env));

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
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, env, true));

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.call);

    ReturnValue strat;
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, env));

    env.core.push_back(std::format("sh{}.signal <=> {}", id, expr.call));
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
    ASSIGN_OR_RETURN_ON_FAILURE(expr, generateEventCall(handler->emitter, env, true));

    env.includes.insert("isignal.dzn");
    env.requiresPorts.insert("isignal " + expr.call);

    ReturnValue strat;
    ASSIGN_OR_RETURN_ON_FAILURE(strat, generateStrategy(handler->body, env));

    env.core.push_back(std::format("sh{}.signal <=> {}", id, expr.call));
    env.core.push_back(std::format("sh{}.action <=> {}", id, env.previousCall));
    env.core.push_back(std::format("sh{}.handler <=> {}", id, strat.name));

    return koda::ReturnValue{std::format("sh{}.api", id)};
  }

  return koda::ReturnValue{};
}

Result<koda::ReturnValue> Compiler::generateEventCall(PEventCall call, Environment& env, bool isSignal)
{
  if (call->receiver.empty())
  {
    INCREMENT_MAP(env.asyncCalls, call->name)
    env.requiresPorts.insert(std::format("iaction {}", call->name));
    return koda::ReturnValue{call->name};
  }
  else
  {
    auto event = std::format("{}.{}", call->receiver, call->name);
    if (isSignal)
    {
      INCREMENT_MAP(env.signalCalls, event)
    }
    else
    {
      INCREMENT_MAP(env.syncCalls, event)
      env.requiresPorts.insert(std::format("iaction {}", call->receiver));
    }

    return koda::ReturnValue{call->receiver, call->name};
  }
}

VoidResult Compiler::createSequenceComponent(uint32_t instances)
{
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
  recordFileMapping(filename, "sequence_temp", "sequenceComponent");

  return VoidResult();
}

VoidResult Compiler::createActionArbiterComponent(uint32_t instances)
{
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
  // std::string args;
  // for (auto it = action.args.cbegin(); it != action.args.cend(); ++it)
  //   args += std::format("{} {}{}", it->second, it->first, (std::next(it) != action.args.end() ? ", " : ""));

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

    // auto inId = std::format("{}.{}", in.instance, in.port);
    // if (refs.contains(inId))
    //   refs[inId].push_back(out);
    // else
    //   refs[inId] = {out};

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

      // Do we need this anywhere?
      // if (conn.lhs.instance == instance && conn.lhs.port == port)
      // {
      //   conn.lhs.instance = arbiter;
      //   conn.lhs.port = "resource";
      // }
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
