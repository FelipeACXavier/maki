#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "dezyne_model.h"
#include "result.h"

namespace koda
{
struct CompilerOptions;
}

namespace koda::dezyne
{
class LoweringPass
{
public:
  LoweringPass(Model& model, const SymbolRegistry& symbols, const koda::CompilerOptions& options);
  VoidResult run(const ir::Program& program);

private:
  struct CallUse
  {
    enum class Kind
    {
      Trigger,
      Action,
      Signal,
      Flow
    } kind = Kind::Action;

    std::string localPort;
    koda::SymbolId receiver = koda::InvalidSymbol;
    koda::SymbolId target = koda::InvalidSymbol;
    std::uint32_t ordinal = 0;
    Span span;
  };

  struct PortRef
  {
    std::string instance;
    std::string port;
  };

  struct Connection
  {
    std::string lhs;
    std::string rhs;
    Span span;
  };

  struct FlowState
  {
    SymbolId component = InvalidSymbol;
    std::uint32_t sequence = 0, join = 0, repeat = 0, within = 0, every = 0;
    std::uint32_t abortHandler = 0, errorHandler = 0, signalHandler = 0, alarm = 0;
    std::string previous;
    std::set<std::string> imports;
    std::vector<std::string> definitions;
    std::vector<Connection> connections;
    std::vector<CallUse> calls;
    std::vector<std::string> alarms;
    std::map<koda::SymbolId, std::uint32_t> triggerOrdinals;
  };

  struct FlowResult
  {
    std::vector<CallUse> calls;
    std::vector<std::string> alarms;
  };

  Model& mModel;
  const SymbolRegistry& mSymbols;
  const koda::CompilerOptions& mOptions;
  std::map<koda::SymbolId, std::uint32_t> mTriggerCount;
  std::map<koda::SymbolId, std::uint32_t> mTriggerCounts;
  std::map<koda::SymbolId, std::string> mTriggerNames;
  std::map<koda::SymbolId, FlowResult> mFlows;

  VoidResult lowerCapability(const ir::Component& capability);
  VoidResult lowerTask(const ir::Component& task);
  Result<FlowResult> lowerFlow(const ir::Flow& flow);
  Result<std::string> lowerStrategy(const ir::Flow& flow, const ir::PStrategy& strategy, FlowState& state);
  Result<std::string> lowerHandler(const ir::Flow& flow, const ir::PHandler& handler, FlowState& state);
  Result<std::string> lowerCall(const ir::Call& call, FlowState& state, bool signal);

  VoidResult ensureAlarmHelper();
  VoidResult ensureSequenceHelper(const ir::Flow& component, std::uint32_t count);
  VoidResult ensureArbiterHelper(const Component& component, std::uint32_t count);
  VoidResult createNecessaryArbiter(const Component& component, std::vector<Connection>& connections);
  void countTriggers(const ir::PStrategy& strategy);
  void countHandlerTriggers(const ir::PHandler& handler);

  std::string sourceName(koda::SymbolId id) const;
  std::string triggerName(koda::SymbolId capability) const;
  static std::string lower(std::string value);
  static std::string componentName(const std::string& name);
  static std::string flowName(const std::string& name);

  std::vector<LoweringPass::CallUse> uniqueRequiredPorts(const FlowState& state) const;
  PortRef portFromString(const std::string& ref) const;
};

}  // namespace koda::dezyne
