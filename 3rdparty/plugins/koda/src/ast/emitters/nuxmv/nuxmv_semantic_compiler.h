#pragma once

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "nuxmv_model.h"
#include "result.h"

namespace koda
{
struct CompilerOptions;
}

namespace koda::nuxmv
{
class SemanticCompiler
{
public:
  SemanticCompiler(const ir::Program& program, const SymbolRegistry& symbols, const CompilerOptions& options);

  // Leaf-stage entry point. At this stage a flow is accepted when its root is
  // Failure or a Call that lowers to Action/AbortCall semantics.
  VoidResult compileFlow(const ir::Component& component, const ir::Flow& flow, Model& model);

  // Public for focused unit tests and for the composite stage that follows.
  VoidResult compileStrategy(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                             Behaviour& behaviour);

  VoidResult compileProperties(const ir::Component& component, Model& model);

private:
  enum class LeafCallKind
  {
    Action,
    AbortCall,
    Flow,
    FlowAbort
  };

  const ir::Program& mProgram;
  const SymbolRegistry& mSymbols;
  const CompilerOptions& mOptions;

  std::set<SymbolId> mExternallyAbortedFlows;
  std::map<SymbolId, ir::EventKind> mEventKinds;
  std::set<std::string> mUsedIds;
  std::uint32_t mAnonymousId = 0;

  std::vector<std::string> mIdContext;
  std::unordered_map<std::string, std::size_t> mIdCounters;

  struct CapabilityCallSite
  {
    std::string id;
    SymbolId target = InvalidSymbol;

    PExpression trigger;
    PExpression abort;
    PExpression reset;

    PExpression triggerReply;
    PExpression abortReply;
    PExpression outcome;

    Provenance origin;
  };

  struct CapabilityInstance
  {
    SymbolId symbol;
    std::string id;
    ir::CapabilityKind kind = ir::CapabilityKind::Unknown;
    std::vector<CapabilityCallSite> calls;
  };

  struct FlowCallSite
  {
    std::string id;

    PExpression trigger;
    PExpression triggerReply;
    PExpression outcome;
    PExpression abort;
    PExpression abortReply;

    PExpression running;
  };

  struct FlowInstance
  {
    SymbolId symbol = InvalidSymbol;
    std::string id;

    std::vector<FlowCallSite> calls;

    std::vector<PExpression> abortRequests;
    std::vector<PExpression> abortReplies;
  };

  std::unordered_map<SymbolId, CapabilityInstance> mCapabilities;
  std::unordered_map<SymbolId, FlowInstance> mFlowInstances;

  void materializeCapabilities(Behaviour& behaviour);
  void materializeSyncCapability(const CapabilityInstance& capability, Behaviour& behaviour);
  void materializeAsyncCapability(const CapabilityInstance& capability, Behaviour& behaviour);
  void materializeFlows(Behaviour& behaviour);
  void materializeBehaviourInterface(Behaviour& behaviour, const std::string& suffix = "");
  void mergeBehaviour(koda::nuxmv::Behaviour& target, koda::nuxmv::Behaviour&& child);
  void registerMainFlow(const Behaviour& behaviour, SymbolId flow, const PExpression command);

  Result<CapabilityInstance*> findCapability(SymbolId symbol);
  FlowInstance& findFlow(SymbolId symbol);

  VoidResult compileChoose(const ir::PStrategy& strategy, const ir::Strategy::Choose& choose, const PExpression& trigger, const PExpression& abort,
                           const PExpression& reset, Behaviour& behaviour);
  VoidResult compileEnd(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset, Behaviour& behaviour);
  VoidResult compileContinue(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                             Behaviour& behaviour);
  VoidResult compileFlowCall(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                             const PExpression& reset, Behaviour& behaviour);
  VoidResult compileFlowAbort(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                              const PExpression& reset, Behaviour& behaviour);

  VoidResult applyStrategyHandlers(const std::string& ownerId, const std::vector<ir::PHandler>& handlers, Behaviour& behaviour);

  void indexEvents();
  void indexFlowAborts(const ir::PStrategy& strategy);

  bool isFlowAbort(const ir::Call& call) const;
  LeafCallKind classify(const ir::Call& call) const;

  VoidResult compileFailure(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                            Behaviour& behaviour);
  VoidResult compileCall(const ir::PStrategy& strategy, const ir::Strategy::Call& call, const PExpression& trigger, const PExpression& abort,
                         const PExpression& reset, Behaviour& behaviour);
  VoidResult compileAction(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                           Behaviour& behaviour);
  VoidResult compileAbortCall(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                              const PExpression& reset, Behaviour& behaviour);
  VoidResult compileSequence(const ir::PStrategy& strategy, const ir::Strategy::Sequence& sequence, const PExpression& trigger, const PExpression& abort,
                             const PExpression& reset, Behaviour& behaviour);
  VoidResult compileRepeat(const ir::PStrategy& strategy, const ir::Strategy::Repeat& repeat, const PExpression& trigger, const PExpression& abort,
                           const PExpression& reset, Behaviour& behaviour);
  VoidResult compileJoin(const ir::PStrategy& strategy, const ir::Strategy::Join& join, const PExpression& trigger, const PExpression& abort,
                         const PExpression& reset, Behaviour& behaviour);
  VoidResult compileEither(const ir::PStrategy& strategy, const ir::Strategy::Either& either, const PExpression& trigger, const PExpression& abort,
                           const PExpression& reset, Behaviour& behaviour);
  VoidResult compileWithin(const ir::PStrategy& strategy, const ir::Strategy::Within& within, const PExpression& trigger, const PExpression& abort,
                           const PExpression& reset, Behaviour& behaviour);

  PExpression flowRisingExpression(const FlowCallSite& call) const;
  PExpression flowFallingExpression(const FlowCallSite& call) const;
  PExpression capabilityRunningExpression(const CapabilityInstance& capability, const CapabilityCallSite* call = nullptr) const;
  PExpression capabilityRisingExpression(const CapabilityInstance& capability, const CapabilityCallSite* call) const;
  PExpression capabilityFallingExpression(const CapabilityInstance& capability, const CapabilityCallSite* call) const;
  PExpression runningExpression(const Behaviour& behaviour) const;
  Result<PExpression> compilePropertyExpr(const ir::PPropertyExpr& expression);
  Result<PExpression> compileObservation(const ir::Observation& observation);
  std::vector<const CapabilityCallSite*> findCallSites(SymbolId receiver, SymbolId target) const;

  struct UniqueId
  {
    std::string id;
    std::string localName;
  };

  UniqueId uniqueId(const ir::PStrategy& strategy, const std::string& fallback);
  std::string sourceName(SymbolId id) const;
  std::string strategyName(const ir::PStrategy& strategy, const std::string& fallback) const;
  static std::string sanitize(std::string value);
  static Provenance provenance(const ir::PStrategy& strategy);
};

}  // namespace koda::nuxmv
