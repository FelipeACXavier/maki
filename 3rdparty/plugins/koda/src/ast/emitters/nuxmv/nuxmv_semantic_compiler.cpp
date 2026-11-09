#include "nuxmv_semantic_compiler.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iterator>

#include "ast/koda_compiler.h"
#include "logging.h"
#include "nuxmv_composite_semantics.h"
#include "nuxmv_constants.h"
#include "nuxmv_handler_semantics.h"
#include "nuxmv_semantic_contracts.h"

namespace koda::nuxmv
{

class IdContextGuard
{
public:
  enum class Mode
  {
    Push,
    Reset
  };

  IdContextGuard(std::vector<std::string>& context, std::string name, Mode mode = Mode::Push)
      : mContext(context)
      , mPrevious(context)
  {
    if (mode == Mode::Reset)
      mContext.clear();

    mContext.push_back(std::move(name));
  }

  ~IdContextGuard()
  {
    mContext = std::move(mPrevious);
  }

  IdContextGuard(const IdContextGuard&) = delete;
  IdContextGuard& operator=(const IdContextGuard&) = delete;

private:
  std::vector<std::string>& mContext;
  std::vector<std::string> mPrevious;
};

namespace
{
// The IR represents an unguarded `when:` option by an absent guard. Keep
// detection local: other generators still control the concrete data-language
// lowering, while Choose only needs to distinguish a guard from `otherwise`.
// A missing/unrecognised IR field is a compile error, never silently treated
// as an unconditional final branch.
template <class T>
inline constexpr bool alwaysFalseChoose = false;

template <class T>
bool chooseHasValue(const T& value)
{
  if constexpr (requires { static_cast<bool>(value); })
    return static_cast<bool>(value);
  else if constexpr (requires { value.has_value(); })
    return value.has_value();
  else
    static_assert(alwaysFalseChoose<T>, "Choose guard must be optional or pointer-like");
}

template <class Option>
bool isChooseOtherwise(const Option& option)
{
  if constexpr (requires { option.isOtherwise(); })
    return chooseHasValue(option.isOtherwise());
  else if constexpr (requires { option.isOtherwise; })
    return chooseHasValue(option.isOtherwise);
  else if constexpr (requires { option.isDefault(); })
    return chooseHasValue(option.isDefault());
  else if constexpr (requires { option.isDefault; })
    return chooseHasValue(option.isDefault);
  else if constexpr (requires { option.isDefaultBranch; })
    return chooseHasValue(option.isDefaultBranch);
  else if constexpr (requires { option.condition; })
    return !chooseHasValue(option.condition);
  else if constexpr (requires { option.when; })
    return !chooseHasValue(option.when);
  else if constexpr (requires { option.guard; })
    return !chooseHasValue(option.guard);
  else if constexpr (requires { option.predicate; })
    return !chooseHasValue(option.predicate);
  else
    static_assert(alwaysFalseChoose<Option>, "Cannot identify default Choose option; add the IR's optional guard field to isChooseOtherwise");
}

PExpression ref(const std::string& id, const std::string& suffix)
{
  return reference(id + "_" + suffix);
}

PExpression is(const PExpression& lhs, const PExpression& rhs)
{
  return equal(lhs, rhs);
}

PExpression and3(const PExpression& a, const PExpression& b, const PExpression& c)
{
  return logicalAnd(logicalAnd(a, b), c);
}

// Preserve pairwise mutual exclusion without emitting O(n^2) TRANS formulas.
// For signals s[0..n-1], each step requires !(s[i] & (s[0] | ... | s[i-1])).
// The prefix is named once, so the emitted expression DAG grows linearly.
// Cannot have an argument called "signals" because it is a reserved QT word
void constrainAtMostOne(Behaviour& behaviour, const std::string& prefix, const std::vector<PExpression>& ssignals, const Provenance& origin)
{
  if (ssignals.size() < 2)
    return;

  if (ssignals.size() == 2)
  {
    behaviour.constraints.push_back({logicalNot(logicalAnd(ssignals[0], ssignals[1])), origin});
    return;
  }

  PExpression previous = ssignals.front();
  for (std::size_t i = 1; i < ssignals.size(); ++i)
  {
    const auto name = prefix + "_prior_" + std::to_string(i);
    behaviour.definitions.push_back({name, previous, origin});
    behaviour.constraints.push_back({logicalNot(logicalAnd(reference(name), ssignals[i])), origin});
    previous = logicalOr(reference(name), ssignals[i]);
  }
}

}  // namespace

SemanticCompiler::SemanticCompiler(const ir::Program& program, const SymbolRegistry& symbols, const CompilerOptions& options)
    : mProgram(program)
    , mSymbols(symbols)
    , mOptions(options)
{
  indexEvents();
}

VoidResult SemanticCompiler::compileFlow(const ir::Component& component, const ir::Flow& flow, Model& model)
{
  model.clear();
  mUsedIds.clear();
  mAnonymousId = 0;
  mIdCounters.clear();
  mCapabilities.clear();
  mFlowInstances.clear();

  const auto modelName = sanitize(component.name + "_" + flow.name);
  model.setName(modelName);
  model.setOutputPath((std::filesystem::path(mOptions.outputDir) / (modelName + ".smv")).string());

  model.addVariable({
    .kind = VariableKind::Input,
    .name = "command",
    .domain = "0..3",
    .initial = {},
    .origin = {.sourceSymbol = flow.symbol, .sourceSpan = flow.span},
  });

  const auto command = reference("command");
  Behaviour root;
  IdContextGuard context(mIdContext, sanitize(flow.name), IdContextGuard::Mode::Reset);

  RETURN_ON_FAILURE(compileStrategy(flow.strategy, is(command, constants::commandTrigger()), is(command, constants::commandAbort()),
                                    is(command, constants::commandReset()), root));

  materializeBehaviourInterface(root);
  RETURN_ON_FAILURE(registerMainFlow(root, flow.symbol, command));

  // Capability calls are collected while recursively compiling the strategy.
  // Materialize one shared state machine per capability only after all call
  // sites are known.
  materializeCapabilities(root);
  materializeFlows(root);

  model.addBehaviour(root);
  model.addDefinition({"reply",
                       cases(
                         {
                           {root.trigger, root.triggerReply},
                           {root.abort, root.abortReply},
                           {root.reset, root.resetReply},
                         },
                         constants::resultNone()),
                       root.origin});
  model.addDefinition({"outcome", root.outcome, root.origin});

  return VoidResult();
}

VoidResult SemanticCompiler::registerMainFlow(const Behaviour& behaviour, SymbolId flow, const PExpression command)
{
  auto flowResult = findFlow(flow);
  RETURN_ON_FAILURE(flowResult);

  auto& rootFlow = *flowResult.Value();
  rootFlow.calls.push_back(FlowCallSite{
    .id = behaviour.id,
    .trigger = equal(command, constants::commandTrigger()),
    .triggerReply = behaviour.triggerReply,
    .outcome = behaviour.outcome,
    .abort = equal(command, constants::commandAbort()),
    .abortReply = behaviour.abortReply,
    .running = runningExpression(behaviour),
  });

  return VoidResult();
}

VoidResult SemanticCompiler::compileStrategy(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                                             Behaviour& behaviour)
{
  if (!strategy)
    return VoidResult::Failed("nuXmv semantic compiler: invalid strategy");

  if (std::holds_alternative<ir::Strategy::Failure>(strategy->value))
    return compileFailure(strategy, trigger, abort, reset, behaviour);
  else if (const auto* call = std::get_if<ir::Strategy::Call>(&strategy->value))
    return compileCall(strategy, *call, trigger, abort, reset, behaviour);
  else if (const auto* sequence = std::get_if<ir::Strategy::Sequence>(&strategy->value))
    return compileSequence(strategy, *sequence, trigger, abort, reset, behaviour);
  else if (const auto* repeat = std::get_if<ir::Strategy::Repeat>(&strategy->value))
    return compileRepeat(strategy, *repeat, trigger, abort, reset, behaviour);
  else if (const auto* retry = std::get_if<ir::Strategy::Retry>(&strategy->value))
    return compileRetry(strategy, *retry, trigger, abort, reset, behaviour);
  else if (const auto* join = std::get_if<ir::Strategy::Join>(&strategy->value))
    return compileJoin(strategy, *join, trigger, abort, reset, behaviour);
  else if (const auto* either = std::get_if<ir::Strategy::Either>(&strategy->value))
    return compileEither(strategy, *either, trigger, abort, reset, behaviour);
  else if (const auto* within = std::get_if<ir::Strategy::Within>(&strategy->value))
    return compileWithin(strategy, *within, trigger, abort, reset, behaviour);
  else if (std::holds_alternative<ir::Strategy::End>(strategy->value))
    return compileEnd(strategy, trigger, abort, reset, behaviour);
  else if (std::holds_alternative<ir::Strategy::Continue>(strategy->value))
    return compileContinue(strategy, trigger, abort, reset, behaviour);
  else if (const auto* choose = std::get_if<ir::Strategy::Choose>(&strategy->value))
    return compileChoose(strategy, *choose, trigger, abort, reset, behaviour);

  return VoidResult::Failed("nuXmv semantic compiler does not yet support strategy '{}'", strategy->id);
}

VoidResult SemanticCompiler::compileFailure(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                                            Behaviour& behaviour)
{
  const auto [id, _] = uniqueId(strategy, "failure");
  const auto state = ref(id, "state");
  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));

  behaviour.variables.push_back({
    .kind = VariableKind::State,
    .name = id + "_state",
    .domain = "{Idle, Error}",
    .initial = reference("Idle"),
    .origin = behaviour.origin,
  });

  behaviour.triggerReply = cases({{logicalAnd(is(state, reference("Idle")), trigger), constants::resultFailure()}}, constants::resultNone());
  behaviour.abortReply = cases({{is(state, reference("Error")), constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{is(state, reference("Error")), constants::resultSuccess()}}, constants::resultNone());
  behaviour.outcome = constants::eventNone();
  behaviour.mayReturnDone = false;  // Failure can only reject its trigger.
  behaviour.mayAccept = false;
  behaviour.mayReject = true;
  behaviour.mayEmitSuccess = false;
  behaviour.mayEmitFailure = false;
  behaviour.mayEmitAborted = false;

  behaviour.nextAssignments.push_back({
    .variable = id + "_state",
    .cases =
      {
        {logicalAnd(is(state, reference("Idle")), trigger), reference("Error")},
        {logicalAnd(is(state, reference("Error")), reset), reference("Idle")},
      },
    .fallback = state,
    .origin = behaviour.origin,
  });

  const auto noCommand = logicalNot(logicalOr(trigger, logicalOr(abort, reset)));
  behaviour.constraints.push_back({
    logicalAnd(binary(is(state, reference("Idle")), "->", logicalOr(noCommand, trigger)),
               binary(is(state, reference("Error")), "->", logicalOr(noCommand, logicalOr(reset, abort)))),
    behaviour.origin,
  });

  contracts::always(behaviour, "failure_rejects_immediately",
                    contracts::implies(logicalAnd(is(state, reference("Idle")), trigger), equal(behaviour.triggerReply, constants::resultFailure())));
  contracts::nextState(behaviour, "trigger_rejected_enters_error", logicalAnd(is(state, reference("Idle")), trigger), is(state, reference("Error")));
  contracts::nextState(behaviour, "reset_clears_error", logicalAnd(is(state, reference("Error")), reset), is(state, reference("Idle")));

  return {};
}

VoidResult SemanticCompiler::compileCall(const ir::PStrategy& strategy, const ir::Strategy::Call& call, const PExpression& trigger, const PExpression& abort,
                                         const PExpression& reset, Behaviour& behaviour)
{
  switch (classify(call.call))
  {
    case LeafCallKind::Action:
      RETURN_ON_FAILURE(compileAction(strategy, call.call, trigger, abort, reset, behaviour));
      break;

    case LeafCallKind::AbortCall:
      RETURN_ON_FAILURE(compileAbortCall(strategy, call.call, trigger, abort, reset, behaviour));
      break;

    case LeafCallKind::Flow:
      RETURN_ON_FAILURE(compileFlowCall(strategy, call.call, trigger, abort, reset, behaviour));
      break;

    case LeafCallKind::FlowAbort:
      RETURN_ON_FAILURE(compileFlowAbort(strategy, call.call, trigger, abort, reset, behaviour));
      break;
  }

  if (!call.handlers.empty())
    materializeBehaviourInterface(behaviour, "_core");

  return applyStrategyHandlers(behaviour.id, call.handlers, behaviour);
}

VoidResult SemanticCompiler::compileAction(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                                           const PExpression& reset, Behaviour& behaviour)
{
  // A Strategy::Call is an invocation site, not a capability instance.  All
  // invocations targeting the same capability symbol share one capability
  // state machine, materialized after the complete flow has been compiled.
  const auto [id, _] = uniqueId(strategy, "action");
  auto capabilityResult = findCapability(call.receiver);
  RETURN_ON_FAILURE(capabilityResult);

  auto& capability = *capabilityResult.Value();

  const auto capState = reference(capability.id + "_state");
  const auto triggerResult = reference(capability.id + "_trigger_result");
  const auto resetResult = reference(capability.id + "_reset_result");

  const auto idle = equal(capState, reference("Idle"));
  const auto error = equal(capState, reference("Error"));
  // A completed asynchronous invocation releases the capability before the
  // next invocation is synchronously dispatched in the *same* KODA macrostep.
  // Both calls are represented by one nuXmv transition, hence state is still
  // Running until next(state) is applied.
  // The shared capability emits this once; all invocation sites reference it.
  const auto available = capability.kind == ir::CapabilityKind::Async ? reference(capability.id + "_available") : idle;

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = error;

  // Shared by synchronous and asynchronous capabilities.
  behaviour.triggerReply = cases(
    {
      {logicalAnd(available, trigger), triggerResult},
    },
    constants::resultNone());

  behaviour.resetReply = cases(
    {
      {logicalAnd(error, reset), resetResult},
    },
    constants::resultNone());

  if (capability.kind == ir::CapabilityKind::Sync)
  {
    behaviour.mayReturnDone = true;
    behaviour.mayAccept = false;
    behaviour.mayReject = true;
    behaviour.mayEmitSuccess = false;
    behaviour.mayEmitFailure = false;
    behaviour.mayEmitAborted = false;
    // Synchronous operations complete during the trigger macrostep.
    // They do not enter Running or emit asynchronous outcomes.
    behaviour.abortReply = cases(
      {
        {logicalAnd(error, abort), constants::resultFailure()},
      },
      constants::resultNone());

    behaviour.outcome = constants::eventNone();
  }
  else if (capability.kind == ir::CapabilityKind::Async)
  {
    behaviour.mayReturnDone = false;
    behaviour.mayAccept = true;
    behaviour.mayReject = true;
    behaviour.mayEmitSuccess = true;
    behaviour.mayEmitFailure = true;
    behaviour.mayEmitAborted = true;
    const auto capOwner = reference(capability.id + "_owner");
    const auto abortResult = reference(capability.id + "_abort_result");
    const auto event = reference(capability.id + "_event");

    const auto owns = equal(capOwner, reference(id));
    const auto running = equal(capState, reference("Running"));
    // A Join may need to cancel an action which was just ACCEPTED while a
    // sibling rejected its trigger. The capability has not entered the stored
    // Running state yet, but the cancellation is part of this macrostep.
    const auto acceptedNow = allOf({available, trigger, equal(triggerResult, constants::resultSuccess())});

    behaviour.abortReply = cases(
      {
        {logicalAnd(abort, logicalOr(logicalAnd(running, owns), acceptedNow)), abortResult},
        {logicalAnd(error, abort), constants::resultFailure()},
      },
      constants::resultNone());

    behaviour.outcome = cases(
      {
        {logicalAnd(running, owns), event},
      },
      constants::eventNone());
  }
  else
  {
    return VoidResult::Failed("nuXmv: unsupported capability kind for '{}'", capability.id);
  }

  capability.calls.push_back(CapabilityCallSite{
    .id = id,
    .target = call.target,

    .trigger = trigger,
    .abort = abort,
    .reset = reset,

    .triggerReply = behaviour.triggerReply,
    .abortReply = behaviour.abortReply,
    .outcome = behaviour.outcome,

    .origin = behaviour.origin,
  });

  return {};
}

VoidResult SemanticCompiler::compileAbortCall(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                                              const PExpression& reset, Behaviour& behaviour)
{
  const auto* abortEvent = mSymbols.get(call.target);

  if (!abortEvent || abortEvent->actionId == InvalidSymbol)
    return VoidResult::Failed("nuXmv: abort event has no associated action");

  const auto* triggerEvent = mSymbols.eventOfAction(abortEvent->owner, OSymbolId{abortEvent->actionId}, "Trigger");
  if (!triggerEvent)
    return VoidResult::Failed("nuXmv: abort event has no corresponding trigger");

  const auto [id, _] = uniqueId(strategy, "abort_call");
  const auto state = ref(id, "state");
  const auto idle = is(state, reference("Idle"));
  const auto error = is(state, reference("Error"));

  CapabilityInstance* capability = nullptr;
  ASSIGN_OR_RETURN_ON_FAILURE(capability, findCapability(call.receiver));

  const auto abortResult = reference(capability->id + "_abort_result");
  const auto targetActive = reference(id + "_target_active");
  const auto invoked = allOf({idle, trigger});

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));

  behaviour.variables = {
    {VariableKind::State, id + "_state", "{Idle, Error}", reference("Idle"), behaviour.origin},
  };

  behaviour.triggerReply = cases(
    {
      // Idempotent cancellation or successful active cancellation.  Factoring
      // these branches keeps equivalent RESULT_DONE semantics while avoiding
      // one copy of the target-active guard in each synchronous abort chain.
      {allOf({invoked, anyOf({logicalNot(targetActive), equal(abortResult, constants::resultSuccess())})}),
       constants::resultDone()},

      // Actual cancellation failed.
      {allOf({invoked, targetActive, equal(abortResult, constants::resultFailure())}), constants::resultFailure()},
    },
    constants::resultNone());

  behaviour.abortReply = cases({{logicalAnd(error, abort), constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{logicalAnd(error, reset), constants::resultSuccess()}}, constants::resultNone());
  behaviour.outcome = constants::eventNone();
  behaviour.mayReturnDone = true;  // Idempotent / successful explicit abort.
  behaviour.mayAccept = false;
  behaviour.mayReject = true;
  behaviour.mayEmitSuccess = false;
  behaviour.mayEmitFailure = false;
  behaviour.mayEmitAborted = false;

  behaviour.nextAssignments.push_back({
    .variable = id + "_state",
    .cases =
      {
        {allOf({invoked, targetActive, is(abortResult, constants::resultFailure())}), reference("Error")},
        {logicalAnd(is(state, reference("Error")), reset), reference("Idle")},
      },
    .fallback = state,
    .origin = behaviour.origin,
  });

  const auto noCommand = logicalNot(logicalOr(trigger, logicalOr(abort, reset)));
  behaviour.constraints.push_back({
    allOf({
      binary(idle, "->", logicalOr(noCommand, trigger)),
      binary(error, "->", logicalOr(noCommand, logicalOr(reset, abort))),
    }),
    behaviour.origin,
  });

  capability->abortCalls.push_back(CapabilityAbortSite{
    .id = id,
    .targetTrigger = triggerEvent->id,
    .invoked = invoked,
    .origin = behaviour.origin,
  });

  return VoidResult();
}

// Materialize the observable interface of a compiled behaviour as named DEFINEs.
//
// Composite semantics refer to child replies/outcomes many times. Keeping those
// values as arbitrary expression trees causes recursive textual duplication when
// behaviours are nested. Naming the interface once preserves expression sharing
// while keeping the generated nuXmv transition system completely flat.
void SemanticCompiler::materializeBehaviourInterface(Behaviour& behaviour, const std::string& suffix)
{
  if (behaviour.id.empty())
    return;

  // The same Behaviour can be materialized at several nested flow-call
  // boundaries (e.g. once before its call-site handlers and again afterwards).
  // Preserve each version under a distinct DEFINE name: overwriting or
  // skipping the earlier definition would change what the handler observes.
  std::string prefix = behaviour.id + suffix;
  for (std::size_t serial = 1; std::ranges::any_of(behaviour.definitions,
                                                   [&prefix](const Definition& definition) {
                                                     return definition.name == prefix + "_trigger_reply" || definition.name == prefix + "_abort_reply" ||
                                                            definition.name == prefix + "_reset_reply" || definition.name == prefix + "_outcome";
                                                   });
       ++serial)
  {
    prefix = behaviour.id + suffix + "_" + std::to_string(serial);
  }

  const auto triggerReplyName = prefix + "_trigger_reply";
  const auto abortReplyName = prefix + "_abort_reply";
  const auto resetReplyName = prefix + "_reset_reply";
  const auto outcomeName = prefix + "_outcome";

  behaviour.definitions.push_back({triggerReplyName, behaviour.triggerReply, behaviour.origin});
  behaviour.definitions.push_back({abortReplyName, behaviour.abortReply, behaviour.origin});
  behaviour.definitions.push_back({resetReplyName, behaviour.resetReply, behaviour.origin});
  behaviour.definitions.push_back({outcomeName, behaviour.outcome, behaviour.origin});

  behaviour.triggerReply = koda::nuxmv::reference(triggerReplyName);
  behaviour.abortReply = koda::nuxmv::reference(abortReplyName);
  behaviour.resetReply = koda::nuxmv::reference(resetReplyName);
  behaviour.outcome = koda::nuxmv::reference(outcomeName);
}

void SemanticCompiler::mergeBehaviour(koda::nuxmv::Behaviour& target, koda::nuxmv::Behaviour&& child)
{
  target.variables.insert(target.variables.end(), std::make_move_iterator(child.variables.begin()), std::make_move_iterator(child.variables.end()));
  target.definitions.insert(target.definitions.end(), std::make_move_iterator(child.definitions.begin()), std::make_move_iterator(child.definitions.end()));
  target.nextAssignments.insert(target.nextAssignments.end(), std::make_move_iterator(child.nextAssignments.begin()),
                                std::make_move_iterator(child.nextAssignments.end()));
  target.constraints.insert(target.constraints.end(), std::make_move_iterator(child.constraints.begin()), std::make_move_iterator(child.constraints.end()));
  target.expectations.insert(target.expectations.end(), std::make_move_iterator(child.expectations.begin()), std::make_move_iterator(child.expectations.end()));
}

VoidResult SemanticCompiler::compileSequence(const ir::PStrategy& strategy, const ir::Strategy::Sequence& sequence, const PExpression& trigger,
                                             const PExpression& abort, const PExpression& reset, Behaviour& behaviour)
{
  if (sequence.items.empty())
    return VoidResult::Failed("nuXmv Sequence requires at least one child");

  const auto [id, localName] = uniqueId(strategy, "sequence");
  IdContextGuard context(mIdContext, localName);

  const auto stateName = id + "_state";
  const auto state = reference(stateName);

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));

  // Compile children before declaring the control state: each child advertises
  // the possible outcomes of its trigger, so synchronous-only children do NOT
  // require a persistent Child<i> state.
  std::vector<Behaviour> children(sequence.items.size());
  for (std::size_t i = 0; i < sequence.items.size(); ++i)
  {
    const auto prefix = id + "_child" + std::to_string(i);
    RETURN_ON_FAILURE(
      compileStrategy(sequence.items[i], reference(prefix + "_trigger"), reference(prefix + "_abort"), reference(prefix + "_reset"), children[i]));
    materializeBehaviourInterface(children[i]);
  }

  // Default modes for constructs that are not yet annotated are conservative
  // (all true), so pruning is safe: false can only come from a compiler that
  // has ruled out asynchronous acceptance.
  std::vector<bool> persistent(children.size());
  std::string domain = "{Idle, Error";
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    persistent[i] = children[i].mayAccept;
    if (persistent[i])
      domain += ", Child" + std::to_string(i);
  }
  domain += "}";

  behaviour.variables.push_back({
    .kind = VariableKind::State,
    .name = stateName,
    .domain = domain,
    .initial = reference("Idle"),
    .origin = behaviour.origin,
  });

  // The complete sequence can return DONE only if every child can do so
  // in its trigger macrostep. This does not constrain asynchronous completion.
  behaviour.mayReturnDone = std::all_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReturnDone; });
  behaviour.mayAccept = std::any_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayAccept; });
  behaviour.mayReject = std::any_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReject; });
  // Conservative upper bounds for the composite's asynchronous outcomes.
  // These will become more precise when the other composite compilers are
  // annotated (and when we represent which children are reachable).
  behaviour.mayEmitSuccess = behaviour.mayAccept;
  behaviour.mayEmitFailure = behaviour.mayAccept;
  behaviour.mayEmitAborted = behaviour.mayAccept;

  // Build synchronous trigger closure backwards:
  //
  //   child[i] Done -> immediately evaluate child[i+1].triggerReply
  //
  // The expression therefore represents the entire synchronous call chain,
  // rather than spreading it over nuXmv transitions.
  std::vector<PExpression> suffixReply(sequence.items.size());
  suffixReply.back() = children.back().triggerReply;

  for (std::size_t n = sequence.items.size() - 1; n-- > 0;)
  {
    // Do not create an unreachable dependency on the remaining suffix.
    if (!children[n].mayReturnDone)
      suffixReply[n] = children[n].triggerReply;
    else
      suffixReply[n] = cases({{equal(children[n].triggerReply, constants::resultDone()), suffixReply[n + 1]}}, children[n].triggerReply);
  }

  behaviour.triggerReply = cases({{equal(state, reference("Idle")), suffixReply.front()}}, constants::resultNone());

  // Sequence abort is synchronous at the KODA semantic level. The active
  // child's abort reply is propagated directly; no Dezyne-only Aborting state
  // is introduced here.
  std::vector<Expression::Case::Branch> abortCases;
  for (std::size_t i = 0; i < children.size(); ++i)
    if (persistent[i])
      abortCases.push_back({equal(state, reference("Child" + std::to_string(i))), children[i].abortReply});
  abortCases.push_back({equal(state, reference("Error")), constants::resultError()});
  behaviour.abortReply = cases(std::move(abortCases), constants::resultNone());

  // Parent asynchronous outcome is the active child's outcome, except that a
  // child's success may synchronously start subsequent children. We compute
  // that closure below for both outcome and next-state selection.
  std::vector<PExpression> continueReply(children.size(), constants::resultNone());
  for (std::size_t i = 0; i < children.size(); ++i)
    if (i + 1 < children.size())
      continueReply[i] = suffixReply[i + 1];
    else
      continueReply[i] = constants::resultDone();

  std::vector<Expression::Case::Branch> outcomeCases;
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    if (!persistent[i])
      continue;
    const auto active = equal(state, reference("Child" + std::to_string(i)));
    const auto childSuccess = equal(children[i].outcome, constants::eventSuccess());
    const auto childFailure = equal(children[i].outcome, constants::eventFailure());
    const auto childAborted = equal(children[i].outcome, constants::eventAborted());

    if (children[i].mayEmitFailure)
      outcomeCases.push_back({logicalAnd(active, childFailure), constants::eventFailure()});
    if (children[i].mayEmitAborted)
      outcomeCases.push_back({logicalAnd(active, childAborted), constants::eventAborted()});

    if (!children[i].mayEmitSuccess)
      continue;

    if (i + 1 == children.size())
    {
      outcomeCases.push_back({
        logicalAnd(active, childSuccess),
        constants::eventSuccess(),
      });
    }
    else
    {
      outcomeCases.push_back({
        allOf({
          active,
          childSuccess,
          equal(continueReply[i], constants::resultFailure()),
        }),
        constants::eventFailure(),
      });
      // The remaining sequence cannot finish in the same macrostep if
      // any child in the continuation suffix cannot return RESULT_DONE.
      bool continuationMayFinish = true;
      for (std::size_t j = i + 1; j < children.size(); ++j)
        if (!children[j].mayReturnDone)
        {
          continuationMayFinish = false;
          break;
        }
      if (continuationMayFinish)
        outcomeCases.push_back({
          allOf({active, childSuccess, equal(continueReply[i], constants::resultDone())}),
          constants::eventSuccess(),
        });
    }
  }
  behaviour.outcome = cases(std::move(outcomeCases), constants::eventNone());

  // Child invocation DEFINEs.  Use a recurrence rather than enumerating every
  // earlier persistent predecessor and every intermediate DONE-only suffix.
  //
  // child[0] triggers when the Sequence is started.
  // child[i] triggers when child[i-1] synchronously returns DONE, or when
  // child[i-1] was the active asynchronous child and reports SUCCESS.
  //
  // Inductively, the first term also covers every earlier asynchronous
  // predecessor followed by a chain of synchronous DONE replies.  This
  // preserves same-macrostep dispatch while keeping the expression DAG linear
  // in the number of children rather than repeatedly constructing prefixes.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto prefix = id + "_child" + std::to_string(i);
    PExpression childTrigger;
    if (i == 0)
    {
      childTrigger = logicalAnd(equal(state, reference("Idle")), trigger);
    }
    else
    {
      std::vector<PExpression> waysToTrigger;
      const auto& predecessor = children[i - 1];
      const auto precedingTrigger = reference(id + "_child" + std::to_string(i - 1) + "_trigger");

      if (predecessor.mayReturnDone)
        waysToTrigger.push_back(logicalAnd(precedingTrigger, equal(predecessor.triggerReply, constants::resultDone())));

      if (persistent[i - 1] && predecessor.mayEmitSuccess)
        waysToTrigger.push_back(allOf({equal(state, reference("Child" + std::to_string(i - 1))),
                                       equal(predecessor.outcome, constants::eventSuccess())}));

      childTrigger = anyOf(waysToTrigger);
    }

    // Optional experiment: materialize downstream sequence trigger predicates
    // as constrained Boolean inputs rather than recursively expanded DEFINEs.
    // Compare both BMC and IC3 behavior before enabling this by default.
#ifndef KODA_NUXMV_FACTOR_SEQUENCE_TRIGGERS
    const auto triggerEncoding = i > 0 ? DefinitionEncoding::ConstrainedBooleanInput : DefinitionEncoding::Macro;
#else
    // const auto triggerEncoding = DefinitionEncoding::Macro;
#endif
    behaviour.definitions.push_back({prefix + "_trigger", childTrigger, behaviour.origin, triggerEncoding});
    behaviour.definitions.push_back({
      prefix + "_abort",
      persistent[i] ? logicalAnd(equal(state, reference("Child" + std::to_string(i))), abort) : boolean(false),
      behaviour.origin,
    });
    behaviour.definitions.push_back({
      prefix + "_reset",
      allOf({equal(state, reference("Error")), reset, children[i].error}),
      behaviour.origin,
    });
  }

  std::vector<PExpression> resetSucceeded;
  std::vector<PExpression> resetFailed;
  for (const auto& child : children)
  {
    resetSucceeded.push_back(logicalOr(logicalNot(child.error), equal(child.resetReply, constants::resultSuccess())));
    resetFailed.push_back(logicalAnd(child.error, equal(child.resetReply, constants::resultFailure())));
  }

  const auto allResetSucceeded = allOf(resetSucceeded);
  const auto anyResetFailed = anyOf(resetFailed);

  // Reset is only meaningful after an error. The current flat reference
  // semantics treat recovery of the composite as a successful reset of its
  // control state. Child-specific reset bookkeeping will be refined when
  // RecoveryHandler is added.
  behaviour.resetReply = cases(
    {
      {allOf({equal(state, reference("Error")), reset, anyResetFailed}), constants::resultFailure()},
      {allOf({equal(state, reference("Error")), reset, allResetSucceeded}), constants::resultSuccess()},
    },
    constants::resultNone());

  // Experimental sequence next-state factoring. The child trigger recurrence
  // already represents both initial synchronous DONE prefixes and asynchronous
  // SUCCESS handovers. Reuse those predicates rather than re-enumerating every
  // predecessor/suffix combination when selecting the persistent sequence state.
  // This option only changes how next-state guards are expressed; it does not
  // add new variables. Validate against the original transition relation.
#ifndef KODA_NUXMV_FACTOR_SEQUENCE_TRANSITIONS
  std::vector<GuardedValue> nextCases;

  // Accepting or rejecting a child produces the same next-state disposition
  // whether the child was reached from Idle or from a predecessor's SUCCESS.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto childTriggered = reference(id + "_child" + std::to_string(i) + "_trigger");
    if (persistent[i])
      nextCases.push_back({logicalAnd(childTriggered, equal(children[i].triggerReply, constants::resultSuccess())),
                           reference("Child" + std::to_string(i))});
    if (children[i].mayReject)
      nextCases.push_back({logicalAnd(childTriggered, equal(children[i].triggerReply, constants::resultFailure())),
                           reference("Error")});
  }

  // A persistent child can complete without selecting a new persistent child:
  // on FAILURE/ABORTED, or when all remaining synchronous children return DONE.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    if (!persistent[i])
      continue;

    const auto active = equal(state, reference("Child" + std::to_string(i)));
    const auto childSuccess = equal(children[i].outcome, constants::eventSuccess());
    if (children[i].mayEmitFailure)
      nextCases.push_back({logicalAnd(active, equal(children[i].outcome, constants::eventFailure())), reference("Error")});
    if (children[i].mayEmitAborted)
      nextCases.push_back({logicalAnd(active, equal(children[i].outcome, constants::eventAborted())), reference("Idle")});
    if (!children[i].mayEmitSuccess)
      continue;

    if (i + 1 == children.size())
    {
      nextCases.push_back({logicalAnd(active, childSuccess), reference("Idle")});
      continue;
    }

    const bool suffixMayFinish = std::all_of(children.begin() + i + 1, children.end(),
                                            [](const Behaviour& child) { return child.mayReturnDone; });
    if (suffixMayFinish)
      nextCases.push_back({allOf({active, childSuccess, equal(suffixReply[i + 1], constants::resultDone())}),
                           reference("Idle")});
  }
#else
  // Persistent sequence-state transitions.
  std::vector<GuardedValue> nextCases;

  // Initial trigger: determine the first child that remains Running, or finish
  // synchronously if all children return Done.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    std::vector<PExpression> prefix{
      equal(state, reference("Idle")),
      trigger,
    };
    for (std::size_t j = 0; j < i; ++j)
      prefix.push_back(equal(children[j].triggerReply, constants::resultDone()));

    if (persistent[i])
    {
      auto running = prefix;
      running.push_back(equal(children[i].triggerReply, constants::resultSuccess()));
      nextCases.push_back({allOf(running), reference("Child" + std::to_string(i))});
    }

    if (children[i].mayReject)
    {
      auto failed = prefix;
      failed.push_back(equal(children[i].triggerReply, constants::resultFailure()));
      nextCases.push_back({allOf(failed), reference("Error")});
    }

    // No later child can be reached in the same initial trigger macrostep.
    if (!children[i].mayReturnDone)
      break;
  }

  // Async child completion followed by synchronous closure through subsequent
  // children.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    if (!persistent[i])
      continue;
    const auto active = equal(state, reference("Child" + std::to_string(i)));
    if (children[i].mayEmitFailure)
      nextCases.push_back({logicalAnd(active, equal(children[i].outcome, constants::eventFailure())), reference("Error")});
    if (children[i].mayEmitAborted)
      nextCases.push_back({logicalAnd(active, equal(children[i].outcome, constants::eventAborted())), reference("Idle")});

    if (!children[i].mayEmitSuccess)
      continue;

    if (i + 1 == children.size())
    {
      nextCases.push_back({
        logicalAnd(active, equal(children[i].outcome, constants::eventSuccess())),
        reference("Idle"),
      });
      continue;
    }

    for (std::size_t j = i + 1; j < children.size(); ++j)
    {
      std::vector<PExpression> prefix{
        active,
        equal(children[i].outcome, constants::eventSuccess()),
      };
      for (std::size_t k = i + 1; k < j; ++k)
        prefix.push_back(equal(children[k].triggerReply, constants::resultDone()));

      if (persistent[j])
      {
        auto running = prefix;
        running.push_back(equal(children[j].triggerReply, constants::resultSuccess()));
        nextCases.push_back({allOf(running), reference("Child" + std::to_string(j))});
      }

      if (children[j].mayReject)
      {
        auto failed = prefix;
        failed.push_back(equal(children[j].triggerReply, constants::resultFailure()));
        nextCases.push_back({allOf(failed), reference("Error")});
      }

      // A non-DONE child can run asynchronously, but subsequent children
      // cannot be started synchronously from this completion event.
      if (!children[j].mayReturnDone)
        break;
    }

    // The remaining suffix can finish immediately only when every child
    // in that suffix may return RESULT_DONE.
    bool suffixMayFinish = true;
    for (std::size_t j = i + 1; j < children.size(); ++j)
      if (!children[j].mayReturnDone)
      {
        suffixMayFinish = false;
        break;
      }
    if (suffixMayFinish)
    {
      std::vector<PExpression> allDone{
        active,
        equal(children[i].outcome, constants::eventSuccess()),
      };
      for (std::size_t j = i + 1; j < children.size(); ++j)
        allDone.push_back(equal(children[j].triggerReply, constants::resultDone()));
      nextCases.push_back({allOf(allDone), reference("Idle")});
    }
  }

#endif

  // Synchronous abort: successful abort returns to Idle; failure enters Error.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    if (!persistent[i])
      continue;
    const auto active = equal(state, reference("Child" + std::to_string(i)));
    nextCases.push_back({
      allOf({active, abort, equal(children[i].abortReply, constants::resultSuccess())}),
      reference("Idle"),
    });
    nextCases.push_back({
      allOf({active, abort, equal(children[i].abortReply, constants::resultFailure())}),
      reference("Error"),
    });
  }

  nextCases.push_back({
    allOf({equal(state, reference("Error")), reset, equal(behaviour.resetReply, constants::resultSuccess())}),
    reference("Idle"),
  });

  behaviour.nextAssignments.push_back({
    .variable = stateName,
    .cases = std::move(nextCases),
    .fallback = state,
    .origin = behaviour.origin,
  });

  // Merge children into this semantic fragment. Their invocation signals are
  // the DEFINEs above, so the final nuXmv model remains completely flat.
  for (auto& child : children)
  {
    behaviour.variables.insert(behaviour.variables.end(), std::make_move_iterator(child.variables.begin()), std::make_move_iterator(child.variables.end()));
    behaviour.definitions.insert(behaviour.definitions.end(), std::make_move_iterator(child.definitions.begin()),
                                 std::make_move_iterator(child.definitions.end()));
    behaviour.nextAssignments.insert(behaviour.nextAssignments.end(), std::make_move_iterator(child.nextAssignments.begin()),
                                     std::make_move_iterator(child.nextAssignments.end()));
    behaviour.constraints.insert(behaviour.constraints.end(), std::make_move_iterator(child.constraints.begin()),
                                 std::make_move_iterator(child.constraints.end()));
    behaviour.expectations.insert(behaviour.expectations.end(), std::make_move_iterator(child.expectations.begin()),
                                  std::make_move_iterator(child.expectations.end()));
  }

  const auto noCommand = logicalNot(logicalOr(trigger, logicalOr(abort, reset)));
  behaviour.constraints.push_back({
    allOf({
      binary(equal(state, reference("Idle")), "->", logicalOr(noCommand, trigger)),
      binary(equal(state, reference("Error")), "->", logicalOr(noCommand, logicalOr(reset, abort))),
    }),
    behaviour.origin,
  });

  const auto implies = [](PExpression lhs, PExpression rhs) { return binary(std::move(lhs), "->", std::move(rhs)); };
  const auto idle = equal(state, reference("Idle"));
  const auto error = equal(state, reference("Error"));

  // Every sequence invocation begins by invoking child 0.
  contracts::always(behaviour, "first_child_trigger", implies(logicalAnd(idle, trigger), reference(id + "_child0_trigger")));

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto index = std::to_string(i);
    const auto childTriggered = reference(id + "_child" + index + "_trigger");
    const auto active = persistent[i] ? equal(state, reference("Child" + index)) : boolean(false);
    const auto accepted = equal(children[i].triggerReply, constants::resultSuccess());
    const auto rejected = equal(children[i].triggerReply, constants::resultFailure());
    const auto succeeded = equal(children[i].outcome, constants::eventSuccess());
    const auto failed = equal(children[i].outcome, constants::eventFailure());

    // Startup obligations only apply when a DONE-only prefix can reach this
    // child from Idle. Otherwise "Idle & child_i_trigger" is impossible.
    bool mayStartAtIdle = true;
    for (std::size_t j = 0; j < i; ++j)
      mayStartAtIdle &= children[j].mayReturnDone;

    if (mayStartAtIdle && persistent[i])
      contracts::nextState(behaviour, "accepted_child" + index + "_active", allOf({idle, childTriggered, accepted}), active);
    if (mayStartAtIdle && children[i].mayReject)
      contracts::nextState(behaviour, "rejected_child" + index + "_error", allOf({idle, childTriggered, rejected}), error);

    // Synchronous children never occupy a persistent Child state, but may
    // still continue an initial DONE chain or an asynchronous handover.
    if (i + 1 < children.size() && children[i].mayReturnDone)
      contracts::always(behaviour, "done_child" + index + "_continues",
                        implies(allOf({childTriggered, equal(children[i].triggerReply, constants::resultDone())}),
                                reference(id + "_child" + std::to_string(i + 1) + "_trigger")));

    if (!persistent[i])
      continue;

    if (children[i].mayEmitFailure)
      contracts::nextState(behaviour, "failed_child" + index + "_error", logicalAnd(active, failed), error);

    if (i + 1 == children.size())
    {
      if (children[i].mayEmitSuccess)
        contracts::nextState(
          behaviour,
          "last_child_succeeded",
          logicalAnd(active, succeeded),
                             idle);
        continue;
    }

    const auto successorTriggered = reference(id + "_child" + std::to_string(i + 1) + "_trigger");
    if (children[i].mayEmitFailure)
      contracts::always(behaviour, "failure_child" + index + "_blocks_next", implies(logicalAnd(active, failed), logicalNot(successorTriggered)));

    if (!children[i].mayEmitSuccess)
      continue;

    // KODA Sequence: success dispatches the successor in the SAME macrostep.
    contracts::always(behaviour, "success_child" + index + "_continues", implies(logicalAnd(active, succeeded), successorTriggered));

    // Synchronous DONE descendants are skipped within the same macrostep.
    // Any later accepted asynchronous child becomes persistent, while a
    // rejected child must instead put Sequence into Error.
    for (std::size_t j = i + 1; j < children.size(); ++j)
    {
      bool prefixMayReturnDone = true;
      for (std::size_t k = i + 1; k < j; ++k)
        prefixMayReturnDone &= children[k].mayReturnDone;
      if (!prefixMayReturnDone)
        break;

      const auto successor = reference(id + "_child" + std::to_string(j) + "_trigger");
      if (persistent[j])
        contracts::nextState(behaviour, "handover_child" + index + "_to_child" + std::to_string(j) + "_active",
                             allOf({active, succeeded, successor, equal(children[j].triggerReply, constants::resultSuccess())}),
                             equal(state, reference("Child" + std::to_string(j))));
      if (children[j].mayReject)
        contracts::nextState(behaviour, "handover_child" + index + "_to_child" + std::to_string(j) + "_rejected",
                             allOf({active, succeeded, successor, equal(children[j].triggerReply, constants::resultFailure())}), error);
    }

    // If the remainder can finish synchronously, the *same* completion
    // macrostep must propagate the final SUCCESS or FAILURE and next state.
    bool suffixMayReturnDone = true;
    for (std::size_t j = i + 1; j < children.size(); ++j)
      suffixMayReturnDone &= children[j].mayReturnDone;

    if (suffixMayReturnDone)
    {
      contracts::always(behaviour, "sync_suffix_child" + index + "_done",
                        implies(allOf({active, succeeded, equal(continueReply[i], constants::resultDone())}),
                                allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
    }
    contracts::always(behaviour, "suffix_child" + index + "_rejected",
                      implies(allOf({active, succeeded, equal(continueReply[i], constants::resultFailure())}),
                              allOf({equal(behaviour.outcome, constants::eventFailure()), unary("X", error)})));
  }

  // Reset rejection must not silently turn an erroneous Sequence into Idle.
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);

  return {};
}

VoidResult SemanticCompiler::compileRepeat(const ir::PStrategy& strategy, const ir::Strategy::Repeat& repeat, const PExpression& trigger,
                                           const PExpression& abort, const PExpression& reset, Behaviour& behaviour)
{
  if (!repeat.body)
    return VoidResult::Failed("nuXmv Repeat requires a body");

  const auto [id, localName] = uniqueId(strategy, "repeat");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  const auto childTriggerName = id + "_body_trigger";
  const auto childAbortName = id + "_body_abort";
  const auto childResetName = id + "_body_reset";

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));

  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Retrigger, Error}", reference("Idle"), behaviour.origin});

  Behaviour child;
  RETURN_ON_FAILURE(compileStrategy(repeat.body, reference(childTriggerName), reference(childAbortName), reference(childResetName), child));

  materializeBehaviourInterface(child);
  // A Repeat invocation is accepted irrespective of the outcome of its
  // first body attempt.  That attempt is *scheduled*, not invoked in the
  // parent's trigger macrostep.  The first attempted body trigger may still
  // reject; Repeat then reports an asynchronous EVENT_FAILURE.
  behaviour.mayReturnDone = false;
  behaviour.mayAccept = true;
  behaviour.mayReject = false;
  // Finite-repeat completion is not yet supported by this lowering (see below).
  behaviour.mayEmitSuccess = (repeat.iterations != 0);
  behaviour.mayEmitFailure = child.mayEmitFailure || child.mayReject;
  behaviour.mayEmitAborted = child.mayEmitAborted;

  // Repeat count/time are intentionally not encoded here yet. This is the
  // unbounded behavioural Repeat semantics used by the reference model.
  //
  // An asynchronous successful iteration moves through Retrigger before the
  // body is invoked again. This transition boundary prevents cyclic
  // combinational dependencies such as:
  //
  //   child.outcome -> childTrigger -> child.triggerReply -> repeat.outcome
  //
  // Retrigger is therefore an encoding state, not an observable KODA state.

  const auto idle = equal(state, reference("Idle"));
  const auto running = equal(state, reference("Running"));
  const auto retrigger = equal(state, reference("Retrigger"));
  const auto error = equal(state, reference("Error"));

  const auto success = equal(child.outcome, constants::eventSuccess());
  const auto failure = equal(child.outcome, constants::eventFailure());

  // Initial trigger only schedules the first iteration.  The Retrigger
  // state represents a deferred dispatch (also used for later iterations).
  // Suppress the scheduled dispatch if the parent aborts before it starts.
  behaviour.definitions.push_back({childTriggerName, logicalAnd(retrigger, logicalNot(abort)), behaviour.origin});
  behaviour.definitions.push_back({childAbortName, logicalAnd(running, abort), behaviour.origin});
  behaviour.definitions.push_back({childResetName, allOf({error, reset, child.error}), behaviour.origin});
  behaviour.triggerReply = cases({{logicalAnd(idle, trigger), constants::resultSuccess()}}, constants::resultNone());

  behaviour.abortReply = cases({{logicalAnd(retrigger, abort), constants::resultSuccess()}, {running, child.abortReply}, {error, constants::resultError()}},
                               constants::resultNone());

  const auto childResetSucceeded = logicalOr(logicalNot(child.error), equal(child.resetReply, constants::resultSuccess()));
  behaviour.resetReply = cases(
    {
      {allOf({error, reset, childResetSucceeded}), constants::resultSuccess()},
      {allOf({error, reset, child.error, equal(child.resetReply, constants::resultFailure())}), constants::resultFailure()},
    },
    constants::resultNone());

  behaviour.outcome = cases(
    {
      // Failure of an asynchronously running iteration.
      {logicalAnd(running, failure), constants::eventFailure()},

      // Result of starting the next iteration.
      {logicalAnd(retrigger, equal(child.triggerReply, constants::resultFailure())), constants::eventFailure()},

      // A synchronously completed iteration of an unbounded Repeat must
      // not signal completion of the whole Repeat.
      {allOf({retrigger, equal(child.triggerReply, constants::resultDone()), boolean(repeat.iterations != 0)}), constants::eventSuccess()},
    },
    constants::eventNone());

  behaviour.nextAssignments.push_back(
    {id + "_state",
     {
       // Initial invocation queues, but does not invoke, iteration 1.
       {logicalAnd(idle, trigger), reference("Retrigger")},
       // Async completion schedules another iteration.
       // Do NOT retrigger combinationally here.
       {logicalAnd(running, success), reference("Retrigger")},
       {logicalAnd(running, failure), reference("Error")},
       // Execute the scheduled retrigger.
       {logicalAnd(retrigger, equal(child.triggerReply, constants::resultSuccess())), reference("Running")},
       {logicalAnd(retrigger, equal(child.triggerReply, constants::resultFailure())), reference("Error")},
       // For an infinite Repeat, synchronous DONE schedules the
       // next iteration, again in a separate macrostep.
       {allOf({retrigger, equal(child.triggerReply, constants::resultDone()), boolean(repeat.iterations == 0)}), reference("Retrigger")},
       // The finite-count implementation still needs an
       // iteration counter; this is a provisional terminal
       // transition, not a proof of bounded Repeat semantics.
       {allOf({retrigger, equal(child.triggerReply, constants::resultDone()), boolean(repeat.iterations != 0)}), reference("Idle")},
       // Abort a deferred (not yet running) iteration.
       {logicalAnd(retrigger, abort), reference("Idle")},
       // External abort while the body is running.
       {allOf({running, abort, equal(child.abortReply, constants::resultSuccess())}), reference("Idle")},
       {allOf({running, abort, equal(child.abortReply, constants::resultFailure())}), reference("Error")},
       // Recovery from Error.
       {allOf({error, reset, childResetSucceeded}), reference("Idle")},
     },
     state,
     behaviour.origin});

  // Reference-level requirement (KODA v0.4): Repeat accepts immediately;
  // the first body attempt runs in a later macrostep.
  contracts::always(behaviour, "repeat_trigger_is_accepted",
                    contracts::implies(allOf({idle, trigger}), equal(behaviour.triggerReply, constants::resultSuccess())));
  contracts::nextState(behaviour, "initial_trigger_schedules_iteration", allOf({idle, trigger}), retrigger);
  contracts::always(behaviour, "initial_body_trigger_is_deferred", contracts::implies(allOf({idle, trigger}), logicalNot(reference(childTriggerName))));
  if (child.mayAccept)
    contracts::nextState(behaviour, "accepted_body_running", allOf({retrigger, equal(child.triggerReply, constants::resultSuccess())}), running);
  if (child.mayReject)
    contracts::nextState(behaviour, "rejected_body_error", allOf({retrigger, equal(child.triggerReply, constants::resultFailure())}), error);
  if (child.mayEmitSuccess)
    contracts::nextState(behaviour, "iteration_success_defers_retrigger", allOf({running, success}), retrigger);
  if (child.mayEmitFailure)
    contracts::nextState(behaviour, "iteration_failure_error", allOf({running, failure}), error);
  if (child.mayAccept)
    contracts::nextState(behaviour, "retrigger_accepted_running", allOf({retrigger, equal(child.triggerReply, constants::resultSuccess())}), running);
  if (child.mayReject)
  {
    const auto rejected = allOf({retrigger, equal(child.triggerReply, constants::resultFailure())});
    contracts::nextState(behaviour, "retrigger_rejected_error", rejected, error);
    contracts::always(behaviour, "retrigger_rejection_emits_failure", contracts::implies(rejected, equal(behaviour.outcome, constants::eventFailure())));
  }
  contracts::nextState(behaviour, "deferred_abort_cancels_iteration", allOf({retrigger, abort}), idle);
  if (child.mayReturnDone && repeat.iterations == 0)
    contracts::always(behaviour, "unbounded_sync_done_defers_next_iteration",
                      contracts::implies(allOf({retrigger, equal(child.triggerReply, constants::resultDone())}),
                                         allOf({equal(behaviour.outcome, constants::eventNone()), unary("X", retrigger)})));
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);

  mergeBehaviour(behaviour, std::move(child));

  if (!repeat.handlers.empty())
    materializeBehaviourInterface(behaviour, "_core");

  return applyStrategyHandlers(id, repeat.handlers, behaviour);
}

VoidResult SemanticCompiler::compileRetry(const ir::PStrategy& strategy, const ir::Strategy::Retry& retry, const PExpression& trigger, const PExpression& abort,
                                          const PExpression& reset, Behaviour& behaviour)
{
  if (!retry.body)
    return VoidResult::Failed("nuXmv Retry requires a body");
  if (retry.iterations < 1)
    return VoidResult::Failed("nuXmv Retry requires at least one attempt");

  const auto [id, localName] = uniqueId(strategy, "retry");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  const auto attempts = reference(id + "_attempts");
  const auto childTriggerName = id + "_body_trigger";
  const auto childAbortName = id + "_body_abort";
  const auto childResetName = id + "_body_reset";

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;

  const auto idle = equal(state, reference("Idle"));
  const auto running = equal(state, reference("Running"));
  const auto resetting = equal(state, reference("Resetting"));
  const auto retrigger = equal(state, reference("Retrigger"));
  const auto error = equal(state, reference("Error"));
  const auto noAbort = logicalNot(abort);

  // attempts counts invocations already started.
  const auto canRetry = binary(attempts, "<", integer(retry.iterations));
  const auto lastAttempt = logicalNot(canRetry);

  // In Retrigger, the next attempt has not yet been counted.
  const auto canRetryAfterTrigger = binary(attempts, "<", integer(retry.iterations - 1));
  const auto lastTriggerAttempt = logicalNot(canRetryAfterTrigger);

  behaviour.error = error;
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Resetting, Retrigger, Error}", reference("Idle"), behaviour.origin});
  behaviour.variables.push_back({VariableKind::State, id + "_attempts", "0.." + std::to_string(retry.iterations), integer(0), behaviour.origin});

  Behaviour child;
  RETURN_ON_FAILURE(compileStrategy(retry.body, reference(childTriggerName), reference(childAbortName), reference(childResetName), child));

  materializeBehaviourInterface(child);
  behaviour.mayReturnDone = child.mayReturnDone;
  behaviour.mayAccept = child.mayAccept || (retry.iterations > 1 && child.mayReject);
  behaviour.mayReject = retry.iterations == 1 && child.mayReject;
  behaviour.mayEmitSuccess = child.mayEmitSuccess || (retry.iterations > 1 && child.mayReject && child.mayReturnDone);
  behaviour.mayEmitFailure = child.mayEmitFailure || (retry.iterations > 1 && child.mayReject);
  behaviour.mayEmitAborted = child.mayEmitAborted;

  const auto childAccepted = equal(child.triggerReply, constants::resultSuccess());
  const auto childRejected = equal(child.triggerReply, constants::resultFailure());
  const auto childDone = equal(child.triggerReply, constants::resultDone());

  const auto childSuccess = equal(child.outcome, constants::eventSuccess());
  const auto childFailure = equal(child.outcome, constants::eventFailure());
  const auto childAborted = equal(child.outcome, constants::eventAborted());

  const auto abortAccepted = equal(child.abortReply, constants::resultSuccess());
  const auto abortRejected = equal(child.abortReply, constants::resultFailure());

  const auto childResetSucceeded = logicalOr(logicalNot(child.error), equal(child.resetReply, constants::resultSuccess()));

  const auto childResetFailed = allOf({child.error, equal(child.resetReply, constants::resultFailure())});

  const auto initial = logicalAnd(idle, trigger);
  const auto internalReset = logicalAnd(resetting, noAbort);
  // const auto internalTrigger = logicalAnd(retrigger, noAbort);
  // With a single allowed attempt, Retrigger is unreachable. Avoid keeping
  // an abort-dependent (and unreachable) signal in the body trigger graph:
  // a parent Join may inspect this Retry trigger reply to decide startup
  // cleanup, which otherwise creates a combinational DEFINE cycle.
  const auto internalTrigger = retry.iterations > 1 ? logicalAnd(retrigger, noAbort) : boolean(false);

  // Child invocation signals.
  // Reset and retrigger occur in separate transitions.
  behaviour.definitions.push_back({childTriggerName, logicalOr(initial, internalTrigger), behaviour.origin});
  behaviour.definitions.push_back({childAbortName, logicalAnd(running, abort), behaviour.origin});
  behaviour.definitions.push_back({childResetName, allOf({logicalOr(internalReset, logicalAnd(error, reset)), child.error}), behaviour.origin});

  // A synchronously rejected initial trigger is accepted by Retry
  // if there are attempts remaining.
  behaviour.triggerReply = cases(
    {
      {allOf({initial, childDone}), constants::resultDone()},
      {allOf({initial, childAccepted}), constants::resultSuccess()},
      {allOf({initial, childRejected}), retry.iterations > 1 ? constants::resultSuccess() : constants::resultFailure()},
    },
    constants::resultNone());

  behaviour.abortReply = cases(
    {
      {logicalAnd(running, abort), child.abortReply},

      // The child may still require reset. Report failure
      // rather than pretending that cancellation completed.
      {logicalAnd(resetting, abort), constants::resultFailure()},

      // No child invocation is active before retrigger.
      {logicalAnd(retrigger, abort), constants::resultSuccess()},
      {logicalAnd(error, abort), constants::resultError()},
    },
    constants::resultNone());

  behaviour.resetReply = cases(
    {
      {allOf({error, reset, childResetFailed}), constants::resultFailure()},
      {allOf({error, reset, childResetSucceeded}), constants::resultSuccess()},
    },
    constants::resultNone());

  // Only report terminal outcomes. Intermediate failed attempts
  // remain internal to Retry.
  behaviour.outcome = cases(
    {
      {logicalAnd(running, childSuccess), constants::eventSuccess()},
      {allOf({running, childFailure, lastAttempt}), constants::eventFailure()},
      {logicalAnd(running, childAborted), constants::eventAborted()},
      {allOf({internalReset, childResetFailed}), constants::eventFailure()},
      {allOf({internalTrigger, childRejected, lastTriggerAttempt}), constants::eventFailure()},
      {allOf({internalTrigger, childDone}), constants::eventSuccess()},
    },
    constants::eventNone());

  behaviour.nextAssignments.push_back({id + "_state",
                                       {
                                         // Initial invocation.
                                         {allOf({initial, childAccepted}), reference("Running")},
                                         {allOf({initial, childRejected}), reference(retry.iterations > 1 ? "Resetting" : "Error")},

                                         // External abort.
                                         {allOf({running, abort, abortAccepted}), reference("Idle")},
                                         {allOf({running, abort, abortRejected}), reference("Error")},

                                         // Asynchronous completion.
                                         {allOf({running, childSuccess}), reference("Idle")},
                                         {allOf({running, childAborted}), reference("Idle")},

                                         // Failed attempt.
                                         {allOf({running, childFailure, canRetry}), reference("Resetting")},
                                         {allOf({running, childFailure, lastAttempt}), reference("Error")},

                                         // Reset before the next attempt.
                                         {logicalAnd(resetting, abort), reference("Error")},
                                         {allOf({internalReset, childResetFailed}), reference("Error")},
                                         {allOf({internalReset, childResetSucceeded}), reference("Retrigger")},

                                         // Start the next attempt.
                                         {logicalAnd(retrigger, abort), reference("Idle")},
                                         {allOf({internalTrigger, childAccepted}), reference("Running")},
                                         {allOf({internalTrigger, childRejected, canRetryAfterTrigger}), reference("Resetting")},
                                         {allOf({internalTrigger, childRejected, lastTriggerAttempt}), reference("Error")},
                                         {allOf({internalTrigger, childDone}), reference("Idle")},

                                         // External reset after terminal failure.
                                         {allOf({error, reset, childResetSucceeded}), reference("Idle")},
                                       },
                                       state,
                                       behaviour.origin});

  std::vector<Expression::Case::Branch> incrementCases;
  for (int i = 0; i < retry.iterations; ++i)
    incrementCases.push_back({equal(attempts, integer(i)), integer(i + 1)});

  const auto incrementAttempts = cases(std::move(incrementCases), integer(retry.iterations));

  // Count total attempts, not the number of retries.
  behaviour.nextAssignments.push_back({id + "_attempts",
                                       {
                                         {allOf({initial, logicalOr(childAccepted, childRejected)}), integer(1)},
                                         {allOf({internalTrigger, childDone}), integer(0)},
                                         {allOf({internalTrigger, logicalOr(childAccepted, childRejected)}), incrementAttempts},
                                         {logicalAnd(running, childSuccess), integer(0)},
                                         {logicalAnd(running, childAborted), integer(0)},
                                         {allOf({running, abort, abortAccepted}), integer(0)},
                                         {logicalAnd(retrigger, abort), integer(0)},
                                         {allOf({error, reset, childResetSucceeded}), integer(0)},
                                       },
                                       attempts,
                                       behaviour.origin});

  if (child.mayAccept)
    contracts::nextState(behaviour, "accepted_first_attempt_running", allOf({initial, childAccepted}), running);
  if (child.mayReject)
    contracts::nextState(behaviour, "rejected_first_attempt", allOf({initial, childRejected}),
                         equal(state, reference(retry.iterations > 1 ? "Resetting" : "Error")));
  if (child.mayEmitSuccess)
    contracts::always(behaviour, "successful_attempt_stops_retries",
                      contracts::implies(allOf({running, childSuccess}), allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
  if (child.mayEmitFailure)
  {
    contracts::nextState(behaviour, "failed_attempt_with_budget_resets", allOf({running, childFailure, canRetry}), resetting);
    contracts::nextState(behaviour, "failed_final_attempt_errors", allOf({running, childFailure, lastAttempt}), error);
  }
  contracts::nextState(behaviour, "successful_internal_reset_defers_retry", allOf({internalReset, childResetSucceeded}), retrigger);
  contracts::nextState(behaviour, "failed_internal_reset_errors", allOf({internalReset, childResetFailed}), error);
  if (child.mayAccept)
    contracts::nextState(behaviour, "retrigger_accepted_running", allOf({internalTrigger, childAccepted}), running);
  if (child.mayReturnDone)
    contracts::always(behaviour, "retrigger_done_succeeds",
                      contracts::implies(allOf({internalTrigger, childDone}), allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);

  mergeBehaviour(behaviour, std::move(child));

  if (!retry.handlers.empty())
    materializeBehaviourInterface(behaviour, "_core");

  return applyStrategyHandlers(id, retry.handlers, behaviour);
}

VoidResult SemanticCompiler::compileJoin(const ir::PStrategy& strategy, const ir::Strategy::Join& join, const PExpression& trigger, const PExpression& abort,
                                         const PExpression& reset, Behaviour& behaviour)
{
  if (join.items.empty())
    return VoidResult::Failed("nuXmv Join requires at least one child");

  const auto [id, localName] = uniqueId(strategy, "join");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  const auto failureAbort = reference(id + "_failure_abort");

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));

  // Aborting is an internal encoding state. It separates detection of a child
  // failure / external abort from cancellation of the remaining children,
  // avoiding combinational cycles through sibling outcomes.
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Aborting, Error}", reference("Idle"), behaviour.origin});
  behaviour.variables.push_back({VariableKind::State, id + "_failure_abort", "boolean", boolean(false), behaviour.origin});

  std::vector<Behaviour> children(join.items.size());
  std::vector<PExpression> completed;

  for (std::size_t i = 0; i < join.items.size(); ++i)
  {
    const auto p = id + "_child" + std::to_string(i);

    behaviour.variables.push_back({VariableKind::State, p + "_completed", "boolean", boolean(false), behaviour.origin});
    completed.push_back(reference(p + "_completed"));

    RETURN_ON_FAILURE(compileStrategy(join.items[i], reference(p + "_trigger"), reference(p + "_abort"), reference(p + "_reset"), children[i]));
    materializeBehaviourInterface(children[i]);
  }

  behaviour.mayReturnDone = std::all_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReturnDone; });
  behaviour.mayAccept = contracts::anyMayAccept(children);
  behaviour.mayReject = std::any_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReject; });
  behaviour.mayEmitSuccess = behaviour.mayAccept;
  behaviour.mayEmitFailure = behaviour.mayAccept;
  behaviour.mayEmitAborted = false;  // current Join outcome has no EVENT_ABORTED branch

  const auto idle = equal(state, reference("Idle"));
  const auto running = equal(state, reference("Running"));
  const auto aborting = equal(state, reference("Aborting"));
  const auto error = equal(state, reference("Error"));

  // -------------------------------------------------------------------------
  // Child control
  // -------------------------------------------------------------------------

  // All Join children must be attempted at launch, including children after
  // one which rejects its trigger synchronously. Unlike Sequence, Join has no
  // short-circuiting startup order.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto p = id + "_child" + std::to_string(i);
    behaviour.definitions.push_back({p + "_trigger", logicalAnd(idle, trigger), behaviour.origin});
  }

  // The startup replies do not depend on any abort request. We may therefore
  // inspect all of them and cancel just-accepted siblings synchronously,
  // preserving the KODA macrostep boundary for primitive async actions.
  // Runtime failures retain the existing deferred Aborting state until that
  // separate behavior is validated.
  std::vector<PExpression> rejectedAtStartup;
  for (const auto& child : children)
    if (child.mayReject)
      rejectedAtStartup.push_back(equal(child.triggerReply, constants::resultFailure()));

  const bool startupMayFail = !rejectedAtStartup.empty();
  const auto startupFailed = anyOf(rejectedAtStartup);

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto p = id + "_child" + std::to_string(i);
    PExpression startupCleanup = boolean(false);
    if (startupMayFail && children[i].mayAccept)
    {
      // An invocation that can neither reject nor return DONE is necessarily
      // accepted when triggered. Do not feed its abort signal back through
      // its triggerReply: nested Retry may have an abort-sensitive *later*
      // retrigger path, even though the initial reply is abort-independent.
      const bool alwaysAccepted = !children[i].mayReject && !children[i].mayReturnDone;
      startupCleanup = alwaysAccepted ? allOf({idle, trigger, startupFailed})
                                      : allOf({idle, trigger, startupFailed, equal(children[i].triggerReply, constants::resultSuccess())});
    }

    const auto laterCleanup = logicalAnd(logicalNot(completed[i]), logicalOr(aborting, logicalAnd(running, abort)));
    behaviour.definitions.push_back({p + "_abort", logicalOr(startupCleanup, laterCleanup), behaviour.origin});
    behaviour.definitions.push_back({p + "_reset", allOf({error, reset, children[i].error}), behaviour.origin});
  }

  // -------------------------------------------------------------------------
  // Synchronous startup result
  // -------------------------------------------------------------------------

  std::vector<PExpression> startupDone;
  std::vector<PExpression> startupNonFailure;

  for (auto& c : children)
  {
    if (c.mayReturnDone)
      startupDone.push_back(equal(c.triggerReply, constants::resultDone()));
    if (c.mayReject)
      startupNonFailure.push_back(notEqual(c.triggerReply, constants::resultFailure()));
  }

  const auto allStartupDone = behaviour.mayReturnDone ? allOf(startupDone) : boolean(false);
  const auto allStartupNonFailure = allOf(startupNonFailure);

  std::vector<Expression::Case::Branch> joinTriggerCases;
  if (behaviour.mayReturnDone)
    joinTriggerCases.push_back({allOf({idle, trigger, allStartupDone}), constants::resultDone()});
  joinTriggerCases.push_back({allOf({idle, trigger, allStartupNonFailure}), constants::resultSuccess()});
  joinTriggerCases.push_back({allOf({idle, trigger}), constants::resultFailure()});
  behaviour.triggerReply = cases(std::move(joinTriggerCases), constants::resultNone());

  // -------------------------------------------------------------------------
  // Runtime completion/failure
  // -------------------------------------------------------------------------

  std::vector<PExpression> failedEvents;

  for (auto& c : children)
    failedEvents.push_back(equal(c.outcome, constants::eventFailure()));

  const auto anyFailure = anyOf(failedEvents);

  std::vector<PExpression> doneNow;

  for (std::size_t i = 0; i < children.size(); ++i)
    doneNow.push_back(logicalOr(completed[i], equal(children[i].outcome, constants::eventSuccess())));

  const auto allDoneNow = allOf(doneNow);

  std::vector<PExpression> abortSucceeded;
  std::vector<PExpression> abortFailed;
  std::vector<PExpression> abortRunning;

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    // Already completed children require no cleanup.
    abortSucceeded.push_back(anyOf({completed[i], children[i].error, equal(children[i].abortReply, constants::resultSuccess())}));
    abortFailed.push_back(allOf({logicalNot(completed[i]), logicalNot(children[i].error),
                                 equal(children[i].abortReply, constants::resultFailure())}));  // abortRunning.push_back(logicalAnd(logicalNot(completed[i]),
                                                                                                // equal(children[i].abortReply, constants::resultRunning())));
  }

  const auto allAbortSucceeded = allOf(abortSucceeded);
  const auto anyAbortFailed = anyOf(abortFailed);
  const auto cleanupComplete = logicalOr(allAbortSucceeded, anyAbortFailed);
  // const auto anyAbortRunning = anyOf(abortRunning);

  // -------------------------------------------------------------------------
  // Public interface
  // -------------------------------------------------------------------------

  // External abort is accepted while Running. Cleanup itself occurs in the
  // subsequent Aborting state.
  behaviour.abortReply = cases(
    {
      {logicalAnd(running, anyAbortFailed), constants::resultFailure()},
      // {logicalAnd(running, anyAbortRunning), constants::resultRunning()},
      {logicalAnd(running, allAbortSucceeded), constants::resultSuccess()},
      {error, constants::resultError()},
    },
    constants::resultNone());

  std::vector<PExpression> resetSucceeded;
  std::vector<PExpression> resetFailed;
  for (const auto& child : children)
  {
    resetSucceeded.push_back(logicalOr(logicalNot(child.error), equal(child.resetReply, constants::resultSuccess())));
    resetFailed.push_back(logicalAnd(child.error, equal(child.resetReply, constants::resultFailure())));
  }

  const auto allResetSucceeded = allOf(resetSucceeded);
  const auto anyResetFailed = anyOf(resetFailed);
  behaviour.resetReply = cases(
    {
      {allOf({equal(state, reference("Error")), reset, anyResetFailed}), constants::resultFailure()},
      {allOf({equal(state, reference("Error")), reset, allResetSucceeded}), constants::resultSuccess()},
    },
    constants::resultNone());

  // A successful Join still completes immediately when all children finish.
  //
  // Failure is exposed after entering Aborting. This ensures sibling cleanup
  // is no longer combinationally connected to the failing child.
  behaviour.outcome = cases(
    {
      // {logicalAnd(aborting, failureAbort), constants::eventFailure()},
      {allOf({aborting, failureAbort, logicalOr(allAbortSucceeded, anyAbortFailed)}), constants::eventFailure()},
      {logicalAnd(running, allDoneNow), constants::eventSuccess()},
    },
    constants::eventNone());

  // -------------------------------------------------------------------------
  // Join state
  // -------------------------------------------------------------------------

  behaviour.nextAssignments.push_back({id + "_state",
                                       {
                                         // Synchronous startup failure.
                                         {allOf({
                                            idle,
                                            trigger,
                                            logicalNot(allStartupNonFailure),
                                          }),
                                          reference("Error")},

                                         // At least one child remains asynchronously active.
                                         {allOf({
                                            idle,
                                            trigger,
                                            allStartupNonFailure,
                                            logicalNot(allStartupDone),
                                          }),
                                          reference("Running")},

                                         // Runtime failure: enter cleanup first.
                                         {logicalAnd(running, anyFailure), reference("Aborting")},

                                         {allOf({running, abort, allAbortSucceeded}), reference("Idle")},
                                         {allOf({running, abort, anyAbortFailed}), reference("Error")},

                                         // Normal successful completion.
                                         {logicalAnd(running, allDoneNow), reference("Idle")},

                                         // Cleanup caused by failure finishes in Error.
                                         {allOf({aborting, failureAbort, cleanupComplete}), reference("Error")},
                                         {allOf({aborting, logicalNot(failureAbort), cleanupComplete}), reference("Idle")},

                                         // Reset error state.
                                         {allOf({error, reset, allResetSucceeded}), reference("Idle")},
                                       },
                                       state,
                                       behaviour.origin});

  // Remember why we entered Aborting.
  behaviour.nextAssignments.push_back({id + "_failure_abort",
                                       {
                                         {logicalAnd(running, anyFailure), boolean(true)},
                                         {logicalAnd(error, reset), boolean(false)},
                                       },
                                       failureAbort,
                                       behaviour.origin});
  // -------------------------------------------------------------------------
  // Child completion bookkeeping
  // -------------------------------------------------------------------------

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto name = id + "_child" + std::to_string(i) + "_completed";

    behaviour.nextAssignments.push_back({name,
                                         {
                                           // Child completed synchronously during startup.
                                           {idle, equal(children[i].triggerReply, constants::resultDone())},

                                           // Child completed asynchronously.
                                           {logicalAnd(running, equal(children[i].outcome, constants::eventSuccess())), boolean(true)},

                                           // Clear bookkeeping after cleanup.
                                           {aborting, boolean(false)},

                                           {error, boolean(false)},

                                           // Normal Join completion.
                                           {logicalAnd(running, allDoneNow), boolean(false)},
                                         },
                                         completed[i],
                                         behaviour.origin});
  }

  // At most one asynchronous child event is processed per transition.
  behaviour.constraints.push_back({exactlyOneEvent(children), behaviour.origin});

  // Every child is invoked, regardless of synchronous rejection elsewhere.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto childTrigger = reference(id + "_child" + std::to_string(i) + "_trigger");
    contracts::always(behaviour, "startup_attempts_child" + std::to_string(i), contracts::implies(allOf({idle, trigger}), childTrigger));
    if (children[i].mayAccept)
    {
      const auto accepted = equal(children[i].triggerReply, constants::resultSuccess());
      const auto childAbort = reference(id + "_child" + std::to_string(i) + "_abort");
      contracts::always(behaviour, "startup_failure_cancels_child" + std::to_string(i),
                        contracts::implies(allOf({idle, trigger, startupFailed, accepted}), childAbort));
    }
  }
  if (behaviour.mayReject)
    contracts::nextState(behaviour, "startup_rejection_errors", allOf({idle, trigger, startupFailed}), error);
  if (behaviour.mayAccept)
  {
    contracts::nextState(behaviour, "startup_accepted_runs", allOf({idle, trigger, allStartupNonFailure, logicalNot(allStartupDone)}), running);
    contracts::nextState(behaviour, "runtime_failure_starts_cleanup", logicalAnd(running, anyFailure), aborting);
    contracts::always(behaviour, "all_children_succeed",
                      contracts::implies(allOf({running, allDoneNow, logicalNot(anyFailure)}),
                                         allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
    contracts::nextState(behaviour, "failure_cleanup_enters_error", allOf({aborting, failureAbort, cleanupComplete}), error);
  }
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);

  for (auto& c : children)
    mergeBehaviour(behaviour, std::move(c));

  return {};
}

VoidResult SemanticCompiler::compileEither(const ir::PStrategy& strategy, const ir::Strategy::Either& either, const PExpression& trigger,
                                           const PExpression& abort, const PExpression& reset, Behaviour& behaviour)
{
  if (either.items.empty())
    return VoidResult::Failed("nuXmv Either requires at least one child");

  const auto [id, localName] = uniqueId(strategy, "either");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Error}", reference("Idle"), behaviour.origin});

  std::vector<Behaviour> children(either.items.size());
  std::vector<PExpression> failed;
  for (std::size_t i = 0; i < either.items.size(); ++i)
  {
    const auto p = id + "_child" + std::to_string(i);
    behaviour.variables.push_back({VariableKind::State, p + "_failed", "boolean", boolean(false), behaviour.origin});
    failed.push_back(reference(p + "_failed"));
    RETURN_ON_FAILURE(compileStrategy(either.items[i], reference(p + "_trigger"), reference(p + "_abort"), reference(p + "_reset"), children[i]));
    materializeBehaviourInterface(children[i]);
  }

  behaviour.mayReturnDone = std::any_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReturnDone; });
  behaviour.mayAccept = contracts::anyMayAccept(children);
  behaviour.mayReject = std::all_of(children.begin(), children.end(), [](const Behaviour& child) { return child.mayReject; });
  behaviour.mayEmitSuccess = behaviour.mayAccept;
  behaviour.mayEmitFailure = behaviour.mayAccept;
  behaviour.mayEmitAborted = false;

  const auto idle = equal(state, reference("Idle")), running = equal(state, reference("Running")), error = equal(state, reference("Error"));

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto p = id + "_child" + std::to_string(i);
    behaviour.definitions.push_back({p + "_trigger", logicalAnd(idle, trigger), behaviour.origin});
    std::vector<PExpression> otherSuccess;
    for (std::size_t j = 0; j < children.size(); ++j)
      if (j != i)
        otherSuccess.push_back(equal(children[j].outcome, constants::eventSuccess()));
    behaviour.definitions.push_back(
      {p + "_abort", logicalAnd(running, logicalOr(abort, otherSuccess.empty() ? boolean(false) : anyOf(otherSuccess))), behaviour.origin});
    behaviour.definitions.push_back({p + "_reset", allOf({error, reset, children[i].error}), behaviour.origin});
  }

  std::vector<PExpression> syncDone, syncFailure, syncRunning;
  for (auto& c : children)
  {
    syncDone.push_back(equal(c.triggerReply, constants::resultDone()));
    syncFailure.push_back(equal(c.triggerReply, constants::resultFailure()));
    syncRunning.push_back(equal(c.triggerReply, constants::resultSuccess()));
  }
  behaviour.triggerReply = cases({{logicalAnd(idle, anyOf(syncDone)), constants::resultDone()},
                                  {logicalAnd(idle, anyOf(syncRunning)), constants::resultSuccess()},
                                  {logicalAnd(idle, allOf(syncFailure)), constants::resultFailure()}},
                                 constants::resultNone());

  std::vector<PExpression> asyncSuccess, asyncFailure, failedNow;
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    asyncSuccess.push_back(equal(children[i].outcome, constants::eventSuccess()));
    asyncFailure.push_back(equal(children[i].outcome, constants::eventFailure()));
    failedNow.push_back(logicalOr(failed[i], equal(children[i].outcome, constants::eventFailure())));
  }
  behaviour.outcome = cases({{logicalAnd(running, anyOf(asyncSuccess)), constants::eventSuccess()},
                             {logicalAnd(running, allOf(failedNow)), constants::eventFailure()}},
                            constants::eventNone());
  behaviour.abortReply = cases({{running, constants::resultSuccess()}, {error, constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{error, constants::resultSuccess()}}, constants::resultNone());

  behaviour.nextAssignments.push_back({id + "_state",
                                       {{logicalAnd(idle, anyOf(syncDone)), reference("Idle")},
                                        {logicalAnd(idle, allOf(syncFailure)), reference("Error")},
                                        {logicalAnd(idle, anyOf(syncRunning)), reference("Running")},
                                        {logicalAnd(running, anyOf(asyncSuccess)), reference("Idle")},
                                        {logicalAnd(running, allOf(failedNow)), reference("Error")},
                                        {logicalAnd(running, abort), reference("Idle")},
                                        {logicalAnd(error, reset), reference("Idle")}},
                                       state,
                                       behaviour.origin});

  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto name = id + "_child" + std::to_string(i) + "_failed";
    behaviour.nextAssignments.push_back({name,
                                         {{logicalAnd(idle, trigger), equal(children[i].triggerReply, constants::resultFailure())},
                                          {logicalAnd(running, equal(children[i].outcome, constants::eventFailure())), boolean(true)},
                                          {logicalOr(abort, logicalAnd(running, anyOf(asyncSuccess))), boolean(false)},
                                          {logicalAnd(error, reset), boolean(false)}},
                                         failed[i],
                                         behaviour.origin});
  }

  behaviour.constraints.push_back({exactlyOneEvent(children), behaviour.origin});
  for (std::size_t i = 0; i < children.size(); ++i)
    contracts::always(behaviour, "startup_attempts_child" + std::to_string(i),
                      contracts::implies(allOf({idle, trigger}), reference(id + "_child" + std::to_string(i) + "_trigger")));
  if (behaviour.mayAccept)
  {
    contracts::nextState(behaviour, "accepted_branch_runs", logicalAnd(idle, anyOf(syncRunning)), running);
    contracts::always(behaviour, "winner_succeeds",
                      contracts::implies(logicalAnd(running, anyOf(asyncSuccess)),
                                         allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
  }
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);
  for (auto& c : children)
    mergeBehaviour(behaviour, std::move(c));
  return {};
}

VoidResult SemanticCompiler::compileWithin(const ir::PStrategy& strategy, const ir::Strategy::Within& within, const PExpression& trigger,
                                           const PExpression& abort, const PExpression& reset, Behaviour& behaviour)
{
  if (!within.body || !within.fallback)
    return VoidResult::Failed("nuXmv Within requires body and fallback");

  const auto [id, localName] = uniqueId(strategy, "within");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  const auto timeout = reference(id + "_timeout");
  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = equal(state, reference("Error"));
  behaviour.variables.push_back({VariableKind::Input, id + "_timeout", "boolean", nullptr, behaviour.origin});

  Behaviour body, fallback;
  RETURN_ON_FAILURE(compileStrategy(within.body, reference(id + "_body_trigger"), reference(id + "_body_abort"), reference(id + "_body_reset"), body));
  materializeBehaviourInterface(body);
  RETURN_ON_FAILURE(
    compileStrategy(within.fallback, reference(id + "_fallback_trigger"), reference(id + "_fallback_abort"), reference(id + "_fallback_reset"), fallback));
  materializeBehaviourInterface(fallback);

  // A synchronous fallback finishes in the timeout/completion macrostep and
  // never occupies Else. Waiting records an abort that returned HANDLING
  // (RESULT_RUNNING in the current nuXmv interface).
  behaviour.variables.push_back({VariableKind::State, id + "_state", fallback.mayAccept ? "{Idle, Do, Waiting, Else, Error}" : "{Idle, Do, Waiting, Error}",
                                 reference("Idle"), behaviour.origin});
  behaviour.mayReturnDone = body.mayReturnDone;  // Fallback is only started later.
  behaviour.mayAccept = body.mayAccept;
  behaviour.mayReject = body.mayReject;
  behaviour.mayEmitSuccess = body.mayAccept;
  behaviour.mayEmitFailure = body.mayAccept;
  behaviour.mayEmitAborted = body.mayEmitAborted || fallback.mayEmitAborted;

  const auto idle = equal(state, reference("Idle"));
  const auto doing = equal(state, reference("Do"));
  const auto waiting = equal(state, reference("Waiting"));
  const auto elsing = fallback.mayAccept ? equal(state, reference("Else")) : boolean(false);
  const auto error = equal(state, reference("Error"));
  const auto bodySuccess = equal(body.outcome, constants::eventSuccess());
  const auto bodyFailure = equal(body.outcome, constants::eventFailure());
  const auto bodyAborted = equal(body.outcome, constants::eventAborted());
  const auto fbSuccess = equal(fallback.outcome, constants::eventSuccess());
  const auto fbFailure = equal(fallback.outcome, constants::eventFailure());
  const auto fbAborted = equal(fallback.outcome, constants::eventAborted());

  // A timeout is an independent scheduler dispatch, not an external task
  // abort. If completion and timeout are represented in the same SMV state,
  // the already-observed body outcome wins over the timeout.
  const auto timeoutRequested = allOf({doing, timeout, logicalNot(abort)});
  const auto timeoutCanAct = allOf({timeoutRequested, logicalNot(bodySuccess), logicalNot(bodyFailure), logicalNot(bodyAborted)});
  const auto cancellationSucceeded = equal(body.abortReply, constants::resultSuccess());
  const auto cancellationFailed = equal(body.abortReply, constants::resultFailure());
  const auto cancellationPending = equal(body.abortReply, constants::resultRunning());
  const auto timeoutCancelled = logicalAnd(timeoutCanAct, cancellationSucceeded);
  const auto timeoutRejected = logicalAnd(timeoutCanAct, cancellationFailed);
  const auto timeoutHandling = logicalAnd(timeoutCanAct, cancellationPending);
  const auto waitingCancelled = logicalAnd(waiting, logicalOr(bodyAborted, bodySuccess));
  const auto startFallback = logicalOr(timeoutCancelled, waitingCancelled);

  behaviour.definitions.push_back({id + "_body_trigger", logicalAnd(idle, trigger), behaviour.origin});
  behaviour.definitions.push_back({id + "_body_abort", logicalAnd(doing, logicalOr(abort, timeout)), behaviour.origin});
  behaviour.definitions.push_back({id + "_body_reset", allOf({error, reset, body.error}), behaviour.origin});
  // Crucial: never trigger the alternative merely because timeout is TRUE.
  // The primary must have stopped, or HANDLING must have completed.
  behaviour.definitions.push_back({id + "_fallback_trigger", startFallback, behaviour.origin});
  behaviour.definitions.push_back({id + "_fallback_abort", logicalAnd(elsing, abort), behaviour.origin});
  behaviour.definitions.push_back({id + "_fallback_reset", allOf({error, reset, fallback.error}), behaviour.origin});

  behaviour.triggerReply = cases({{allOf({idle, equal(body.triggerReply, constants::resultDone())}), constants::resultDone()},
                                  {allOf({idle, equal(body.triggerReply, constants::resultFailure())}), constants::resultFailure()},
                                  {allOf({idle, equal(body.triggerReply, constants::resultSuccess())}), constants::resultSuccess()}},
                                 constants::resultNone());
  behaviour.abortReply = cases({{doing, body.abortReply}, {elsing, fallback.abortReply}, {error, constants::resultError()}}, constants::resultNone());

  const auto resetSucceeded = allOf({
    logicalOr(logicalNot(body.error), equal(body.resetReply, constants::resultSuccess())),
    logicalOr(logicalNot(fallback.error), equal(fallback.resetReply, constants::resultSuccess())),
  });
  const auto resetFailed = anyOf({
    logicalAnd(body.error, equal(body.resetReply, constants::resultFailure())),
    logicalAnd(fallback.error, equal(fallback.resetReply, constants::resultFailure())),
  });
  behaviour.resetReply = cases(
    {
      {allOf({error, reset, resetFailed}), constants::resultFailure()},
      {allOf({error, reset, resetSucceeded}), constants::resultSuccess()},
    },
    constants::resultNone());

  const auto fallbackAccepted = equal(fallback.triggerReply, constants::resultSuccess());
  const auto fallbackDone = equal(fallback.triggerReply, constants::resultDone());
  const auto fallbackRejected = equal(fallback.triggerReply, constants::resultFailure());
  behaviour.outcome = cases(
    {
      {logicalAnd(doing, bodySuccess), constants::eventSuccess()},
      {logicalAnd(doing, bodyFailure), constants::eventFailure()},
      {logicalAnd(doing, bodyAborted), constants::eventAborted()},
      {timeoutRejected, constants::eventFailure()},
      {logicalAnd(waiting, bodyFailure), constants::eventFailure()},
      {allOf({startFallback, fallbackDone}), constants::eventSuccess()},
      {allOf({startFallback, fallbackRejected}), constants::eventFailure()},
      {logicalAnd(elsing, fbSuccess), constants::eventSuccess()},
      {logicalAnd(elsing, fbFailure), constants::eventFailure()},
      {logicalAnd(elsing, fbAborted), constants::eventAborted()},
    },
    constants::eventNone());

  behaviour.nextAssignments.push_back({id + "_state",
                                       {
                                         {allOf({idle, trigger, equal(body.triggerReply, constants::resultSuccess())}), reference("Do")},
                                         {allOf({idle, trigger, equal(body.triggerReply, constants::resultFailure())}), reference("Error")},
                                         {logicalAnd(doing, bodySuccess), reference("Idle")},
                                         {logicalAnd(doing, bodyFailure), reference("Error")},
                                         {logicalAnd(doing, bodyAborted), reference("Idle")},
                                         {timeoutRejected, reference("Error")},
                                         {timeoutHandling, reference("Waiting")},
                                         {logicalAnd(waiting, bodyFailure), reference("Error")},
                                         {allOf({startFallback, fallbackAccepted}), reference(fallback.mayAccept ? "Else" : "Error")},
                                         {allOf({startFallback, fallbackDone}), reference("Idle")},
                                         {allOf({startFallback, fallbackRejected}), reference("Error")},
                                         {logicalAnd(elsing, fbSuccess), reference("Idle")},
                                         {logicalAnd(elsing, fbFailure), reference("Error")},
                                         {logicalAnd(elsing, fbAborted), reference("Idle")},
                                         {allOf({doing, abort, cancellationFailed}), reference("Error")},
                                         {allOf({doing, abort, cancellationSucceeded}), reference("Idle")},
                                         {logicalAnd(elsing, abort), reference("Idle")},
                                         {allOf({error, reset, equal(behaviour.resetReply, constants::resultSuccess())}), reference("Idle")},
                                       },
                                       state,
                                       behaviour.origin});

  // The clock is abstracted to a scheduler input, which is enabled only when
  // the primary is active. It does not encode elapsed real time.
  behaviour.constraints.push_back({binary(timeout, "->", doing), behaviour.origin});

  // Independently authored contracts from the KODA Within semantics.
  contracts::always(behaviour, "failed_timeout_abort_blocks_fallback", contracts::implies(timeoutRejected, logicalNot(reference(id + "_fallback_trigger"))));
  contracts::always(behaviour, "successful_timeout_starts_fallback", contracts::implies(timeoutCancelled, reference(id + "_fallback_trigger")));
  contracts::always(behaviour, "failed_timeout_reports_failure", contracts::implies(timeoutRejected, equal(behaviour.outcome, constants::eventFailure())));
  contracts::nextState(behaviour, "failed_timeout_enters_error", timeoutRejected, error);
  contracts::nextState(behaviour, "pending_timeout_waits", timeoutHandling, waiting);
  contracts::always(behaviour, "waiting_completion_starts_fallback", contracts::implies(waitingCancelled, reference(id + "_fallback_trigger")));
  contracts::nextState(behaviour, "waiting_failure_enters_error", logicalAnd(waiting, bodyFailure), error);

  if (body.mayAccept)
  {
    contracts::nextState(behaviour, "accepted_body_runs", allOf({idle, trigger, equal(body.triggerReply, constants::resultSuccess())}), doing);
    if (body.mayEmitSuccess)
      contracts::always(behaviour, "body_success_finishes",
                        contracts::implies(logicalAnd(doing, bodySuccess), allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
    if (body.mayEmitFailure)
      contracts::nextState(behaviour, "body_failure_errors", logicalAnd(doing, bodyFailure), error);
    if (body.mayEmitAborted)
      contracts::nextState(behaviour, "body_aborted_finishes", logicalAnd(doing, bodyAborted), idle);
    if (fallback.mayAccept)
      contracts::nextState(behaviour, "timeout_accepted_fallback_runs", allOf({startFallback, fallbackAccepted}), elsing);
    if (fallback.mayReturnDone)
      contracts::always(behaviour, "timeout_sync_fallback_done",
                        contracts::implies(allOf({startFallback, fallbackDone}),
                                           allOf({equal(behaviour.outcome, constants::eventSuccess()), unary("X", idle)})));
    if (fallback.mayReject)
      contracts::always(behaviour, "timeout_fallback_rejects",
                        contracts::implies(allOf({startFallback, fallbackRejected}),
                                           allOf({equal(behaviour.outcome, constants::eventFailure()), unary("X", error)})));
    if (fallback.mayEmitSuccess)
      contracts::nextState(behaviour, "fallback_succeeds", logicalAnd(elsing, fbSuccess), idle);
    if (fallback.mayEmitFailure)
      contracts::nextState(behaviour, "fallback_fails", logicalAnd(elsing, fbFailure), error);
  }
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);
  mergeBehaviour(behaviour, std::move(body));
  mergeBehaviour(behaviour, std::move(fallback));
  if (!within.handlers.empty())
    materializeBehaviourInterface(behaviour, "_core");
  return applyStrategyHandlers(id, within.handlers, behaviour);
}

VoidResult SemanticCompiler::compileEnd(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                                        Behaviour& behaviour)
{
  const auto [id, _] = uniqueId(strategy, "end");

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = boolean(false);

  // End is an immediate successful termination marker. It has no persistent state.
  behaviour.triggerReply = cases({{trigger, constants::resultDone()}}, constants::resultNone());
  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();
  behaviour.mayReturnDone = true;
  behaviour.mayAccept = false;
  behaviour.mayReject = false;
  behaviour.mayEmitSuccess = false;
  behaviour.mayEmitFailure = false;
  behaviour.mayEmitAborted = false;
  // This compiler currently maps explicit success to RESULT_DONE. The KODA
  // v0.4 internal TERMINATE disposition is not yet represented by this backend.
  contracts::always(behaviour, "success_completes_synchronously", contracts::implies(trigger, equal(behaviour.triggerReply, constants::resultDone())));
  return {};
}

VoidResult SemanticCompiler::compileContinue(const ir::PStrategy& strategy, const PExpression& trigger, const PExpression& abort, const PExpression& reset,
                                             Behaviour& behaviour)
{
  const auto [id, _] = uniqueId(strategy, "continue");

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = boolean(false);

  // Continue completes the current body synchronously. The enclosing Repeat consumes
  // RESULT_DONE as its loop-control result. No artificial runtime state is needed.
  behaviour.triggerReply = cases({{trigger, constants::resultDone()}}, constants::resultNone());
  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();
  behaviour.mayReturnDone = true;
  behaviour.mayAccept = false;
  behaviour.mayReject = false;
  behaviour.mayEmitSuccess = false;
  behaviour.mayEmitFailure = false;
  behaviour.mayEmitAborted = false;
  contracts::always(behaviour, "continue_completes_synchronously", contracts::implies(trigger, equal(behaviour.triggerReply, constants::resultDone())));
  return {};
}

VoidResult SemanticCompiler::compileChoose(const ir::PStrategy& strategy, const ir::Strategy::Choose& choose, const PExpression& trigger,
                                           const PExpression& abort, const PExpression& reset, Behaviour& behaviour)
{
  if (choose.options.empty())
    return VoidResult::Failed("nuXmv Choose requires at least one option");

  const auto [id, localName] = uniqueId(strategy, "choose");
  IdContextGuard context(mIdContext, localName);

  const auto state = reference(id + "_state");
  const auto idle = equal(state, reference("Idle"));
  const auto error = equal(state, reference("Error"));
  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = error;

  const auto count = choose.options.size();
  std::vector<Behaviour> options(count);
  std::vector<PExpression> conditions;
  conditions.reserve(count);
  bool hasOtherwise = false;

  for (std::size_t i = 0; i < count; ++i)
  {
    const auto& option = choose.options[i];
    if (!option.strategy)
      return VoidResult::Failed("nuXmv Choose option {} has no strategy", i);

    const auto conditionName = id + "_condition" + std::to_string(i);
    if (isChooseOtherwise(option))
    {
      if (hasOtherwise || i + 1 != count)
        return VoidResult::Failed("nuXmv Choose `when:` must occur once and as the final option");
      hasOtherwise = true;

      // The default is the complement of all previous guards, *not* an
      // independent environment input. Earlier guards remain abstract until
      // KODA data expressions are mapped into the world model.
      std::vector<PExpression> noEarlierGuard;
      for (const auto& earlier : conditions)
        noEarlierGuard.push_back(logicalNot(earlier));
      behaviour.definitions.push_back({conditionName, allOf(noEarlierGuard), behaviour.origin});
    }
    else
    {
      behaviour.variables.push_back({VariableKind::Input, conditionName, "boolean", nullptr, behaviour.origin});
    }
    conditions.push_back(reference(conditionName));

    const auto p = id + "_option" + std::to_string(i);
    RETURN_ON_FAILURE(compileStrategy(option.strategy, reference(p + "_trigger"), reference(p + "_abort"), reference(p + "_reset"), options[i]));
    materializeBehaviourInterface(options[i]);

    std::vector<PExpression> selected{idle, trigger, conditions[i]};
    for (std::size_t j = 0; j < i; ++j)
      selected.push_back(logicalNot(conditions[j]));
    behaviour.definitions.push_back({p + "_trigger", allOf(selected), behaviour.origin});
    behaviour.definitions.push_back(
      {p + "_abort", options[i].mayAccept ? logicalAnd(equal(state, reference("Option" + std::to_string(i))), abort) : boolean(false), behaviour.origin});
    behaviour.definitions.push_back({p + "_reset", allOf({error, reset, options[i].error}), behaviour.origin});
  }

  // Synchronous options never occupy persistent Option<i> states.
  std::string domain = "{Idle, Error";
  for (std::size_t i = 0; i < count; ++i)
    if (options[i].mayAccept)
      domain += ", Option" + std::to_string(i);
  domain += "}";
  behaviour.variables.push_back({VariableKind::State, id + "_state", domain, reference("Idle"), behaviour.origin});

  behaviour.mayReturnDone = !hasOtherwise || std::any_of(options.begin(), options.end(), [](const Behaviour& option) { return option.mayReturnDone; });
  behaviour.mayAccept = contracts::anyMayAccept(options);
  behaviour.mayReject = std::any_of(options.begin(), options.end(), [](const Behaviour& option) { return option.mayReject; });
  behaviour.mayEmitSuccess = std::any_of(options.begin(), options.end(), [](const Behaviour& option) { return option.mayEmitSuccess; });
  behaviour.mayEmitFailure = std::any_of(options.begin(), options.end(), [](const Behaviour& option) { return option.mayEmitFailure; });
  behaviour.mayEmitAborted = std::any_of(options.begin(), options.end(), [](const Behaviour& option) { return option.mayEmitAborted; });

  std::vector<Expression::Case::Branch> triggerCases, outcomeCases, abortCases;
  std::vector<GuardedValue> nextCases;
  for (std::size_t i = 0; i < count; ++i)
  {
    std::vector<PExpression> selected{idle, trigger, conditions[i]};
    for (std::size_t j = 0; j < i; ++j)
      selected.push_back(logicalNot(conditions[j]));
    const auto sel = allOf(selected);
    triggerCases.push_back({sel, options[i].triggerReply});

    const auto active = options[i].mayAccept ? equal(state, reference("Option" + std::to_string(i))) : boolean(false);
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventSuccess())), constants::eventSuccess()});
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventFailure())), constants::eventFailure()});
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventAborted())), constants::eventAborted()});
    abortCases.push_back({logicalAnd(active, abort), options[i].abortReply});

    if (options[i].mayAccept)
      nextCases.push_back({allOf({sel, equal(options[i].triggerReply, constants::resultSuccess())}), reference("Option" + std::to_string(i))});
    nextCases.push_back({allOf({sel, equal(options[i].triggerReply, constants::resultFailure())}), reference("Error")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventSuccess())), reference("Idle")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventFailure())), reference("Error")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventAborted())), reference("Idle")});
    nextCases.push_back({allOf({active, abort, equal(options[i].abortReply, constants::resultSuccess())}), reference("Idle")});
    nextCases.push_back({allOf({active, abort, equal(options[i].abortReply, constants::resultFailure())}), reference("Error")});
  }

  // If every explicit guard is false and there is no otherwise, Choose
  // finishes synchronously. When no trigger is active, it must report NONE.
  std::vector<PExpression> noGuard;
  for (std::size_t i = 0; i < count; ++i)
    if (!isChooseOtherwise(choose.options[i]))
      noGuard.push_back(logicalNot(conditions[i]));
  if (!hasOtherwise)
    triggerCases.push_back({allOf({idle, trigger, allOf(noGuard)}), constants::resultDone()});
  behaviour.triggerReply = cases(std::move(triggerCases), constants::resultNone());
  behaviour.outcome = cases(std::move(outcomeCases), constants::eventNone());
  abortCases.push_back({logicalAnd(error, abort), constants::resultError()});
  behaviour.abortReply = cases(std::move(abortCases), constants::resultNone());

  std::vector<PExpression> failedResets, successfulResets;
  for (const auto& option : options)
  {
    failedResets.push_back(logicalAnd(option.error, equal(option.resetReply, constants::resultFailure())));
    successfulResets.push_back(logicalOr(logicalNot(option.error), equal(option.resetReply, constants::resultSuccess())));
  }
  behaviour.resetReply = cases(
    {
      {allOf({error, reset, anyOf(failedResets)}), constants::resultFailure()},
      {allOf({error, reset, allOf(successfulResets)}), constants::resultSuccess()},
    },
    constants::resultNone());
  nextCases.push_back({allOf({error, reset, equal(behaviour.resetReply, constants::resultSuccess())}), reference("Idle")});
  behaviour.nextAssignments.push_back({id + "_state", std::move(nextCases), state, behaviour.origin});

  if (hasOtherwise)
  {
    contracts::always(behaviour, "otherwise_selected_when_no_guard_matches",
                      contracts::implies(allOf({idle, trigger, allOf(noGuard)}), reference(id + "_option" + std::to_string(count - 1) + "_trigger")));
    contracts::always(behaviour, "otherwise_reply_propagates",
                      contracts::implies(allOf({idle, trigger, allOf(noGuard)}), equal(behaviour.triggerReply, options.back().triggerReply)));
  }
  else
  {
    contracts::always(behaviour, "no_matching_guard_finishes",
                      contracts::implies(allOf({idle, trigger, allOf(noGuard)}), equal(behaviour.triggerReply, constants::resultDone())));
  }
  for (std::size_t i = 0; i < count; ++i)
  {
    std::vector<PExpression> selected{idle, trigger, conditions[i]};
    for (std::size_t j = 0; j < i; ++j)
      selected.push_back(logicalNot(conditions[j]));
    const auto sel = allOf(selected);
    const auto selectedTrigger = reference(id + "_option" + std::to_string(i) + "_trigger");
    contracts::always(behaviour, "selected_option" + std::to_string(i) + "_triggered", contracts::implies(sel, selectedTrigger));
    for (std::size_t j = i + 1; j < count; ++j)
      contracts::always(behaviour, "selected_option" + std::to_string(i) + "_excludes_option" + std::to_string(j),
                        contracts::implies(sel, logicalNot(reference(id + "_option" + std::to_string(j) + "_trigger"))));
    const auto active = options[i].mayAccept ? equal(state, reference("Option" + std::to_string(i))) : boolean(false);
    if (options[i].mayAccept)
      contracts::nextState(behaviour, "accepted_option" + std::to_string(i) + "_active",
                           allOf({sel, equal(options[i].triggerReply, constants::resultSuccess())}), active);
    if (options[i].mayReject)
      contracts::nextState(behaviour, "rejected_option" + std::to_string(i) + "_error",
                           allOf({sel, equal(options[i].triggerReply, constants::resultFailure())}), error);
    if (options[i].mayEmitSuccess)
      contracts::nextState(behaviour, "option" + std::to_string(i) + "_succeeds", allOf({active, equal(options[i].outcome, constants::eventSuccess())}), idle);
    if (options[i].mayEmitFailure)
      contracts::nextState(behaviour, "option" + std::to_string(i) + "_fails", allOf({active, equal(options[i].outcome, constants::eventFailure())}), error);
    if (options[i].mayAccept)
    {
      contracts::nextState(behaviour, "accepted_abort_option" + std::to_string(i) + "_idle",
                           allOf({active, abort, equal(options[i].abortReply, constants::resultSuccess())}), idle);
      contracts::nextState(behaviour, "failed_abort_option" + std::to_string(i) + "_error",
                           allOf({active, abort, equal(options[i].abortReply, constants::resultFailure())}), error);
    }
  }
  contracts::nextState(behaviour, "failed_reset_preserves_error", allOf({error, reset, equal(behaviour.resetReply, constants::resultFailure())}), error);
  for (auto& option : options)
    mergeBehaviour(behaviour, std::move(option));

  return VoidResult();
}

VoidResult SemanticCompiler::compileFlowCall(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                                             const PExpression& reset, Behaviour& behaviour)
{
  const ir::Flow* target = nullptr;
  for (const auto& component : mProgram.components)
  {
    for (const auto& flow : component.flows)
    {
      if (flow.symbol != call.target)
        continue;

      target = &flow;
      break;
    }

    if (target)
      break;
  }

  if (!target)
    return VoidResult::Failed("nuXmv: unresolved flow call target '{}'", sourceName(call.target));

  if (!target->strategy)
    return VoidResult::Failed("nuXmv: flow '{}' has no strategy", target->name);

  auto flowResult = findFlow(call.target);
  RETURN_ON_FAILURE(flowResult);
  auto& flow = *flowResult.Value();

  // A flow can be aborted either by its normal parent or by an explicit
  // flow.abort() call elsewhere in the task.
  const auto externallyAbortable = mExternallyAbortedFlows.contains(call.target);
  const auto effectiveAbort = externallyAbortable ? logicalOr(abort, reference(flow.id + "_abort_request")) : abort;

  // Semantic inlining: recursively compile the target into the caller's flat
  // model. The external flow-abort signal reaches the root behaviour directly.
  RETURN_ON_FAILURE(compileStrategy(target->strategy, trigger, effectiveAbort, reset, behaviour));

  if (behaviour.id.empty())
  {
    const auto [id, _] = uniqueId(strategy, target->name);
    behaviour.id = id;
  }

  // Materialize the target flow interface before registering observations.
  // This prevents FlowCallSite from retaining large expression trees.
  //
  // Use a separate suffix because the parent will subsequently materialize
  // the outer call interface, possibly after applying call-site handlers.
  materializeBehaviourInterface(behaviour, "_flowcall");

  flow.calls.push_back(FlowCallSite{
    .id = behaviour.id,
    .trigger = trigger,
    .triggerReply = behaviour.triggerReply,
    .outcome = behaviour.outcome,
    .abort = effectiveAbort,
    .abortReply = behaviour.abortReply,
    .running = runningExpression(behaviour),
  });

  flow.abortReplies.push_back(behaviour.abortReply);

  return VoidResult();
}

VoidResult SemanticCompiler::compileFlowAbort(const ir::PStrategy& strategy, const ir::Call& call, const PExpression& trigger, const PExpression& abort,
                                              const PExpression& reset, Behaviour& behaviour)
{
  const auto [id, _] = uniqueId(strategy, "flow_abort");
  auto flowResult = findFlow(call.receiver);
  RETURN_ON_FAILURE(flowResult);
  auto& flow = *flowResult.Value();

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;
  behaviour.error = boolean(false);

  // Triggering this call raises the shared abort request of the target flow.
  flow.abortRequests.push_back(trigger);

  const auto abortReply = reference(flow.id + "_abort_reply");

  // Same call-level abstraction as AbortCall:
  //   successful abort -> synchronous Done
  //   failed abort     -> Failure
  //   running          -> Running
  behaviour.triggerReply = cases(
    {
      {equal(abortReply, constants::resultFailure()), constants::resultFailure()},
      // {equal(abortReply, constants::resultRunning()), constants::resultRunning()},
      {equal(abortReply, constants::resultSuccess()), constants::resultDone()},
    },
    constants::resultNone());

  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();
  behaviour.mayReturnDone = true;  // An external flow abort may finish immediately.
  behaviour.mayAccept = false;
  behaviour.mayReject = true;
  behaviour.mayEmitSuccess = false;
  behaviour.mayEmitFailure = false;
  behaviour.mayEmitAborted = false;

  return {};
}

VoidResult SemanticCompiler::applyStrategyHandlers(const std::string& ownerId, const std::vector<ir::PHandler>& handlers, Behaviour& behaviour)
{
  return applyHandlers(
    ownerId, handlers,
    [this](const ir::PStrategy& body, const PExpression& trigger, const PExpression& abort, const PExpression& reset, Behaviour& compiled) {
      RETURN_ON_FAILURE(compileStrategy(body, trigger, abort, reset, compiled));
      materializeBehaviourInterface(compiled);
      return VoidResult{};
    },
    behaviour);
}

VoidResult SemanticCompiler::compileProperties(const ir::Component& component, Model& model)
{
  for (const auto& property : component.properties)
  {
    auto expression = compilePropertyExpr(property.expression);
    RETURN_ON_FAILURE(expression);

    model.addProperty({
      .name = sanitize(property.name),
      .expression = expression.Value(),
      .origin =
        {
          .sourceSymbol = component.symbol,
          .sourceSpan = property.span,
        },
    });
  }

  return VoidResult();
}

PExpression SemanticCompiler::flowRisingExpression(const FlowCallSite& call) const
{
  // Started: externally Idle -> active. Internal active-state changes do not
  // count as another start.
  return logicalAnd(equal(reference(call.id + "_state"), reference("Idle")), unary("X", call.running));
}

PExpression SemanticCompiler::flowFallingExpression(const FlowCallSite& call) const
{
  // Stopped is outcome-agnostic: any active -> non-active transition.
  return logicalAnd(call.running, unary("X", logicalNot(call.running)));
}

PExpression SemanticCompiler::capabilityRunningExpression(const CapabilityInstance& capability, const CapabilityCallSite* call) const
{
  // Sync capabilities are never "running"
  if (capability.kind == ir::CapabilityKind::Sync)
    return boolean(false);

  const auto running = equal(reference(capability.id + "_state"), reference("Running"));

  if (!call)
    return running;

  return logicalAnd(running, equal(reference(capability.id + "_owner"), reference(call->id)));
}

PExpression SemanticCompiler::capabilityRisingExpression(const CapabilityInstance& capability, const CapabilityCallSite* call) const
{
  return logicalAnd(equal(reference(capability.id + "_state"), reference("Idle")), unary("X", capabilityRunningExpression(capability, call)));
}

PExpression SemanticCompiler::capabilityFallingExpression(const CapabilityInstance& capability, const CapabilityCallSite* call) const
{
  const auto running = capabilityRunningExpression(capability, call);
  return logicalAnd(running, unary("X", logicalNot(capabilityRunningExpression(capability, call))));
}

PExpression SemanticCompiler::runningExpression(const Behaviour& behaviour) const
{
  const auto stateName = behaviour.id + "_state";
  const auto hasState = std::ranges::any_of(behaviour.variables, [&stateName](const Variable& variable) {
    return variable.kind == VariableKind::State && variable.name == stateName;
  });

  if (!hasState)
    return boolean(false);

  const auto state = reference(stateName);
  return allOf({
    notEqual(state, reference("Idle")),
    notEqual(state, reference("Error")),
  });
}

Result<PExpression> SemanticCompiler::compilePropertyExpr(const ir::PPropertyExpr& expr)
{
  if (!expr)
    return Result<PExpression>::Failed("nuXmv: invalid property expression");

  if (const auto* observation = std::get_if<ir::Observation>(&expr->value))
    return compileObservation(*observation);

  if (const auto* op = std::get_if<ir::PropertyExpr::Not>(&expr->value))
  {
    auto operand = compilePropertyExpr(op->operand);
    RETURN_ON_FAILURE(operand);

    return logicalNot(operand.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::And>(&expr->value))
  {
    auto lhs = compilePropertyExpr(op->lhs);
    RETURN_ON_FAILURE(lhs);

    auto rhs = compilePropertyExpr(op->rhs);
    RETURN_ON_FAILURE(rhs);

    return logicalAnd(lhs.Value(), rhs.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Or>(&expr->value))
  {
    auto lhs = compilePropertyExpr(op->lhs);
    RETURN_ON_FAILURE(lhs);

    auto rhs = compilePropertyExpr(op->rhs);
    RETURN_ON_FAILURE(rhs);

    return logicalOr(lhs.Value(), rhs.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Implies>(&expr->value))
  {
    auto lhs = compilePropertyExpr(op->lhs);
    RETURN_ON_FAILURE(lhs);

    auto rhs = compilePropertyExpr(op->rhs);
    RETURN_ON_FAILURE(rhs);

    return binary(lhs.Value(), "->", rhs.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Always>(&expr->value))
  {
    auto operand = compilePropertyExpr(op->operand);
    RETURN_ON_FAILURE(operand);

    return unary("G", operand.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Eventually>(&expr->value))
  {
    auto operand = compilePropertyExpr(op->operand);
    RETURN_ON_FAILURE(operand);
    return unary("F", operand.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Next>(&expr->value))
  {
    auto operand = compilePropertyExpr(op->operand);
    RETURN_ON_FAILURE(operand);

    return unary("X", operand.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::Until>(&expr->value))
  {
    auto lhs = compilePropertyExpr(op->lhs);
    RETURN_ON_FAILURE(lhs);

    auto rhs = compilePropertyExpr(op->rhs);
    RETURN_ON_FAILURE(rhs);

    return binary(lhs.Value(), "U", rhs.Value());
  }
  else if (const auto* op = std::get_if<ir::PropertyExpr::WeakUntil>(&expr->value))
  {
    auto lhs = compilePropertyExpr(op->lhs);
    RETURN_ON_FAILURE(lhs);

    auto rhs = compilePropertyExpr(op->rhs);
    RETURN_ON_FAILURE(rhs);

    // P W Q == (P U Q) | G P
    return logicalOr(binary(lhs.Value(), "U", rhs.Value()), unary("G", lhs.Value()));
  }

  return Result<PExpression>::Failed("nuXmv: unsupported property expression");
}

Result<PExpression> SemanticCompiler::compileObservation(const ir::Observation& observation)
{
  const auto* receiver = mSymbols.get(observation.receiver);
  if (!receiver)
    return Result<PExpression>::Failed("nuXmv: unresolved property observation");

  if (receiver->kind == SymbolKind::Flow)
  {
    const auto it = mFlowInstances.find(observation.receiver);
    if (it == mFlowInstances.end())
      return Result<PExpression>::Failed("nuXmv: flow '{}' is not instantiated", receiver->name);

    const auto& flow = it->second;

    switch (observation.kind)
    {
      case ir::Observation::Kind::Started:
      {
        return reference(flow.id + "_started");
      }
      case ir::Observation::Kind::Rejected:
      {
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(logicalAnd(call.trigger, equal(call.triggerReply, constants::resultFailure())));

        return anyOf(expressions);
      }
      case ir::Observation::Kind::Stopped:
      {
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(flowFallingExpression(call));

        return anyOf(expressions);
      }
      case ir::Observation::Kind::Aborted:
      {
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(logicalAnd(call.abort, equal(call.abortReply, constants::resultSuccess())));

        return anyOf(expressions);
      }
      case ir::Observation::Kind::Succeeded:
      {
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(anyOf({
            allOf({call.trigger, equal(call.triggerReply, constants::resultDone())}),
            equal(call.outcome, constants::eventSuccess()),
          }));

        return anyOf(expressions);
      }
      case ir::Observation::Kind::Failed:
      {
        return reference(flow.id + "_failed");
      }
      case ir::Observation::Kind::Running:
      {
        return reference(flow.id + "_running");
      }
    }
  }

  const auto capability = mCapabilities.find(observation.receiver);
  if (capability == mCapabilities.end())
    return Result<PExpression>::Failed("nuXmv: capability '{}' is not used in this flow", receiver->name);

  const auto& instance = capability->second;
  const auto calls = findCallSites(observation.receiver, observation.target);

  // All calls to a capability share the same state machine and trigger-result
  // input. Factor these common terms instead of emitting one CASE expression
  // per invocation site in each observation.
  //
  // For asynchronous capabilities, an invocation can be accepted either from
  // Idle or during same-macrostep handover: the previous invocation is still
  // Running in the pre-state, but emits EVENT_SUCCESS in this transition.
  // Observations must use the same availability semantics as compileAction().
  std::vector<PExpression> matchingTriggers;
  std::vector<PExpression> matchingOwners;
  matchingTriggers.reserve(calls.size());
  matchingOwners.reserve(calls.size());
  for (const auto* call : calls)
  {
    matchingTriggers.push_back(call->trigger);
    if (instance.kind == ir::CapabilityKind::Async)
      matchingOwners.push_back(equal(reference(instance.id + "_owner"), reference(call->id)));
  }

  const auto anyMatchingTrigger = anyOf(matchingTriggers);
  const auto anyMatchingOwner = anyOf(matchingOwners);
  const auto idle = equal(reference(instance.id + "_state"), reference("Idle"));
  const auto triggerResult = reference(instance.id + "_trigger_result");
  // Synchronous capabilities have no _event. Async availability is shared
  // with compileAction() and the materialized capability state machine.
  const auto available = instance.kind == ir::CapabilityKind::Async ? reference(instance.id + "_available") : idle;

  // Used only for asynchronous capabilities: synchronous capabilities have
  // neither a Running state nor an _event or _owner variable.
  const auto matchingAsyncEvent = [&](const PExpression& eventKind) {
    return allOf({
      equal(reference(instance.id + "_state"), reference("Running")),
      anyMatchingOwner,
      equal(reference(instance.id + "_event"), eventKind),
    });
  };

  switch (observation.kind)
  {
    case ir::Observation::Kind::Running:
    {
      if (observation.target == InvalidSymbol)
        return capabilityRunningExpression(instance);

      std::vector<PExpression> expressions;
      for (const auto* call : calls)
        expressions.push_back(capabilityRunningExpression(instance, call));
      return anyOf(expressions);
    }
    case ir::Observation::Kind::Started:
    {
      const auto accepted = instance.kind == ir::CapabilityKind::Sync ? constants::resultDone() : constants::resultSuccess();
      return allOf({available, anyMatchingTrigger, equal(triggerResult, accepted)});
    }
    case ir::Observation::Kind::Rejected:
    {
      return allOf({available, anyMatchingTrigger, equal(triggerResult, constants::resultFailure())});
    }
    case ir::Observation::Kind::Stopped:
    {
      std::vector<PExpression> expressions;
      for (const auto* call : calls)
        expressions.push_back(capabilityFallingExpression(instance, call));
      return anyOf(expressions);
    }
    case ir::Observation::Kind::Aborted:
    {
      std::vector<PExpression> expressions;
      for (const auto* call : calls)
        expressions.push_back(logicalAnd(call->abort, equal(call->abortReply, constants::resultSuccess())));
      return anyOf(expressions);
    }
    case ir::Observation::Kind::Succeeded:
    {
      const auto synchronousDone = allOf({available, anyMatchingTrigger, equal(triggerResult, constants::resultDone())});
      if (instance.kind != ir::CapabilityKind::Async)
        return synchronousDone;

      return logicalOr(synchronousDone, matchingAsyncEvent(constants::eventSuccess()));
    }
    case ir::Observation::Kind::Failed:
    {
      const auto synchronouslyRejected = allOf({available, anyMatchingTrigger, equal(triggerResult, constants::resultFailure())});
      if (instance.kind != ir::CapabilityKind::Async)
        return synchronouslyRejected;

      return logicalOr(synchronouslyRejected, matchingAsyncEvent(constants::eventFailure()));
    }
  }

  return Result<PExpression>::Failed("nuXmv: unsupported property observation");
}

std::vector<const SemanticCompiler::CapabilityCallSite*> SemanticCompiler::findCallSites(SymbolId receiver, SymbolId target) const
{
  std::vector<const CapabilityCallSite*> result;
  const auto it = mCapabilities.find(receiver);
  if (it == mCapabilities.end())
    return result;

  for (const auto& call : it->second.calls)
    if (target == InvalidSymbol || call.target == target)
      result.push_back(&call);

  return result;
}

void SemanticCompiler::indexEvents()
{
  mEventKinds.clear();
  mExternallyAbortedFlows.clear();
  for (const auto& component : mProgram.components)
  {
    for (const auto& action : component.actions)
      for (const auto& event : action.events)
        mEventKinds[event.symbol] = event.kind;

    for (const auto& flow : component.flows)
    {
      auto events = mSymbols.children(flow.symbol, koda::SymbolKind::Event);
      if (auto event = events.size() == 1 ? mSymbols.get(events.front()) : nullptr; event)
        mEventKinds[event->id] = ir::EventKind::Abort;

      indexFlowAborts(flow.strategy);
    }
  }
}

void SemanticCompiler::indexFlowAborts(const ir::PStrategy& strategy)
{
  if (!strategy)
    return;

  if (auto sequence = std::get_if<ir::Strategy::Sequence>(&strategy->value); sequence)
  {
    for (const auto& item : sequence->items)
      indexFlowAborts(item);
  }
  else if (auto join = std::get_if<ir::Strategy::Join>(&strategy->value); join)
  {
    for (const auto& item : join->items)
      indexFlowAborts(item);
  }
  else if (auto either = std::get_if<ir::Strategy::Either>(&strategy->value); either)
  {
    for (const auto& item : either->items)
      indexFlowAborts(item);
  }
  else if (auto within = std::get_if<ir::Strategy::Within>(&strategy->value); within)
  {
    indexFlowAborts(within->body);
    indexFlowAborts(within->fallback);
  }
  else if (auto repeat = std::get_if<ir::Strategy::Repeat>(&strategy->value); repeat)
  {
    indexFlowAborts(repeat->body);
    for (const auto& handler : repeat->handlers)
      if (handler)
        indexFlowAborts(handler->body);
  }
  else if (auto retry = std::get_if<ir::Strategy::Retry>(&strategy->value); retry)
  {
    indexFlowAborts(retry->body);
    for (const auto& handler : retry->handlers)
      if (handler)
        indexFlowAborts(handler->body);
  }
  else if (auto call = std::get_if<ir::Strategy::Call>(&strategy->value); call)
  {
    const auto receiver = mSymbols.get(call->call.receiver);
    if (receiver && receiver->kind == SymbolKind::Flow)
    {
      LOG_DEBUG("Found call: {} {} {}", receiver->id, (int)receiver->kind, receiver->name);
      const auto target = mSymbols.get(call->call.target);
      if (target && target->kind == SymbolKind::Event && target->type.toString() == "Abort")
        mExternallyAbortedFlows.insert(call->call.receiver);
    }

    for (const auto& handler : call->handlers)
      if (handler)
        indexFlowAborts(handler->body);
  }
  else if (auto choose = std::get_if<ir::Strategy::Choose>(&strategy->value); choose)
  {
    for (const auto& option : choose->options)
      indexFlowAborts(option.strategy);
  }
}

Result<SemanticCompiler::CapabilityInstance*> SemanticCompiler::findCapability(SymbolId symbol)
{
  if (const auto it = mCapabilities.find(symbol); it != mCapabilities.end())
    return &it->second;

  const auto* info = mSymbols.get(symbol);
  if (!info || info->kind != SymbolKind::Argument)
    return Result<SemanticCompiler::CapabilityInstance*>::Failed("Could not find capability with symbol: {}", symbol);

  const auto cId = mSymbols.component(info->type.toString());
  if (!cId)
    return Result<SemanticCompiler::CapabilityInstance*>::Failed("No component of type: {}", info->type.toString());

  const auto* cap = mSymbols.get(cId.value());
  if (!cap)
    return Result<SemanticCompiler::CapabilityInstance*>::Failed("No symbol for type: {}", info->type.toString());

  ir::CapabilityKind kind = ir::CapabilityKind::Unknown;
  for (const auto& component : mProgram.components)
  {
    if (component.symbol == cap->id)
    {
      kind = component.capabilityKind;
      break;
    }
  }

  CapabilityInstance instance{
    .symbol = symbol,
    .id = sanitize(info ? info->name : "capability"),
    .kind = kind,
    .calls = {},
  };

  auto [it, _] = mCapabilities.emplace(symbol, std::move(instance));
  return &it->second;
}

Result<SemanticCompiler::FlowInstance*> SemanticCompiler::findFlow(SymbolId symbol)
{
  if (const auto it = mFlowInstances.find(symbol); it != mFlowInstances.end())
    return &it->second;

  FlowInstance instance{
    .symbol = symbol,
    .id = sanitize(sourceName(symbol)),
    .calls = {},
    .abortRequests = {},
    .abortReplies = {},
  };

  auto [it, _] = mFlowInstances.emplace(symbol, std::move(instance));
  return &it->second;
}

void SemanticCompiler::materializeCapabilities(Behaviour& behaviour)
{
  for (const auto& [symbol, capability] : mCapabilities)
  {
    if (capability.calls.empty())
      continue;

    switch (capability.kind)
    {
      case ir::CapabilityKind::Sync:
        materializeSyncCapability(capability, behaviour);
        break;

      case ir::CapabilityKind::Async:
        materializeAsyncCapability(capability, behaviour);
        break;

      case ir::CapabilityKind::Unknown:
      default:
        LOG_ERROR("Cannot materialize capability '{}': capability kind is unknown", capability.id);
        break;
    }
  }
}

void SemanticCompiler::materializeSyncCapability(const CapabilityInstance& capability, Behaviour& behaviour)
{
  const auto& id = capability.id;
  const auto state = reference(id + "_state");
  const auto triggerResult = reference(id + "_trigger_result");
  const auto resetResult = reference(id + "_reset_result");

  auto origin = capability.calls.front().origin;
  origin.sourceSymbol = capability.symbol;

  // A synchronous capability either completes in the invocation macrostep or
  // rejects it. It can therefore never become Running and has no asynchronous
  // owner/event/abort machinery.
  behaviour.variables.push_back({VariableKind::Input, id + "_trigger_result", "{0, 2, 5}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_reset_result", "{0, 1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Error}", reference("Idle"), origin});

  std::vector<PExpression> triggers;
  std::vector<PExpression> resets;
  std::vector<GuardedValue> stateCases;

  for (const auto& call : capability.calls)
  {
    triggers.push_back(call.trigger);
    resets.push_back(call.reset);

    // RESULT_DONE leaves the capability in Idle (the fallback below). Only a
    // rejected synchronous invocation enters Error.
    stateCases.push_back({and3(equal(state, reference("Idle")), call.trigger, equal(triggerResult, constants::resultFailure())), reference("Error")});
  }

  const auto anyTrigger = anyOf(triggers);
  const auto anyReset = anyOf(resets);
  const auto noCommand = logicalNot(logicalOr(anyTrigger, anyReset));

  behaviour.constraints.push_back({
    allOf({
      binary(logicalNot(anyTrigger), "->", equal(triggerResult, constants::resultNone())),
      binary(anyTrigger, "->", notEqual(triggerResult, constants::resultNone())),
      binary(logicalNot(anyReset), "->", equal(resetResult, constants::resultNone())),
      binary(anyReset, "->", notEqual(resetResult, constants::resultNone())),
    }),
    origin,
  });
  stateCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("Idle")});

  behaviour.nextAssignments.push_back({id + "_state", std::move(stateCases), state, origin});

  // A capability instance is exclusive even when calls complete synchronously.
  // Use a linear-size, semantically equivalent cardinality encoding.
  constrainAtMostOne(behaviour, id + "_triggers", triggers, origin);
  constrainAtMostOne(behaviour, id + "_resets", resets, origin);

  behaviour.constraints.push_back({
    allOf({
      binary(equal(state, reference("Idle")), "->", logicalOr(noCommand, anyTrigger)),
      binary(equal(state, reference("Error")), "->", logicalOr(noCommand, anyReset)),
    }),
    origin,
  });
}

void SemanticCompiler::materializeAsyncCapability(const CapabilityInstance& capability, Behaviour& behaviour)
{
  const auto& id = capability.id;
  const auto state = reference(id + "_state");
  const auto owner = reference(id + "_owner");
  const auto triggerResult = reference(id + "_trigger_result");
  const auto abortResult = reference(id + "_abort_result");
  const auto resetResult = reference(id + "_reset_result");
  const auto event = reference(id + "_event");

  auto origin = capability.calls.front().origin;
  origin.sourceSymbol = capability.symbol;

  std::string ownerDomain = "{None";
  for (const auto& call : capability.calls)
    ownerDomain += ", " + call.id;
  ownerDomain += "}";

  // Async invocation is either accepted (SUCCESS -> Running) or rejected.
  // RESULT_DONE is deliberately excluded: completion must arrive as an event.
  behaviour.variables.push_back({VariableKind::Input, id + "_trigger_result", "{0, 1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_abort_result", "{0, 1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_reset_result", "{0, 1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_event", "0..2", {}, origin});
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Error}", reference("Idle"), origin});
  behaviour.variables.push_back({VariableKind::State, id + "_owner", ownerDomain, reference("None"), origin});

  std::vector<PExpression> triggers;
  std::vector<PExpression> resets;
  std::vector<PExpression> ownedAborts;
  std::vector<PExpression> existingOwnerAborts;
  std::vector<PExpression> explicitAborts;

  // Implicit abort requests from active action invocations.
  for (const auto& call : capability.calls)
  {
    triggers.push_back(call.trigger);
    resets.push_back(call.reset);

    const auto currentlyOwned = allOf({equal(state, reference("Running")), equal(owner, reference(call.id)), call.abort});
    const auto acceptedNow = allOf({reference(id + "_available"), call.trigger, equal(triggerResult, constants::resultSuccess())});
    existingOwnerAborts.push_back(currentlyOwned);
    ownedAborts.push_back(logicalOr(currentlyOwned, logicalAnd(acceptedNow, call.abort)));
  }

  // Explicit abort requests, e.g. camera.abort_initialization().
  for (const auto& abortSite : capability.abortCalls)
  {
    std::vector<PExpression> matchingOwners;

    // Find the action invocation targeted by this abort.
    for (const auto& call : capability.calls)
      if (call.target == abortSite.targetTrigger)
        matchingOwners.push_back(equal(owner, reference(call.id)));

    // The target must be the currently running action.
    const auto targetActive = allOf({equal(state, reference("Running")), anyOf(matchingOwners)});

    // Used by compileAbortCall() to determine its reply.
    behaviour.definitions.push_back({abortSite.id + "_target_active", targetActive, abortSite.origin});

    // An explicit abort becomes a real cancellation request
    // only when the targeted action is running.
    explicitAborts.push_back(logicalAnd(abortSite.invoked, targetActive));
  }

  const auto idle = equal(state, reference("Idle"));
  const auto running = equal(state, reference("Running"));

  // These guards are referenced in several next-state branches and constraints.
  // Keep one named expression per capability rather than serializing the entire
  // call-site tree at every use (especially expensive for shared recovery flows).
  const auto shared = [&](const std::string& suffix, PExpression expression) -> PExpression {
    const auto name = id + "_" + suffix;
    behaviour.definitions.push_back({name, std::move(expression), origin});
    return reference(name);
  };

  const auto anyTrigger = shared("any_trigger", anyOf(triggers));
  const auto anyReset = shared("any_reset", anyOf(resets));
  const auto releasedNow = shared("released_now", allOf({running, equal(event, constants::eventSuccess())}));
  const auto available = shared("available", logicalOr(idle, releasedNow));
  const auto handover = shared("handover", logicalAnd(running, anyTrigger));

  // Both abort mechanisms use the same capability interface.
  const auto anyAbort = shared("any_abort", logicalOr(anyOf(ownedAborts), anyOf(explicitAborts)));
  const auto priorOwnerAbort = shared("prior_owner_abort", logicalOr(anyOf(existingOwnerAborts), anyOf(explicitAborts)));
  const auto noCommand = shared("no_command", logicalNot(anyOf({anyTrigger, anyAbort, anyReset})));

  behaviour.constraints.push_back({
    allOf({
      binary(logicalNot(anyTrigger), "->", equal(triggerResult, constants::resultNone())),
      binary(anyTrigger, "->", notEqual(triggerResult, constants::resultNone())),
      binary(logicalNot(anyAbort), "->", equal(abortResult, constants::resultNone())),
      binary(anyAbort, "->", notEqual(abortResult, constants::resultNone())),
      binary(logicalNot(anyReset), "->", equal(resetResult, constants::resultNone())),
      binary(anyReset, "->", notEqual(resetResult, constants::resultNone())),
    }),
    origin,
  });

  // A transition may finish one invocation and start another invocation of
  // the same capability. Both operations belong to one atomic KODA macrostep.
  // A just-accepted invocation may also be cancelled immediately (Join
  // synchronous startup failure); its intermediate Running state is virtual.
  std::vector<GuardedValue> stateCases;
  std::vector<GuardedValue> ownerCases;
  for (const auto& call : capability.calls)
  {
    const auto accepted = allOf({available, call.trigger, equal(triggerResult, constants::resultSuccess())});
    const auto rejected = allOf({available, call.trigger, equal(triggerResult, constants::resultFailure())});
    const auto cancelled = logicalAnd(accepted, call.abort);
    stateCases.push_back({allOf({cancelled, equal(abortResult, constants::resultSuccess())}), reference("Idle")});
    stateCases.push_back({allOf({cancelled, equal(abortResult, constants::resultFailure())}), reference("Error")});
    stateCases.push_back({logicalAnd(accepted, logicalNot(call.abort)), reference("Running")});
    stateCases.push_back({rejected, reference("Error")});
    ownerCases.push_back({logicalAnd(accepted, logicalNot(call.abort)), reference(call.id)});
    ownerCases.push_back({logicalOr(rejected, cancelled), reference("None")});
  }

  stateCases.push_back({allOf({running, noCommand, equal(event, constants::eventSuccess())}), reference("Idle")});
  stateCases.push_back({allOf({running, noCommand, equal(event, constants::eventFailure())}), reference("Error")});
  stateCases.push_back({allOf({running, priorOwnerAbort, equal(abortResult, constants::resultSuccess())}), reference("Idle")});
  stateCases.push_back({allOf({running, priorOwnerAbort, equal(abortResult, constants::resultFailure())}), reference("Error")});
  stateCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("Idle")});
  behaviour.nextAssignments.push_back({id + "_state", std::move(stateCases), state, origin});

  ownerCases.push_back({allOf({running, noCommand, equal(event, constants::eventSuccess())}), reference("None")});
  ownerCases.push_back({allOf({running, noCommand, equal(event, constants::eventFailure())}), reference("None")});
  ownerCases.push_back({allOf({running, priorOwnerAbort}), reference("None")});
  ownerCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("None")});
  behaviour.nextAssignments.push_back({id + "_owner", std::move(ownerCases), owner, origin});

  constrainAtMostOne(behaviour, id + "_triggers", triggers, origin);
  constrainAtMostOne(behaviour, id + "_resets", resets, origin);

  behaviour.constraints.push_back({
    allOf({
      binary(idle, "->", logicalOr(noCommand, anyTrigger)),
      binary(running, "->", anyOf({noCommand, anyAbort, handover})),
      binary(equal(state, reference("Error")), "->", logicalOr(noCommand, anyReset)),
      binary(logicalNot(running), "->", equal(event, constants::eventNone())),
      // A running capability may accept a new invocation in the very same
      // transition only if its previous invocation succeeds now. Do not allow
      // an unrelated cancellation of the previous owner in that transition.
      binary(handover, "->", allOf({releasedNow, logicalNot(priorOwnerAbort), logicalNot(anyReset)})),
      binary(allOf({running, logicalNot(noCommand), logicalNot(handover)}), "->", equal(event, constants::eventNone())),
      binary(logicalNot(anyAbort), "->", equal(abortResult, constants::resultNone())),
      binary(anyAbort, "->", notEqual(abortResult, constants::resultNone())),
    }),
    origin,
  });
}

void SemanticCompiler::materializeFlows(Behaviour& behaviour)
{
  for (const auto& [symbol, flow] : mFlowInstances)
  {
    Provenance origin;
    origin.sourceSymbol = symbol;

    if (mExternallyAbortedFlows.contains(symbol))
    {
      const auto abortRequest = flow.abortRequests.empty() ? boolean(false) : anyOf(flow.abortRequests);

      behaviour.definitions.push_back({flow.id + "_abort_request", abortRequest, origin});

      std::vector<Expression::Case::Branch> replyCases;
      for (const auto& reply : flow.abortReplies)
        replyCases.push_back({notEqual(reply, constants::resultNone()), reply});

      behaviour.definitions.push_back({flow.id + "_abort_reply", cases(std::move(replyCases), constants::resultNone()), origin});
    }

    // Materialize flow observations once, instead of embedding
    // their full expression trees in every LTL property.
    std::vector<PExpression> startedCalls;
    std::vector<PExpression> failedCalls;

    for (const auto& call : flow.calls)
    {
      startedCalls.push_back(allOf({
        call.trigger,
        anyOf({
          equal(call.triggerReply, constants::resultSuccess()),
          equal(call.triggerReply, constants::resultDone()),
        }),
      }));

      failedCalls.push_back(anyOf({
        allOf({
          call.trigger,
          equal(call.triggerReply, constants::resultFailure()),
        }),
        equal(call.outcome, constants::eventFailure()),
      }));
    }

    behaviour.definitions.push_back({
      flow.id + "_started",
      anyOf(startedCalls),
      origin,
    });

    behaviour.definitions.push_back({
      flow.id + "_failed",
      anyOf(failedCalls),
      origin,
    });

    // Running observation
    std::vector<PExpression> runningCalls;
    for (const auto& call : flow.calls)
      runningCalls.push_back(call.running);

    behaviour.definitions.push_back({
      flow.id + "_running",
      runningCalls.empty() ? boolean(false) : anyOf(runningCalls),
      origin,
    });
  }
}

bool SemanticCompiler::isFlowAbort(const ir::Call& call) const
{
  if (call.receiver == InvalidSymbol)
    return false;

  const auto* receiver = mSymbols.get(call.receiver);
  if (!receiver || receiver->kind != SymbolKind::Flow)
    return false;

  const auto it = mEventKinds.find(call.target);
  return it != mEventKinds.end() && it->second == ir::EventKind::Abort;
}

SemanticCompiler::LeafCallKind SemanticCompiler::classify(const ir::Call& call) const
{
  if (isFlowAbort(call))
    return LeafCallKind::FlowAbort;

  if (call.kind == ir::CallKind::Flow)
    return LeafCallKind::Flow;

  const auto it = mEventKinds.find(call.target);
  if (it != mEventKinds.end() && it->second == ir::EventKind::Abort)
    return LeafCallKind::AbortCall;

  return LeafCallKind::Action;
}

std::string SemanticCompiler::strategyName(const ir::PStrategy& strategy, const std::string& fallback) const
{
  if (!strategy || strategy->name.empty())
    return sanitize(fallback);

  return sanitize(strategy->name);
}

SemanticCompiler::UniqueId SemanticCompiler::uniqueId(const ir::PStrategy& strategy, const std::string& fallback)
{
  const auto localName = strategyName(strategy, fallback);

  std::string base;
  for (const auto& part : mIdContext)
  {
    if (!base.empty())
      base += "_";

    base += part;
  }

  if (!base.empty())
    base += "_";

  base += localName;
  const auto count = mIdCounters[base]++;
  if (count > 0)
    base += "_" + std::to_string(count);

  return {base, localName};
}

std::string SemanticCompiler::sourceName(SymbolId id) const
{
  const auto* symbol = mSymbols.get(id);
  return symbol ? symbol->name : "unknown";
}

std::string SemanticCompiler::sanitize(std::string value)
{
  for (auto& c : value)
  {
    const auto uc = static_cast<unsigned char>(c);
    if (!std::isalnum(uc) && c != '_')
      c = '_';
    else
      c = static_cast<char>(std::tolower(uc));
  }

  if (!value.empty() && std::isdigit(static_cast<unsigned char>(value.front())))
    value.insert(value.begin(), '_');

  return value;
}

Provenance SemanticCompiler::provenance(const ir::PStrategy& strategy)
{
  if (!strategy)
    return {};
  return {.sourceSymbol = std::nullopt, .sourceSpan = strategy->span};
}

}  // namespace koda::nuxmv
