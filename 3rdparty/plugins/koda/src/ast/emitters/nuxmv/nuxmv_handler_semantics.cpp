#include "nuxmv_handler_semantics.h"

#include <iterator>

#include "nuxmv_constants.h"
#include "nuxmv_semantic_contracts.h"

namespace koda::nuxmv
{
namespace
{
void mergeBehaviour(Behaviour& target, Behaviour&& child)
{
  target.variables.insert(target.variables.end(), std::make_move_iterator(child.variables.begin()), std::make_move_iterator(child.variables.end()));
  target.definitions.insert(target.definitions.end(), std::make_move_iterator(child.definitions.begin()), std::make_move_iterator(child.definitions.end()));
  target.nextAssignments.insert(target.nextAssignments.end(), std::make_move_iterator(child.nextAssignments.begin()),
                                std::make_move_iterator(child.nextAssignments.end()));
  target.constraints.insert(target.constraints.end(), std::make_move_iterator(child.constraints.begin()), std::make_move_iterator(child.constraints.end()));
  target.expectations.insert(target.expectations.end(), std::make_move_iterator(child.expectations.begin()), std::make_move_iterator(child.expectations.end()));
}

const ir::PHandler* findHandler(const std::vector<ir::PHandler>& handlers, ir::HandlerKind kind)
{
  for (const auto& handler : handlers)
    if (handler && handler->kind == kind)
      return &handler;
  return nullptr;
}
}  // namespace

VoidResult applyHandlers(const std::string& ownerId, const std::vector<ir::PHandler>& handlers, const HandlerBodyCompiler& compileBody, Behaviour& wrapped)
{
  const auto* errorHandler = findHandler(handlers, ir::HandlerKind::Error);
  const auto* abortHandler = findHandler(handlers, ir::HandlerKind::Abort);

  for (const auto& handler : handlers)
  {
    if (!handler)
      continue;
    if (handler->kind == ir::HandlerKind::Emitter || handler->kind == ir::HandlerKind::EmitterContinue)
      return VoidResult::Failed("nuXmv handler lowering does not yet support emitter handlers on '{}'", ownerId);
  }

  if (!errorHandler && !abortHandler)
    return {};

  // Preserve the wrapped interface. The wrapper consumes wrapped failure/abort
  // outcomes and exposes its own routed result/outcome.
  const auto originalTriggerReply = wrapped.triggerReply;
  const auto originalAbortReply = wrapped.abortReply;
  const auto originalResetReply = wrapped.resetReply;
  const auto originalOutcome = wrapped.outcome;
  const bool originalMayReturnDone = wrapped.mayReturnDone;
  const bool originalMayAccept = wrapped.mayAccept;
  const bool originalMayReject = wrapped.mayReject;
  const bool originalMayEmitSuccess = wrapped.mayEmitSuccess;
  const bool originalMayEmitFailure = wrapped.mayEmitFailure;
  const bool originalMayEmitAborted = wrapped.mayEmitAborted;

  const auto handlerStateName = ownerId + "_handler_state";
  const auto handlerState = reference(handlerStateName);

  std::string domain = "{None";
  if (errorHandler)
    domain += ", Recovering";
  if (abortHandler)
    domain += ", Aborting";
  domain += "}";

  wrapped.variables.push_back({
    VariableKind::State,
    handlerStateName,
    domain,
    reference("None"),
    wrapped.origin,
  });

  Behaviour recovery;
  Behaviour abortRecovery;

  if (errorHandler)
  {
    if (!(*errorHandler)->body)
      return VoidResult::Failed("Error handler on '{}' has no body", ownerId);

    RETURN_ON_FAILURE(compileBody((*errorHandler)->body, reference(ownerId + "_recovery_trigger"), reference(ownerId + "_recovery_abort"),
                                  reference(ownerId + "_recovery_reset"), recovery));

    const auto startRecovery = logicalOr(allOf({wrapped.trigger, equal(originalTriggerReply, constants::resultFailure())}),
                                         equal(originalOutcome, constants::eventFailure()));

    wrapped.definitions.push_back({
      ownerId + "_recovery_trigger",
      logicalAnd(equal(handlerState, reference("None")), startRecovery),
      wrapped.origin,
      DefinitionEncoding::ConstrainedBooleanInput,
    });
    wrapped.definitions.push_back({
      ownerId + "_recovery_abort",
      logicalAnd(equal(handlerState, reference("Recovering")), wrapped.abort),
      wrapped.origin,
    });
    wrapped.definitions.push_back({
      ownerId + "_recovery_reset",
      boolean(false),
      wrapped.origin,
    });
  }

  if (abortHandler)
  {
    if (!(*abortHandler)->body)
      return VoidResult::Failed("Abort handler on '{}' has no body", ownerId);

    RETURN_ON_FAILURE(compileBody((*abortHandler)->body, reference(ownerId + "_abort_handler_trigger"), reference(ownerId + "_abort_handler_abort"),
                                  reference(ownerId + "_abort_handler_reset"), abortRecovery));

    // An explicit abort request does not imply that cancellation succeeded.
    // Only start the handler when the core confirmed cancellation, or when the
    // execution-lifecycle ABORTED notification arrived (possibly from another
    // observer of the same capability execution).
    const auto successfulExplicitAbort = allOf({wrapped.abort, equal(originalAbortReply, constants::resultSuccess())});
    const auto startAbortHandler = logicalOr(successfulExplicitAbort, equal(originalOutcome, constants::eventAborted()));

    wrapped.definitions.push_back({
      ownerId + "_abort_handler_trigger",
      logicalAnd(equal(handlerState, reference("None")), startAbortHandler),
      wrapped.origin,
    });
    wrapped.definitions.push_back({
      ownerId + "_abort_handler_abort",
      logicalAnd(equal(handlerState, reference("Aborting")), wrapped.abort),
      wrapped.origin,
    });
    wrapped.definitions.push_back({
      ownerId + "_abort_handler_reset",
      boolean(false),
      wrapped.origin,
    });
  }

  // Trigger reply: synchronous failure can immediately enter recovery.
  // Avoid the RESULT_DONE branch when recovery cannot finish synchronously.
  if (errorHandler)
  {
    const auto handlingFailure = allOf({equal(handlerState, reference("None")), equal(originalTriggerReply, constants::resultFailure())});

    std::vector<Expression::Case::Branch> triggerCases;
    if (recovery.mayReturnDone)
      triggerCases.push_back({allOf({handlingFailure, equal(recovery.triggerReply, constants::resultDone())}), constants::resultDone()});
    triggerCases.push_back({allOf({handlingFailure, equal(recovery.triggerReply, constants::resultSuccess())}), constants::resultSuccess()});

    wrapped.triggerReply = cases(std::move(triggerCases), originalTriggerReply);
  }

  // An error handler may turn a synchronous failure into synchronous DONE.
  // An abort handler changes only the abort interface, not the trigger reply.
  wrapped.mayReturnDone = originalMayReturnDone || (errorHandler && originalMayReject && recovery.mayReturnDone);

  // Upper bounds for the wrapper's observable modes. When in doubt we retain
  // a mode rather than incorrectly deleting a state in an enclosing Sequence.
  // The handler lifecycle still has open cancellation/disposition questions in
  // KODA v0.4, so these flags must never be treated as equivalence proofs.
  wrapped.mayAccept = originalMayAccept || (errorHandler && originalMayReject && recovery.mayAccept);
  wrapped.mayReject = originalMayReject;
  wrapped.mayEmitSuccess = originalMayEmitSuccess ||
                           (errorHandler && (originalMayReject || originalMayEmitFailure) && (recovery.mayReturnDone || recovery.mayEmitSuccess)) ||
                           (abortHandler && (originalMayAccept || originalMayEmitAborted) && (abortRecovery.mayReturnDone || abortRecovery.mayEmitSuccess));
  wrapped.mayEmitFailure = originalMayEmitFailure ||
                           (errorHandler && (originalMayReject || originalMayEmitFailure) && (recovery.mayReject || recovery.mayEmitFailure)) ||
                           (abortHandler && (originalMayAccept || originalMayEmitAborted) && (abortRecovery.mayReject || abortRecovery.mayEmitFailure));
  wrapped.mayEmitAborted = originalMayEmitAborted || (abortHandler && abortRecovery.mayEmitAborted);

  // External abort starts AbortHandler synchronously. A successfully completed
  // handler means the abort was handled and the wrapper completes successfully.
  if (abortHandler)
  {
    // Do not route a failed core abort through the handler.  A handler that
    // accepted asynchronously corresponds to Dezyne's Handling reply, not to
    // an already completed abort. RESULT_RUNNING preserves the parent's active
    // state until the handler reports its eventual outcome.
    const auto startingAbort = allOf({equal(handlerState, reference("None")), wrapped.abort, equal(originalAbortReply, constants::resultSuccess())});
    std::vector<Expression::Case::Branch> abortReplyCases;
    if (abortRecovery.mayReturnDone)
      abortReplyCases.push_back({allOf({startingAbort, equal(abortRecovery.triggerReply, constants::resultDone())}), constants::resultSuccess()});
    abortReplyCases.push_back({allOf({startingAbort, equal(abortRecovery.triggerReply, constants::resultSuccess())}), constants::resultRunning()});
    abortReplyCases.push_back({allOf({startingAbort, equal(abortRecovery.triggerReply, constants::resultFailure())}), constants::resultFailure()});

    wrapped.abortReply = cases(std::move(abortReplyCases), originalAbortReply);
  }

  std::vector<Expression::Case::Branch> outcomeCases;

  if (errorHandler)
  {
    if (recovery.mayReturnDone)
      outcomeCases.push_back({
        allOf({
          equal(handlerState, reference("None")),
          equal(originalOutcome, constants::eventFailure()),
          equal(recovery.triggerReply, constants::resultDone()),
        }),
        constants::eventSuccess(),
      });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("None")),
        equal(originalOutcome, constants::eventFailure()),
        equal(recovery.triggerReply, constants::resultFailure()),
      }),
      constants::eventFailure(),
    });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("Recovering")),
        equal(recovery.outcome, constants::eventSuccess()),
      }),
      constants::eventSuccess(),
    });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("Recovering")),
        equal(recovery.outcome, constants::eventFailure()),
      }),
      constants::eventFailure(),
    });
  }

  if (abortHandler)
  {
    if (abortRecovery.mayReturnDone)
      outcomeCases.push_back({
        allOf({
          equal(handlerState, reference("None")),
          equal(originalOutcome, constants::eventAborted()),
          equal(abortRecovery.triggerReply, constants::resultDone()),
        }),
        constants::eventSuccess(),
      });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("None")),
        equal(originalOutcome, constants::eventAborted()),
        equal(abortRecovery.triggerReply, constants::resultFailure()),
      }),
      constants::eventFailure(),
    });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("Aborting")),
        equal(abortRecovery.outcome, constants::eventSuccess()),
      }),
      constants::eventSuccess(),
    });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("Aborting")),
        equal(abortRecovery.outcome, constants::eventFailure()),
      }),
      constants::eventFailure(),
    });
    outcomeCases.push_back({
      allOf({
        equal(handlerState, reference("Aborting")),
        equal(abortRecovery.outcome, constants::eventAborted()),
      }),
      constants::eventAborted(),
    });
  }

  // While a handler is active, the wrapped outcome is hidden.  In particular,
  // consume the core's ABORTED notification while the abort handler starts:
  // forwarding it immediately would make the enclosing Sequence finish while
  // an accepted asynchronous handler is still running.
  // The error handler has claimed this failure when it accepted a recovery
  // invocation.  Do not forward the original EVENT_FAILURE while that
  // asynchronous handler starts: it would cause the enclosing composition to
  // enter Error (and become resettable) while recovery is still running.
  const auto acceptedErrorRecovery = errorHandler
                                       ? allOf({equal(originalOutcome, constants::eventFailure()), equal(recovery.triggerReply, constants::resultSuccess())})
                                       : boolean(false);
  outcomeCases.push_back({
    allOf({
      equal(handlerState, reference("None")),
      logicalNot(acceptedErrorRecovery),
      abortHandler ? notEqual(originalOutcome, constants::eventAborted()) : boolean(true),
    }),
    originalOutcome,
  });
  wrapped.outcome = cases(std::move(outcomeCases), constants::eventNone());

  std::vector<GuardedValue> handlerNext;

  if (errorHandler)
  {
    const auto startAsyncRecovery = allOf({
      equal(handlerState, reference("None")),
      logicalOr(equal(originalTriggerReply, constants::resultFailure()), equal(originalOutcome, constants::eventFailure())),
      equal(recovery.triggerReply, constants::resultSuccess()),
    });

    handlerNext.push_back({startAsyncRecovery, reference("Recovering")});
    handlerNext.push_back({
      allOf({
        equal(handlerState, reference("Recovering")),
        logicalOr(equal(recovery.outcome, constants::eventSuccess()), equal(recovery.outcome, constants::eventFailure())),
      }),
      reference("None"),
    });
  }

  if (abortHandler)
  {
    const auto abortSource = logicalOr(allOf({wrapped.abort, equal(originalAbortReply, constants::resultSuccess())}),
                                       equal(originalOutcome, constants::eventAborted()));

    const auto startAsyncAbortHandler = allOf({
      equal(handlerState, reference("None")),
      abortSource,
      equal(abortRecovery.triggerReply, constants::resultSuccess()),
    });

    handlerNext.push_back({startAsyncAbortHandler, reference("Aborting")});
    handlerNext.push_back({
      allOf({
        equal(handlerState, reference("Aborting")),
        anyOf({equal(abortRecovery.outcome, constants::eventSuccess()), equal(abortRecovery.outcome, constants::eventFailure()),
               equal(abortRecovery.outcome, constants::eventAborted())}),
      }),
      reference("None"),
    });
  }

  wrapped.nextAssignments.push_back({
    handlerStateName,
    std::move(handlerNext),
    handlerState,
    wrapped.origin,
  });

  // These contracts test routing and completion of the recovery wrapper;
  // they never constrain the generated transition relation.
  const auto none = equal(handlerState, reference("None"));
  if (errorHandler)
  {
    const auto recovering = equal(handlerState, reference("Recovering"));
    const auto recoveryTrigger = reference(ownerId + "_recovery_trigger");
    contracts::always(wrapped, "error_handler_activated_on_failure",
                      contracts::implies(allOf({none, logicalOr(allOf({wrapped.trigger, equal(originalTriggerReply, constants::resultFailure())}),
                                                                equal(originalOutcome, constants::eventFailure()))}),
                                         recoveryTrigger));
    if (recovery.mayAccept)
    {
      contracts::nextState(wrapped, "accepted_error_handler_recovers", allOf({none, recoveryTrigger, equal(recovery.triggerReply, constants::resultSuccess())}),
                           recovering);
      contracts::always(wrapped, "accepted_error_recovery_consumes_core_failure",
                        contracts::implies(allOf({
                                             none,
                                             equal(originalOutcome, constants::eventFailure()),
                                             equal(recovery.triggerReply, constants::resultSuccess()),
                                           }),
                                           equal(wrapped.outcome, constants::eventNone())));
    }
    if (recovery.mayEmitSuccess)
      contracts::always(wrapped, "error_handler_success_propagates",
                        contracts::implies(allOf({recovering, equal(recovery.outcome, constants::eventSuccess())}),
                                           equal(wrapped.outcome, constants::eventSuccess())));
    if (recovery.mayEmitFailure)
      contracts::always(wrapped, "error_handler_failure_propagates",
                        contracts::implies(allOf({recovering, equal(recovery.outcome, constants::eventFailure())}),
                                           equal(wrapped.outcome, constants::eventFailure())));
  }
  if (abortHandler)
  {
    const auto aborting = equal(handlerState, reference("Aborting"));
    const auto abortTrigger = reference(ownerId + "_abort_handler_trigger");
    contracts::always(wrapped, "abort_handler_routed",
                      contracts::implies(allOf({none, logicalOr(allOf({wrapped.abort, equal(originalAbortReply, constants::resultSuccess())}),
                                                                equal(originalOutcome, constants::eventAborted()))}),
                                         abortTrigger));
    // A recovery handler must not start merely because abort() was requested:
    // the underlying action must have accepted cancellation (or emitted an
    // ABORTED lifecycle notification). This is a normative KODA v0.4 check.
    contracts::always(wrapped, "abort_handler_requires_successful_cancel",
                      contracts::implies(allOf({none, abortTrigger}),
                                         logicalOr(equal(originalOutcome, constants::eventAborted()),
                                                   allOf({wrapped.abort, equal(originalAbortReply, constants::resultSuccess())}))));
    contracts::always(wrapped, "failed_abort_does_not_start_handler",
                      contracts::implies(allOf({none, wrapped.abort, equal(originalAbortReply, constants::resultFailure())}), logicalNot(abortTrigger)));
    contracts::always(wrapped, "failed_abort_reply_propagates",
                      contracts::implies(allOf({none, wrapped.abort, equal(originalAbortReply, constants::resultFailure())}),
                                         equal(wrapped.abortReply, constants::resultFailure())));
    if (abortRecovery.mayAccept)
    {
      contracts::nextState(wrapped, "accepted_abort_handler_runs", allOf({none, abortTrigger, equal(abortRecovery.triggerReply, constants::resultSuccess())}),
                           aborting);
      contracts::always(wrapped, "accepted_external_abort_remains_pending",
                        contracts::implies(allOf({none, wrapped.abort, equal(originalAbortReply, constants::resultSuccess()),
                                                  equal(abortRecovery.triggerReply, constants::resultSuccess())}),
                                           equal(wrapped.abortReply, constants::resultRunning())));
      contracts::always(wrapped, "aborted_notification_is_consumed",
                        contracts::implies(allOf({none, equal(originalOutcome, constants::eventAborted()),
                                                  equal(abortRecovery.triggerReply, constants::resultSuccess())}),
                                           equal(wrapped.outcome, constants::eventNone())));
    }
    if (abortRecovery.mayEmitSuccess)
      contracts::always(wrapped, "abort_handler_success_propagates",
                        contracts::implies(allOf({aborting, equal(abortRecovery.outcome, constants::eventSuccess())}),
                                           equal(wrapped.outcome, constants::eventSuccess())));
    if (abortRecovery.mayEmitAborted)
      contracts::always(wrapped, "abort_handler_abort_propagates",
                        contracts::implies(allOf({aborting, equal(abortRecovery.outcome, constants::eventAborted())}),
                                           equal(wrapped.outcome, constants::eventAborted())));
  }

  if (errorHandler)
    mergeBehaviour(wrapped, std::move(recovery));
  if (abortHandler)
    mergeBehaviour(wrapped, std::move(abortRecovery));

  // originalResetReply is intentionally retained. Handler state returns to None
  // after completion; underlying composite reset semantics remain owned by the
  // wrapped behaviour.
  wrapped.resetReply = originalResetReply;

  return {};
}

}  // namespace koda::nuxmv
