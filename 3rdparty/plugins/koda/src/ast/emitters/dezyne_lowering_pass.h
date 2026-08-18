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
    // Ordinal within the current flow. Used for the flow-local port name.
    std::uint32_t localOrdinal = 0;

    // Ordinal across all uses of the target event. Used for wiring to the
    // numbered capability port.
    std::uint32_t targetOrdinal = 0;

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
    std::map<koda::SymbolId, std::uint32_t> eventOrdinals;
  };

  struct FlowResult
  {
    std::vector<CallUse> calls;
    std::vector<std::string> alarms;
  };

  Model& mModel;
  const SymbolRegistry& mSymbols;
  const koda::CompilerOptions& mOptions;
  // Total number of usages of each callable action event.
  // Key = event symbol, e.g. Drive::to_position or Siren::start.
  std::map<koda::SymbolId, std::uint32_t> mCallCounts;

  // Current ordinal while lowering calls.
  // Also keyed by the event symbol.
  std::map<koda::SymbolId, std::uint32_t> mCallOrdinals;

  // Events that require iaction ports and therefore support multiplicity.
  std::set<koda::SymbolId> mActionEvents;

  std::map<koda::SymbolId, FlowResult> mFlows;

  VoidResult lowerCapability(const ir::Component& capability);
  VoidResult lowerTask(const ir::Component& task);
  Result<FlowResult> lowerFlow(const ir::Flow& flow);
  Result<std::string> lowerStrategy(const ir::Flow& flow, const ir::PStrategy& strategy, FlowState& state);
  Result<std::string> lowerHandler(const ir::Flow& flow, const ir::PHandler& handler, FlowState& state);
  Result<std::string> lowerCall(const ir::Call& call, FlowState& state, bool signal);

  VoidResult createNecessaryArbiter(SymbolId componentId, std::vector<Connection>& connections);

  void countTriggers(const ir::PStrategy& strategy);
  void countHandlerTriggers(const ir::PHandler& handler);

  std::string sourceName(koda::SymbolId id) const;
  static std::string lower(std::string value);
  static std::string componentName(const std::string& name);
  static std::string flowName(const std::string& name);

  std::vector<LoweringPass::CallUse> uniqueRequiredPorts(const FlowState& state) const;
  PortRef portFromString(const std::string& ref) const;
};

}  // namespace koda::dezyne