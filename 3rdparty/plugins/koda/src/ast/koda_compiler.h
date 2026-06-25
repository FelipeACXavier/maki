#pragma once

#include <deque>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "ast.h"
#include "koda_plugin.h"
#include "logging.h"
#include "result.h"
#include "MirrorNode.h"

namespace koda
{
struct ReturnValue
{
  std::string name;
  std::string call;
  std::map<std::string, std::string> args;
  std::string srcId;
};

struct CompilerOptions
{
  std::string inputFile;
  std::string outputDir = "./out";

  int verbose = 0;
  bool showHelp = false;
  bool dryRun = false;
  bool showVersion = false;
  enum class PluginOption
  {
    NoPlugins = 0,
    PluginsOnly,
    RunAll
  } pluginRule = PluginOption::NoPlugins;
};

class Compiler
{
public:
  struct Composition
  {
    std::string id = "";
    std::map<std::string, Composition> children;
  };

  struct Action
  {
    std::string name;
    std::map<std::string, std::string> args;
    std::string srcId;

    void print() const
    {
      LOG_RAW("  Action: {} {}", name, args.empty() ? "" : "Args:");
      for (auto it = args.cbegin(); it != args.cend(); ++it)
        LOG_RAW("    {}: {}", it->first, it->second);
    }
  };

  struct Capability
  {
    std::string name;
    std::string title;
    std::string message;
    std::optional<Action> trigger;
    std::optional<Action> abort;
    std::optional<Action> error;
    std::optional<Action> success;
    std::vector<Action> ins;
    std::vector<Action> outs;
    std::string srcId;

    void print() const
    {
      LOG_RAW("Capability: {} {}", name, message);
      if (trigger) { LOG_RAW("Trigger:"); trigger->print(); }
      if (abort)   { LOG_RAW("Abort:");   abort->print(); }
      if (success) { LOG_RAW("Return:");  success->print(); }
      if (error)   { LOG_RAW("Error:");   error->print(); }
      if (!ins.empty()) {
        LOG_RAW("Ins:");
        for (const auto& in : ins) in.print();
      }
      if (!outs.empty()) {
        LOG_RAW("Outs:");
        for (const auto& out : outs) out.print();
      }
    }
  };

  // Flow now stores call identifiers together with their source ids
  struct Flow
  {
    std::string name;
    std::vector<std::pair<std::string, std::string>> asyncCalls;   // {identifier, srcId}
    std::vector<std::pair<std::string, std::string>> syncCalls;
    std::vector<std::pair<std::string, std::string>> signalCalls;
    std::vector<std::pair<std::string, std::string>> strategies;   // {name, srcId}
  };

  struct Instance
  {
    std::string type;
    std::string name;
    std::string srcId;

    bool operator<(const Instance& other) const
    {
      if (type != other.type) return type < other.type;
      return name < other.name;
    }
  };

  struct PortRef
  {
    std::string instance;
    std::string port;
  };

  struct Connection
  {
    enum class Type { Unknown, Action, Signal };
    PortRef lhs;
    PortRef rhs;
    Type type;
    std::string srcId;
  };

  struct TopLevelSystem
  {
    std::set<Instance> instances;
    std::vector<Connection> connections;
  };

  struct CapabilityCall
  {
    uint32_t count;
    std::vector<std::string> args;
  };

  // Helper for storing generated lines with optional source id
  struct LineWithSrc
  {
    std::string text;
    std::string srcId;
  };

  struct Environment
  {
    uint32_t sequence = 0;
    uint32_t join = 0;
    uint32_t repeat = 0;
    uint32_t within = 0;
    uint32_t every = 0;
    uint32_t alarm = 0;

    uint32_t arbiter = 0;
    uint32_t abortHandler = 0;
    uint32_t errorHandler = 0;
    uint32_t signalHandler = 0;

    std::string previousCall = "";

    // Counts of calls (for generating multiple ports)
    std::map<std::string, uint32_t> asyncCallsCounter;

    // Counts used by the arbiter connection logic
    std::map<std::string, uint32_t> syncCallsCountMap;
    std::map<std::string, uint32_t> signalCallsCountMap;
    std::map<std::string, uint32_t> strategiesCountMap;

    // Temporary storage of calls with their source ids (populated during strategy gen)
    std::vector<std::pair<std::string, std::string>> asyncCallsWithSrc;
    std::vector<std::pair<std::string, std::string>> syncCallsWithSrc;
    std::vector<std::pair<std::string, std::string>> signalCallsWithSrc;
    std::vector<std::pair<std::string, std::string>> strategiesWithSrc;

    std::set<std::string> includes = {};
    std::set<std::string> requiresPorts = {};

    // Lines that will be emitted to the flow/task component
    std::deque<LineWithSrc> definitions;
    std::deque<LineWithSrc> core;

    Capability currentCapability;
    std::map<std::string, Flow> flows;
    std::map<std::string, Capability> capabilities;
    std::map<std::string, std::string> capabilityMap;
    std::map<std::string, std::vector<CapabilityCall>> capabilityCalls;

    TopLevelSystem system;

    std::optional<Capability> getCapability(const std::string& key) const
    {
      if (!capabilityMap.contains(key)) return std::nullopt;
      if (!capabilities.contains(capabilityMap.at(key))) return std::nullopt;
      return capabilities.at(capabilityMap.at(key));
    }

    void clear()
    {
      sequence = join = repeat = within = every = 0;
      arbiter = abortHandler = errorHandler = signalHandler = 0;
      previousCall = "";
      syncCallsCountMap.clear();
      signalCallsCountMap.clear();
      strategiesCountMap.clear();
      asyncCallsWithSrc.clear();
      syncCallsWithSrc.clear();
      signalCallsWithSrc.clear();
      strategiesWithSrc.clear();
      includes.clear();
      requiresPorts.clear();
      definitions.clear();
      core.clear();
    }

    void print() const
    {
      LOG_RAW("---------------------------------------");
      LOG_RAW("Environment");
      LOG_RAW("Sequence count: {}", sequence);
      LOG_RAW("Join count: {}", join);
      LOG_RAW("Abort handler count: {}", abortHandler);
      LOG_RAW("Error handler count: {}", errorHandler);
      LOG_RAW("Signal handler count: {}", signalHandler);
      LOG_RAW("Includes count: {}", includes.size());
      LOG_RAW("Requires ports count: {}", requiresPorts.size());
      LOG_RAW("Definitions count: {}", definitions.size());
      LOG_RAW("Core count: {}", core.size());

      LOG_RAW("Capabilities:");
      for (auto it = capabilities.cbegin(); it != capabilities.cend(); ++it)
        LOG_RAW("  {}: {}", it->first, it->second.name);

      if (!asyncCallsCounter.empty()) {
        LOG_RAW("Async Calls Counter:");
        for (auto it = asyncCallsCounter.cbegin(); it != asyncCallsCounter.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second);
      }

      if (!syncCallsCountMap.empty()) {
        LOG_RAW("Sync Calls Count:");
        for (auto it = syncCallsCountMap.cbegin(); it != syncCallsCountMap.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second);
      }

      if (!signalCallsCountMap.empty()) {
        LOG_RAW("Signal Calls Count:");
        for (auto it = signalCallsCountMap.cbegin(); it != signalCallsCountMap.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second);
      }

      if (!strategiesCountMap.empty()) {
        LOG_RAW("Strategies Count:");
        for (auto it = strategiesCountMap.cbegin(); it != strategiesCountMap.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second);
      }

      if (!flows.empty()) {
        LOG_RAW("Flows:");
        for (auto it = flows.cbegin(); it != flows.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second.name);
      }

      if (!capabilityMap.empty()) {
        LOG_RAW("CapMap:");
        for (auto it = capabilityMap.cbegin(); it != capabilityMap.cend(); ++it)
          LOG_RAW("  {}: {}", it->first, it->second);
      }

      if (!capabilityCalls.empty()) {
        LOG_RAW("CapCalls:");
        for (auto it = capabilityCalls.cbegin(); it != capabilityCalls.cend(); ++it)
          for (const auto& call : it->second)
            LOG_RAW("  {}: {}", it->first, call.count);
      }

      LOG_RAW("---------------------------------------");
    }
  };

  Compiler();

  VoidResult parse(const CompilerOptions& options);
  VoidResult generate();
  System getAST() const;
  Environment getIR() const;

  void printAST() const;

  VoidResult addPlugin(std::shared_ptr<KodaPlugin> plugin);
  std::vector<std::string> generatedFiles() const;

private:
  System mAST;
  Environment mEnv;
  std::ofstream mCurrentFile;
  CompilerOptions mOptions;
  std::map<std::string, std::shared_ptr<KodaPlugin>> mPlugins;
  std::vector<std::string> mGeneratedFiles;

  // Line mapping infrastructure
  std::map<std::string, std::map<int, std::string>> mLineMappings;   // file -> (line -> srcId)
  std::map<std::string, int> mLineCounters;                           // file -> current line number

  void startFile(const std::string& filepath);
  void emitLine(std::ofstream& stream, const std::string& filepath,
                const std::string& line, const std::string& srcId);
  void writeLineMapping();

  MirrorNode mMirrorRoot;
  MirrorWalker mWalker;
  std::map<std::string, std::string> mAstMap;
  bool skipStrategy = false;

  void mapToSrcId(const std::string& dezyneName, const std::string& srcId);

  Result<ReturnValue> generateTask(PComponent task, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateCapability(PComponent capability, const MirrorNode& node, Environment& env);
  Result<ReturnValue> emitCapability(PComponent capability, Environment& env);

  Result<ReturnValue> generateStrategy(PStrategyBlock strategy);
  Result<ReturnValue> generateFlow(PFlow flow, const MirrorNode& node, Environment& env);

  Result<ReturnValue> generateStrategy(PStrategy strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateSequence(PSeq strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateJoin(PJoin strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateEither(PEither strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateLet(PLet strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateWithin(PWithin strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateIfElse(PIfElse strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateRepeat(PRepeat strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateGuard(PGuard strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateEvery(PRepeat strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateEnd(PEnd strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateContinue(PContinue strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateRef(PRef strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateTaskCall(PTaskCall strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateParen(PParen strategy, const MirrorNode& node, Environment& env);

  Result<ReturnValue> generateStrategyHandler(PStrategyHandler handler, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateEventCall(PEventCall call, const MirrorNode& node, Environment& env, bool isSignal);

  Result<ReturnValue> generateStrategyBlock(PStrategyBlock strategy, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateActionDef(PActionDef action, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateRosDef(PRosDef rosDef, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateEventDef(PEventDef event, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateVarsBlock(PVarsBlock varsBlock, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateVarsDef(PVarDef varDef, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateArgument(PArgument argument, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateStatement(PStatement statement, const MirrorNode& node, Environment& env);

  Result<ReturnValue> generateExpr(PExpr expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateId(PId expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateStr(PStr expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateInt(PInt expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateFloat(PFloat expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateCall(PCall expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateNeg(PNeg expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateNot(PNot expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateBinOp(PBinOp expr, const MirrorNode& node, Environment& env);
  Result<ReturnValue> generateParen(PEParen expr, const MirrorNode& node, Environment& env);

  std::string componentName(const std::string& name) const;
  std::string flowName(const std::string& name) const;
  std::string toFilename(const std::string& name) const;
  std::string createPort(const Action& action, bool in) const;

  VoidResult createSequenceComponent(uint32_t instances);
  VoidResult createActionArbiterComponent(uint32_t instances);
  void createSequenceDoneRecursion(bool fromIdle, uint32_t start, uint32_t instances,
                                   std::ofstream& file, const std::string& indent);

  void connectWithArbiter(Environment& env);
  void connectWithArbiter(const std::map<std::string, uint32_t>& connections,
                          Connection::Type connectionType, Environment& env);

  PortRef portFromString(const std::string& ref) const;
  VoidResult runPlugins();
};

}  // namespace koda

// Formatter for PortRef (unchanged)
template <>
struct std::formatter<koda::Compiler::PortRef>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const koda::Compiler::PortRef& i, FormatContext& ctx) const
  {
    return std::format_to(ctx.out(), "{} {}", i.instance, i.port);
  }
};