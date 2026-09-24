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
  registerMainFlow(root, flow.symbol, command);

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

void SemanticCompiler::registerMainFlow(const Behaviour& behaviour, SymbolId flow, const PExpression command)
{
  auto& rootFlow = findFlow(flow);
  rootFlow.calls.push_back(FlowCallSite{
      .id = behaviour.id,
      .trigger = equal(command, constants::commandTrigger()),
      .triggerReply = behaviour.triggerReply,
      .outcome = behaviour.outcome,
      .abort = equal(command, constants::commandAbort()),
      .abortReply = behaviour.abortReply,
      .running = runningExpression(behaviour),
  });
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

  behaviour.variables.push_back({
      .kind = VariableKind::State,
      .name = id + "_state",
      .domain = "{Idle, Error}",
      .initial = reference("Idle"),
      .origin = behaviour.origin,
  });

  behaviour.triggerReply = cases({{is(state, reference("Idle")), constants::resultFailure()}}, constants::resultNone());
  behaviour.abortReply = cases({{is(state, reference("Error")), constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{is(state, reference("Error")), constants::resultSuccess()}}, constants::resultNone());
  behaviour.outcome = constants::eventNone();

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

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;

  const auto capState = reference(capability.id + "_state");
  const auto capOwner = reference(capability.id + "_owner");
  const auto triggerResult = reference(capability.id + "_trigger_result");
  const auto abortResult = reference(capability.id + "_abort_result");
  const auto resetResult = reference(capability.id + "_reset_result");
  const auto event = reference(capability.id + "_event");
  const auto owns = equal(capOwner, reference(id));

  // Replies/outcomes remain call-site specific, while the underlying state,
  // environment result and asynchronous event belong to the shared capability.
  behaviour.triggerReply = cases({{logicalAnd(equal(capState, reference("Idle")), trigger), triggerResult}}, constants::resultNone());
  behaviour.abortReply = cases(
      {
          {logicalAnd(equal(capState, reference("Running")), logicalAnd(owns, abort)), abortResult},
          {logicalAnd(equal(capState, reference("Error")), abort), constants::resultFailure()},
      },
      constants::resultNone());
  behaviour.resetReply = cases({{logicalAnd(equal(capState, reference("Error")), reset), resetResult}}, constants::resultNone());
  behaviour.outcome = cases({{logicalAnd(equal(capState, reference("Running")), owns), event}}, constants::eventNone());

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

VoidResult SemanticCompiler::compileAbortCall(const ir::PStrategy& strategy, const ir::Call&, const PExpression& trigger, const PExpression& abort,
                                              const PExpression& reset, Behaviour& behaviour)
{
  const auto [id, _] = uniqueId(strategy, "abort_call");
  const auto state = ref(id, "state");
  const auto abortResult = ref(id, "abort_result");

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;

  behaviour.variables = {
      {VariableKind::Input, id + "_abort_result", "{1, 2, 3}", {}, behaviour.origin},
      {VariableKind::State, id + "_state", "{Idle, Error}", reference("Idle"), behaviour.origin},
  };

  behaviour.triggerReply = cases(
      {
          {logicalAnd(is(state, reference("Idle")), is(abortResult, constants::resultFailure())), constants::resultFailure()},
          {logicalAnd(is(state, reference("Idle")), is(abortResult, constants::resultRunning())), constants::resultRunning()},
          {logicalAnd(is(state, reference("Idle")), is(abortResult, constants::resultSuccess())), constants::resultDone()},
      },
      constants::resultNone());

  behaviour.abortReply = cases({{is(state, reference("Error")), constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{is(state, reference("Error")), constants::resultSuccess()}}, constants::resultNone());
  behaviour.outcome = constants::eventNone();

  behaviour.nextAssignments.push_back({
      .variable = id + "_state",
      .cases =
          {
              {and3(is(state, reference("Idle")), trigger, is(abortResult, constants::resultFailure())), reference("Error")},
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

  return {};
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

  const auto prefix = behaviour.id + suffix;
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

  // A sequence has one persistent state for each asynchronously active child,
  // plus Idle and Error. Synchronous Done chains never become persistent states.
  std::string domain = "{Idle, Error";
  for (std::size_t i = 0; i < sequence.items.size(); ++i)
    domain += ", Child" + std::to_string(i);
  domain += "}";

  behaviour.variables.push_back({
      .kind = VariableKind::State,
      .name = stateName,
      .domain = domain,
      .initial = reference("Idle"),
      .origin = behaviour.origin,
  });

  // Compile children first with symbolic invocation signals. These are DEFINEs
  // in the final model, so recursively following RESULT_DONE stays within the
  // same nuXmv macrostep.
  std::vector<Behaviour> children(sequence.items.size());
  for (std::size_t i = 0; i < sequence.items.size(); ++i)
  {
    const auto prefix = id + "_child" + std::to_string(i);
    const auto childTrigger = reference(prefix + "_trigger");
    const auto childAbort = reference(prefix + "_abort");
    const auto childReset = reference(prefix + "_reset");

    RETURN_ON_FAILURE(compileStrategy(sequence.items[i], childTrigger, childAbort, childReset, children[i]));
    materializeBehaviourInterface(children[i]);
  }

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
    suffixReply[n] = cases(
        {
            {equal(children[n].triggerReply, constants::resultDone()), suffixReply[n + 1]},
        },
        children[n].triggerReply);
  }

  behaviour.triggerReply = cases({{equal(state, reference("Idle")), suffixReply.front()}}, constants::resultNone());

  // Sequence abort is synchronous at the KODA semantic level. The active
  // child's abort reply is propagated directly; no Dezyne-only Aborting state
  // is introduced here.
  std::vector<Expression::Case::Branch> abortCases;
  for (std::size_t i = 0; i < children.size(); ++i)
    abortCases.push_back({equal(state, reference("Child" + std::to_string(i))), children[i].abortReply});
  abortCases.push_back({equal(state, reference("Error")), constants::resultError()});
  behaviour.abortReply = cases(std::move(abortCases), constants::resultNone());

  // Reset is only meaningful after an error. The current flat reference
  // semantics treat recovery of the composite as a successful reset of its
  // control state. Child-specific reset bookkeeping will be refined when
  // RecoveryHandler is added.
  behaviour.resetReply = cases({{equal(state, reference("Error")), constants::resultSuccess()}}, constants::resultNone());

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
    const auto active = equal(state, reference("Child" + std::to_string(i)));
    const auto childSuccess = equal(children[i].outcome, constants::eventSuccess());
    const auto childFailure = equal(children[i].outcome, constants::eventFailure());
    const auto childAborted = equal(children[i].outcome, constants::eventAborted());

    outcomeCases.push_back({
        logicalAnd(active, childFailure),
        constants::eventFailure(),
    });
    outcomeCases.push_back({
        logicalAnd(active, childAborted),
        constants::eventAborted(),
    });

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
      outcomeCases.push_back({
          allOf({
              active,
              childSuccess,
              equal(continueReply[i], constants::resultDone()),
          }),
          constants::eventSuccess(),
      });
    }
  }
  behaviour.outcome = cases(std::move(outcomeCases), constants::eventNone());

  // Child invocation DEFINEs. Child 0 is triggered by the parent trigger.
  // Later children are triggered either by synchronous Done during startup or
  // by success of the currently active predecessor.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto prefix = id + "_child" + std::to_string(i);

    PExpression childTrigger = boolean(false);
    if (i == 0)
    {
      childTrigger = logicalAnd(equal(state, reference("Idle")), trigger);
    }
    else
    {
      // Synchronous startup path: all preceding children returned Done.
      std::vector<PExpression> startupConditions{
          equal(state, reference("Idle")),
          trigger,
      };
      for (std::size_t j = 0; j < i; ++j)
        startupConditions.push_back(equal(children[j].triggerReply, constants::resultDone()));

      // Async continuation path from the immediate predecessor.
      const auto continuation = allOf({
          equal(state, reference("Child" + std::to_string(i - 1))),
          equal(children[i - 1].outcome, constants::eventSuccess()),
      });

      childTrigger = logicalOr(allOf(startupConditions), continuation);
    }

    behaviour.definitions.push_back({prefix + "_trigger", childTrigger, behaviour.origin});
    behaviour.definitions.push_back({
        prefix + "_abort",
        logicalAnd(equal(state, reference("Child" + std::to_string(i))), abort),
        behaviour.origin,
    });
    behaviour.definitions.push_back({prefix + "_reset", boolean(false), behaviour.origin});
  }

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

    auto running = prefix;
    running.push_back(equal(children[i].triggerReply, constants::resultSuccess()));
    nextCases.push_back({allOf(running), reference("Child" + std::to_string(i))});

    auto failed = prefix;
    failed.push_back(equal(children[i].triggerReply, constants::resultFailure()));
    nextCases.push_back({allOf(failed), reference("Error")});
  }

  // Async child completion followed by synchronous closure through subsequent
  // children.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    const auto active = equal(state, reference("Child" + std::to_string(i)));
    nextCases.push_back({
        logicalAnd(active, equal(children[i].outcome, constants::eventFailure())),
        reference("Error"),
    });
    nextCases.push_back({
        logicalAnd(active, equal(children[i].outcome, constants::eventAborted())),
        reference("Idle"),
    });

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

      auto running = prefix;
      running.push_back(equal(children[j].triggerReply, constants::resultSuccess()));
      nextCases.push_back({allOf(running), reference("Child" + std::to_string(j))});

      auto failed = prefix;
      failed.push_back(equal(children[j].triggerReply, constants::resultFailure()));
      nextCases.push_back({allOf(failed), reference("Error")});
    }

    std::vector<PExpression> allDone{
        active,
        equal(children[i].outcome, constants::eventSuccess()),
    };
    for (std::size_t j = i + 1; j < children.size(); ++j)
      allDone.push_back(equal(children[j].triggerReply, constants::resultDone()));
    nextCases.push_back({allOf(allDone), reference("Idle")});
  }

  // Synchronous abort: successful abort returns to Idle; failure enters Error.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
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
      logicalAnd(equal(state, reference("Error")), reset),
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
  }

  const auto noCommand = logicalNot(logicalOr(trigger, logicalOr(abort, reset)));
  behaviour.constraints.push_back({
      allOf({
          binary(equal(state, reference("Idle")), "->", logicalOr(noCommand, trigger)),
          binary(equal(state, reference("Error")), "->", logicalOr(noCommand, logicalOr(reset, abort))),
      }),
      behaviour.origin,
  });

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

  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Retrigger, Error}", reference("Idle"), behaviour.origin});

  Behaviour child;
  RETURN_ON_FAILURE(compileStrategy(repeat.body, reference(childTriggerName), reference(childAbortName), reference(childResetName), child));

  materializeBehaviourInterface(child);

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

  // The body is triggered either by the initial Repeat trigger or while in
  // the internal Retrigger state. In particular, child.outcome is NOT used
  // directly to trigger the body again.
  behaviour.definitions.push_back({childTriggerName, logicalOr(logicalAnd(idle, trigger), retrigger), behaviour.origin});

  behaviour.definitions.push_back({childAbortName, logicalAnd(running, abort), behaviour.origin});

  behaviour.definitions.push_back({childResetName, boolean(false), behaviour.origin});

  behaviour.triggerReply = cases({{logicalAnd(idle, equal(child.triggerReply, constants::resultSuccess())), constants::resultSuccess()},
                                  {logicalAnd(idle, equal(child.triggerReply, constants::resultFailure())), constants::resultFailure()},
                                  {logicalAnd(idle, equal(child.triggerReply, constants::resultDone())), constants::resultDone()}},
                                 constants::resultNone());

  behaviour.abortReply = cases({{running, child.abortReply}, {error, constants::resultError()}}, constants::resultNone());

  behaviour.resetReply = cases({{error, constants::resultSuccess()}}, constants::resultNone());

  behaviour.outcome = cases(
      {
          // Failure of an asynchronously running iteration.
          {logicalAnd(running, failure), constants::eventFailure()},

          // Result of starting the next iteration.
          {logicalAnd(retrigger, equal(child.triggerReply, constants::resultFailure())), constants::eventFailure()},

          {logicalAnd(retrigger, equal(child.triggerReply, constants::resultDone())), constants::eventSuccess()},
      },
      constants::eventNone());

  behaviour.nextAssignments.push_back({id + "_state",
                                       {
                                           // Initial invocation.
                                           {allOf({idle, trigger, equal(child.triggerReply, constants::resultSuccess())}), reference("Running")},

                                           {allOf({idle, trigger, equal(child.triggerReply, constants::resultFailure())}), reference("Error")},

                                           // Async completion schedules another iteration.
                                           // Do NOT retrigger combinationally here.
                                           {logicalAnd(running, success), reference("Retrigger")},

                                           {logicalAnd(running, failure), reference("Error")},

                                           // Execute the scheduled retrigger.
                                           {logicalAnd(retrigger, equal(child.triggerReply, constants::resultSuccess())), reference("Running")},

                                           {logicalAnd(retrigger, equal(child.triggerReply, constants::resultFailure())), reference("Error")},

                                           {logicalAnd(retrigger, equal(child.triggerReply, constants::resultDone())), reference("Idle")},

                                           // External abort while the body is running.
                                           {allOf({running, abort, equal(child.abortReply, constants::resultSuccess())}), reference("Idle")},

                                           {allOf({running, abort, equal(child.abortReply, constants::resultFailure())}), reference("Error")},

                                           // Recovery from Error.
                                           {logicalAnd(error, reset), reference("Idle")},
                                       },
                                       state,
                                       behaviour.origin});

  mergeBehaviour(behaviour, std::move(child));

  if (!repeat.handlers.empty())
    materializeBehaviourInterface(behaviour, "_core");

  return applyStrategyHandlers(id, repeat.handlers, behaviour);
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

  const auto idle = equal(state, reference("Idle"));
  const auto running = equal(state, reference("Running"));
  const auto aborting = equal(state, reference("Aborting"));
  const auto error = equal(state, reference("Error"));

  // -------------------------------------------------------------------------
  // Child control
  // -------------------------------------------------------------------------

  // Ordered synchronous startup.
  //
  // Child i starts only if every preceding child did not synchronously fail.
  for (std::size_t i = 0; i < children.size(); ++i)
  {
    std::vector<PExpression> start{idle, trigger};

    for (std::size_t j = 0; j < i; ++j)
      start.push_back(notEqual(children[j].triggerReply, constants::resultFailure()));

    const auto p = id + "_child" + std::to_string(i);

    behaviour.definitions.push_back({p + "_trigger", allOf(start), behaviour.origin});

    // IMPORTANT:
    //
    // Do not abort a sibling directly from another child's outcome.
    // That creates:
    //
    //   child outcome -> sibling abort -> sibling outcome -> child abort
    //
    // Instead, failure first moves Join into Aborting. Remaining active
    // children are cancelled on the following transition.
    behaviour.definitions.push_back({p + "_abort", logicalAnd(logicalNot(completed[i]), logicalOr(aborting, logicalAnd(running, abort))), behaviour.origin});
    behaviour.definitions.push_back({p + "_reset", boolean(false), behaviour.origin});
  }

  // -------------------------------------------------------------------------
  // Synchronous startup result
  // -------------------------------------------------------------------------

  std::vector<PExpression> startupDone;
  std::vector<PExpression> startupNonFailure;

  for (auto& c : children)
  {
    startupDone.push_back(equal(c.triggerReply, constants::resultDone()));

    startupNonFailure.push_back(notEqual(c.triggerReply, constants::resultFailure()));
  }

  const auto allStartupDone = allOf(startupDone);
  const auto allStartupNonFailure = allOf(startupNonFailure);

  behaviour.triggerReply = cases(
      {
          {allOf({idle, trigger, allStartupDone}), constants::resultDone()},

          {allOf({idle, trigger, allStartupNonFailure}), constants::resultSuccess()},
      },
      constants::resultFailure());

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
    abortSucceeded.push_back(logicalOr(completed[i], equal(children[i].abortReply, constants::resultSuccess())));
    abortFailed.push_back(logicalAnd(logicalNot(completed[i]), equal(children[i].abortReply, constants::resultFailure())));
    abortRunning.push_back(logicalAnd(logicalNot(completed[i]), equal(children[i].abortReply, constants::resultRunning())));
  }

  const auto allAbortSucceeded = allOf(abortSucceeded);
  const auto anyAbortFailed = anyOf(abortFailed);
  const auto anyAbortRunning = anyOf(abortRunning);

  // -------------------------------------------------------------------------
  // Public interface
  // -------------------------------------------------------------------------

  // External abort is accepted while Running. Cleanup itself occurs in the
  // subsequent Aborting state.
  behaviour.abortReply = cases(
      {
          {logicalAnd(running, anyAbortFailed), constants::resultFailure()},
          {logicalAnd(running, anyAbortRunning), constants::resultRunning()},
          {logicalAnd(running, allAbortSucceeded), constants::resultSuccess()},
          {error, constants::resultError()},
      },
      constants::resultNone());

  behaviour.resetReply = cases(
      {
          {error, constants::resultSuccess()},
      },
      constants::resultNone());

  // A successful Join still completes immediately when all children finish.
  //
  // Failure is exposed after entering Aborting. This ensures sibling cleanup
  // is no longer combinationally connected to the failing child.
  behaviour.outcome = cases(
      {
          {logicalAnd(aborting, failureAbort), constants::eventFailure()},

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
                                           {logicalAnd(aborting, failureAbort), reference("Error")},

                                           // Cleanup caused by external abort finishes in Idle.
                                           {logicalAnd(aborting, logicalNot(failureAbort)), reference("Idle")},

                                           // Reset error state.
                                           {logicalAnd(error, reset), reference("Idle")},
                                       },
                                       state,
                                       behaviour.origin});

  // Remember why we entered Aborting.
  behaviour.nextAssignments.push_back({id + "_failure_abort",
                                       {
                                           {logicalAnd(running, anyFailure), boolean(true)},
                                           {aborting, boolean(false)},
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
    behaviour.definitions.push_back({p + "_reset", boolean(false), behaviour.origin});
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
  behaviour.outcome =
      cases({{logicalAnd(running, anyOf(asyncSuccess)), constants::eventSuccess()}, {logicalAnd(running, allOf(failedNow)), constants::eventFailure()}},
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
  behaviour.variables.push_back({VariableKind::Input, id + "_timeout", "boolean", nullptr, behaviour.origin});
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Do, Else, Error}", reference("Idle"), behaviour.origin});

  Behaviour body, fallback;
  RETURN_ON_FAILURE(compileStrategy(within.body, reference(id + "_body_trigger"), reference(id + "_body_abort"), reference(id + "_body_reset"), body));
  materializeBehaviourInterface(body);
  RETURN_ON_FAILURE(
      compileStrategy(within.fallback, reference(id + "_fallback_trigger"), reference(id + "_fallback_abort"), reference(id + "_fallback_reset"), fallback));
  materializeBehaviourInterface(fallback);

  const auto idle = equal(state, reference("Idle")), doing = equal(state, reference("Do")), elsing = equal(state, reference("Else")),
             error = equal(state, reference("Error"));
  const auto bodySuccess = equal(body.outcome, constants::eventSuccess());
  const auto bodyFailure = equal(body.outcome, constants::eventFailure());
  const auto fbSuccess = equal(fallback.outcome, constants::eventSuccess());
  const auto fbFailure = equal(fallback.outcome, constants::eventFailure());

  behaviour.definitions.push_back({id + "_body_trigger", logicalAnd(idle, trigger), behaviour.origin});
  behaviour.definitions.push_back({id + "_body_abort", logicalAnd(doing, logicalOr(abort, timeout)), behaviour.origin});
  behaviour.definitions.push_back({id + "_body_reset", boolean(false), behaviour.origin});
  behaviour.definitions.push_back({id + "_fallback_trigger", logicalAnd(doing, timeout), behaviour.origin});
  behaviour.definitions.push_back({id + "_fallback_abort", logicalAnd(elsing, abort), behaviour.origin});
  behaviour.definitions.push_back({id + "_fallback_reset", boolean(false), behaviour.origin});

  behaviour.triggerReply = cases({{allOf({idle, equal(body.triggerReply, constants::resultDone())}), constants::resultDone()},
                                  {allOf({idle, equal(body.triggerReply, constants::resultFailure())}), constants::resultFailure()},
                                  {allOf({idle, equal(body.triggerReply, constants::resultSuccess())}), constants::resultSuccess()}},
                                 constants::resultNone());
  behaviour.abortReply = cases({{doing, body.abortReply}, {elsing, fallback.abortReply}, {error, constants::resultError()}}, constants::resultNone());
  behaviour.resetReply = cases({{error, constants::resultSuccess()}}, constants::resultNone());

  behaviour.outcome = cases({{logicalAnd(doing, bodySuccess), constants::eventSuccess()},
                             {logicalAnd(doing, bodyFailure), constants::eventFailure()},
                             {logicalAnd(elsing, fbSuccess), constants::eventSuccess()},
                             {logicalAnd(elsing, fbFailure), constants::eventFailure()},
                             {allOf({doing, timeout, equal(fallback.triggerReply, constants::resultDone())}), constants::eventSuccess()},
                             {allOf({doing, timeout, equal(fallback.triggerReply, constants::resultFailure())}), constants::eventFailure()}},
                            constants::eventNone());

  behaviour.nextAssignments.push_back({id + "_state",
                                       {{allOf({idle, trigger, equal(body.triggerReply, constants::resultSuccess())}), reference("Do")},
                                        {allOf({idle, trigger, equal(body.triggerReply, constants::resultFailure())}), reference("Error")},
                                        {logicalAnd(doing, bodySuccess), reference("Idle")},
                                        {logicalAnd(doing, bodyFailure), reference("Error")},
                                        {allOf({doing, timeout, equal(fallback.triggerReply, constants::resultSuccess())}), reference("Else")},
                                        {allOf({doing, timeout, equal(fallback.triggerReply, constants::resultDone())}), reference("Idle")},
                                        {allOf({doing, timeout, equal(fallback.triggerReply, constants::resultFailure())}), reference("Error")},
                                        {logicalAnd(elsing, fbSuccess), reference("Idle")},
                                        {logicalAnd(elsing, fbFailure), reference("Error")},
                                        {logicalAnd(doing, abort), reference("Idle")},
                                        {logicalAnd(elsing, abort), reference("Idle")},
                                        {logicalAnd(error, reset), reference("Idle")}},
                                       state,
                                       behaviour.origin});

  // Timeout is an abstract environment event: only possible while the body is active.
  behaviour.constraints.push_back({binary(timeout, "->", doing), behaviour.origin});
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

  // End is an immediate successful termination marker. It has no persistent state.
  behaviour.triggerReply = cases({{trigger, constants::resultDone()}}, constants::resultNone());
  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();
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

  // Continue completes the current body synchronously. The enclosing Repeat consumes
  // RESULT_DONE as its loop-control result. No artificial runtime state is needed.
  behaviour.triggerReply = cases({{trigger, constants::resultDone()}}, constants::resultNone());
  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();
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
  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;

  std::string domain = "{Idle, Error";
  for (std::size_t i = 0; i < choose.options.size(); ++i)
    domain += ", Option" + std::to_string(i);
  domain += "}";
  behaviour.variables.push_back({VariableKind::State, id + "_state", domain, reference("Idle"), behaviour.origin});

  std::vector<Behaviour> options(choose.options.size());
  std::vector<PExpression> conditions;
  for (std::size_t i = 0; i < choose.options.size(); ++i)
  {
    if (!choose.options[i].strategy)
      return VoidResult::Failed("nuXmv Choose option {} has no strategy", i);

    // Data-expression translation is a separate backend concern. Until connected,
    // expose each condition explicitly instead of pretending to translate it.
    const auto conditionName = id + "_condition" + std::to_string(i);
    behaviour.variables.push_back({VariableKind::Input, conditionName, "boolean", nullptr, behaviour.origin});
    conditions.push_back(reference(conditionName));

    const auto p = id + "_option" + std::to_string(i);
    RETURN_ON_FAILURE(compileStrategy(choose.options[i].strategy, reference(p + "_trigger"), reference(p + "_abort"), reference(p + "_reset"), options[i]));
    materializeBehaviourInterface(options[i]);

    // First true option wins, matching ordered choice.
    std::vector<PExpression> selected{equal(state, reference("Idle")), trigger, conditions[i]};
    for (std::size_t j = 0; j < i; ++j)
      selected.push_back(logicalNot(conditions[j]));
    behaviour.definitions.push_back({p + "_trigger", allOf(selected), behaviour.origin});
    behaviour.definitions.push_back({p + "_abort", logicalAnd(equal(state, reference("Option" + std::to_string(i))), abort), behaviour.origin});
    behaviour.definitions.push_back({p + "_reset", boolean(false), behaviour.origin});
  }

  std::vector<Expression::Case::Branch> triggerCases, outcomeCases;
  std::vector<GuardedValue> nextCases;
  for (std::size_t i = 0; i < options.size(); ++i)
  {
    std::vector<PExpression> selected{equal(state, reference("Idle")), trigger, conditions[i]};
    for (std::size_t j = 0; j < i; ++j)
      selected.push_back(logicalNot(conditions[j]));
    const auto sel = allOf(selected);
    triggerCases.push_back({sel, options[i].triggerReply});

    const auto active = equal(state, reference("Option" + std::to_string(i)));
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventSuccess())), constants::eventSuccess()});
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventFailure())), constants::eventFailure()});
    outcomeCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventAborted())), constants::eventAborted()});

    nextCases.push_back({allOf({sel, equal(options[i].triggerReply, constants::resultSuccess())}), reference("Option" + std::to_string(i))});
    nextCases.push_back({allOf({sel, equal(options[i].triggerReply, constants::resultFailure())}), reference("Error")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventSuccess())), reference("Idle")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventFailure())), reference("Error")});
    nextCases.push_back({logicalAnd(active, equal(options[i].outcome, constants::eventAborted())), reference("Idle")});
    nextCases.push_back({logicalAnd(active, abort), reference("Idle")});
  }

  behaviour.triggerReply = cases(std::move(triggerCases), constants::resultFailure());
  behaviour.outcome = cases(std::move(outcomeCases), constants::eventNone());
  behaviour.abortReply = cases({{logicalNot(equal(state, reference("Idle"))), constants::resultSuccess()}}, constants::resultNone());
  behaviour.resetReply = cases({{equal(state, reference("Error")), constants::resultSuccess()}}, constants::resultNone());
  nextCases.push_back({logicalAnd(equal(state, reference("Error")), reset), reference("Idle")});
  behaviour.nextAssignments.push_back({id + "_state", std::move(nextCases), state, behaviour.origin});

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

  auto& flow = findFlow(call.target);

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
  auto& flow = findFlow(call.receiver);

  behaviour.id = id;
  behaviour.origin = provenance(strategy);
  behaviour.trigger = trigger;
  behaviour.abort = abort;
  behaviour.reset = reset;

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
          {equal(abortReply, constants::resultRunning()), constants::resultRunning()},
          {equal(abortReply, constants::resultSuccess()), constants::resultDone()},
      },
      constants::resultNone());

  behaviour.abortReply = constants::resultNone();
  behaviour.resetReply = constants::resultNone();
  behaviour.outcome = constants::eventNone();

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
  const auto hasState = std::ranges::any_of(
      behaviour.variables, [&stateName](const Variable& variable) { return variable.kind == VariableKind::State && variable.name == stateName; });

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
        // A flow starts on the rising edge of a call site's persistent
        // running state. This avoids expanding the synchronous trigger-reply
        // tree in temporal properties.
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(flowRisingExpression(call));

        return anyOf(expressions);
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
          expressions.push_back(equal(call.outcome, constants::eventSuccess()));

        return anyOf(expressions);
      }
      case ir::Observation::Kind::Failed:
      {
        std::vector<PExpression> expressions;
        for (const auto& call : flow.calls)
          expressions.push_back(equal(call.outcome, constants::eventFailure()));

        return anyOf(expressions);
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
      std::vector<PExpression> expressions;
      for (const auto* call : calls)
        expressions.push_back(capabilityRisingExpression(instance, call));
      return anyOf(expressions);
    }
    case ir::Observation::Kind::Rejected:
    {
      std::vector<PExpression> expressions;
      for (const auto* call : calls)
        expressions.push_back(logicalAnd(call->trigger, equal(call->triggerReply, constants::resultFailure())));

      return anyOf(expressions);
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
      std::vector<PExpression> expressions;
      for (const auto& call : calls)
        expressions.push_back(logicalAnd(call->abort, equal(call->abortReply, constants::resultSuccess())));

      return anyOf(expressions);
    }
    case ir::Observation::Kind::Failed:
    {
      std::vector<PExpression> expressions;
      for (const auto& call : calls)
        expressions.push_back(logicalAnd(call->abort, equal(call->abortReply, constants::resultSuccess())));

      return anyOf(expressions);
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

SemanticCompiler::FlowInstance& SemanticCompiler::findFlow(SymbolId symbol)
{
  if (const auto it = mFlowInstances.find(symbol); it != mFlowInstances.end())
    return it->second;

  FlowInstance instance{
      .symbol = symbol,
      .id = sanitize(sourceName(symbol)),
      .calls = {},
      .abortRequests = {},
      .abortReplies = {},
  };

  return mFlowInstances.emplace(symbol, std::move(instance)).first->second;
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
  behaviour.variables.push_back({VariableKind::Input, id + "_trigger_result", "{2, 5}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_reset_result", "{1, 2}", {}, origin});
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

  stateCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("Idle")});

  behaviour.nextAssignments.push_back({id + "_state", std::move(stateCases), state, origin});

  // A capability instance is exclusive even when calls complete synchronously.
  for (std::size_t i = 0; i < capability.calls.size(); ++i)
  {
    for (std::size_t j = i + 1; j < capability.calls.size(); ++j)
    {
      behaviour.constraints.push_back({logicalNot(logicalAnd(capability.calls[i].trigger, capability.calls[j].trigger)), origin});
      behaviour.constraints.push_back({logicalNot(logicalAnd(capability.calls[i].reset, capability.calls[j].reset)), origin});
    }
  }

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
  behaviour.variables.push_back({VariableKind::Input, id + "_trigger_result", "{1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_abort_result", "{1, 2, 3}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_reset_result", "{1, 2}", {}, origin});
  behaviour.variables.push_back({VariableKind::Input, id + "_event", "0..2", {}, origin});
  behaviour.variables.push_back({VariableKind::State, id + "_state", "{Idle, Running, Error}", reference("Idle"), origin});
  behaviour.variables.push_back({VariableKind::State, id + "_owner", ownerDomain, reference("None"), origin});

  std::vector<PExpression> triggers;
  std::vector<PExpression> resets;
  std::vector<PExpression> ownedAborts;
  for (const auto& call : capability.calls)
  {
    triggers.push_back(call.trigger);
    resets.push_back(call.reset);
    ownedAborts.push_back(logicalAnd(equal(owner, reference(call.id)), call.abort));
  }

  const auto anyTrigger = anyOf(triggers);
  const auto anyReset = anyOf(resets);
  const auto anyOwnedAbort = anyOf(ownedAborts);
  const auto noCommand = logicalNot(logicalOr(anyTrigger, logicalOr(anyOwnedAbort, anyReset)));

  std::vector<GuardedValue> stateCases;
  for (const auto& call : capability.calls)
  {
    stateCases.push_back({and3(equal(state, reference("Idle")), call.trigger, equal(triggerResult, constants::resultSuccess())), reference("Running")});
    stateCases.push_back({and3(equal(state, reference("Idle")), call.trigger, equal(triggerResult, constants::resultFailure())), reference("Error")});
  }

  stateCases.push_back({allOf({equal(state, reference("Running")), noCommand, equal(event, constants::eventSuccess())}), reference("Idle")});
  stateCases.push_back({allOf({equal(state, reference("Running")), noCommand, equal(event, constants::eventFailure())}), reference("Error")});
  stateCases.push_back({allOf({equal(state, reference("Running")), anyOwnedAbort, equal(abortResult, constants::resultSuccess())}), reference("Idle")});
  stateCases.push_back({allOf({equal(state, reference("Running")), anyOwnedAbort, equal(abortResult, constants::resultFailure())}), reference("Error")});
  stateCases.push_back({allOf({equal(state, reference("Running")), anyOwnedAbort, equal(abortResult, constants::resultRunning())}), reference("Running")});
  stateCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("Idle")});
  behaviour.nextAssignments.push_back({id + "_state", std::move(stateCases), state, origin});

  std::vector<GuardedValue> ownerCases;
  for (const auto& call : capability.calls)
    ownerCases.push_back({allOf({equal(state, reference("Idle")), call.trigger, equal(triggerResult, constants::resultSuccess())}), reference(call.id)});
  ownerCases.push_back({allOf({equal(state, reference("Running")), noCommand, equal(event, constants::eventSuccess())}), reference("None")});
  ownerCases.push_back({allOf({equal(state, reference("Running")), noCommand, equal(event, constants::eventFailure())}), reference("None")});
  ownerCases.push_back({allOf({equal(state, reference("Running")), anyOwnedAbort, equal(abortResult, constants::resultSuccess())}), reference("None")});
  ownerCases.push_back({allOf({equal(state, reference("Running")), anyOwnedAbort, equal(abortResult, constants::resultFailure())}), reference("None")});
  ownerCases.push_back({allOf({equal(state, reference("Error")), anyReset, equal(resetResult, constants::resultSuccess())}), reference("None")});
  behaviour.nextAssignments.push_back({id + "_owner", std::move(ownerCases), owner, origin});

  for (std::size_t i = 0; i < capability.calls.size(); ++i)
  {
    for (std::size_t j = i + 1; j < capability.calls.size(); ++j)
    {
      behaviour.constraints.push_back({logicalNot(logicalAnd(capability.calls[i].trigger, capability.calls[j].trigger)), origin});
      behaviour.constraints.push_back({logicalNot(logicalAnd(capability.calls[i].reset, capability.calls[j].reset)), origin});
    }
  }

  behaviour.constraints.push_back({
      allOf({
          binary(equal(state, reference("Idle")), "->", logicalOr(noCommand, anyTrigger)),
          binary(equal(state, reference("Running")), "->", logicalOr(noCommand, anyOwnedAbort)),
          binary(equal(state, reference("Error")), "->", logicalOr(noCommand, anyReset)),
          binary(logicalNot(equal(state, reference("Running"))), "->", equal(event, constants::eventNone())),
          binary(logicalAnd(equal(state, reference("Running")), logicalNot(noCommand)), "->", equal(event, constants::eventNone())),
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
