#include "nuxmv_handler_semantics.h"

#include "nuxmv_constants.h"

#include <iterator>

namespace koda::nuxmv
{
namespace
{
void mergeBehaviour(Behaviour& target, Behaviour&& child)
{
  target.variables.insert(target.variables.end(),
                          std::make_move_iterator(child.variables.begin()),
                          std::make_move_iterator(child.variables.end()));
  target.definitions.insert(target.definitions.end(),
                            std::make_move_iterator(child.definitions.begin()),
                            std::make_move_iterator(child.definitions.end()));
  target.nextAssignments.insert(target.nextAssignments.end(),
                                std::make_move_iterator(child.nextAssignments.begin()),
                                std::make_move_iterator(child.nextAssignments.end()));
  target.constraints.insert(target.constraints.end(),
                            std::make_move_iterator(child.constraints.begin()),
                            std::make_move_iterator(child.constraints.end()));
}

const ir::PHandler* findHandler(const std::vector<ir::PHandler>& handlers,
                                ir::HandlerKind kind)
{
  for (const auto& handler : handlers)
    if (handler && handler->kind == kind)
      return &handler;
  return nullptr;
}
}  // namespace

VoidResult applyHandlers(const std::string& ownerId,
                         const std::vector<ir::PHandler>& handlers,
                         const HandlerBodyCompiler& compileBody,
                         Behaviour& wrapped)
{
  const auto* errorHandler = findHandler(handlers, ir::HandlerKind::Error);
  const auto* abortHandler = findHandler(handlers, ir::HandlerKind::Abort);

  for (const auto& handler : handlers)
  {
    if (!handler)
      continue;
    if (handler->kind == ir::HandlerKind::Emitter ||
        handler->kind == ir::HandlerKind::EmitterContinue)
      return VoidResult::Failed(
          "nuXmv handler lowering does not yet support emitter handlers on '{}'",
          ownerId);
  }

  if (!errorHandler && !abortHandler)
    return {};

  // Preserve the wrapped interface. The wrapper consumes wrapped failure/abort
  // outcomes and exposes its own routed result/outcome.
  const auto originalTriggerReply = wrapped.triggerReply;
  const auto originalAbortReply = wrapped.abortReply;
  const auto originalResetReply = wrapped.resetReply;
  const auto originalOutcome = wrapped.outcome;

  const auto handlerStateName = ownerId + "_handler_state";
  const auto handlerState = reference(handlerStateName);

  std::string domain = "{None";
  if (errorHandler) domain += ", Recovering";
  if (abortHandler) domain += ", Aborting";
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

    RETURN_ON_FAILURE(compileBody(
        (*errorHandler)->body,
        reference(ownerId + "_recovery_trigger"),
        reference(ownerId + "_recovery_abort"),
        reference(ownerId + "_recovery_reset"),
        recovery));

    const auto startRecovery =
        logicalOr(
            equal(originalTriggerReply, constants::resultFailure()),
            equal(originalOutcome, constants::eventFailure()));

    wrapped.definitions.push_back({
        ownerId + "_recovery_trigger",
        logicalAnd(equal(handlerState, reference("None")), startRecovery),
        wrapped.origin,
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

    RETURN_ON_FAILURE(compileBody(
        (*abortHandler)->body,
        reference(ownerId + "_abort_handler_trigger"),
        reference(ownerId + "_abort_handler_abort"),
        reference(ownerId + "_abort_handler_reset"),
        abortRecovery));

    // Current KODA semantics: AbortHandler handles both sources:
    // 1. external abort requested on the parent;
    // 2. an EVENT_ABORTED originating inside the wrapped behaviour.
    const auto startAbortHandler =
        logicalOr(
            wrapped.abort,
            equal(originalOutcome, constants::eventAborted()));

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
  if (errorHandler)
  {
    wrapped.triggerReply = cases({
        {allOf({
             equal(handlerState, reference("None")),
             equal(originalTriggerReply, constants::resultFailure()),
             equal(recovery.triggerReply, constants::resultDone()),
         }),
         constants::resultDone()},
        {allOf({
             equal(handlerState, reference("None")),
             equal(originalTriggerReply, constants::resultFailure()),
             equal(recovery.triggerReply, constants::resultSuccess()),
         }),
         constants::resultSuccess()},
        {allOf({
             equal(handlerState, reference("None")),
             equal(originalTriggerReply, constants::resultFailure()),
             equal(recovery.triggerReply, constants::resultFailure()),
         }),
         constants::resultFailure()},
    }, originalTriggerReply);
  }

  // External abort starts AbortHandler synchronously. A successfully completed
  // handler means the abort was handled and the wrapper completes successfully.
  if (abortHandler)
  {
    wrapped.abortReply = cases({
        {allOf({
             equal(handlerState, reference("None")),
             wrapped.abort,
             equal(abortRecovery.triggerReply, constants::resultDone()),
         }),
         constants::resultDone()},
        {allOf({
             equal(handlerState, reference("None")),
             wrapped.abort,
             equal(abortRecovery.triggerReply, constants::resultSuccess()),
         }),
         constants::resultSuccess()},
        {allOf({
             equal(handlerState, reference("None")),
             wrapped.abort,
             equal(abortRecovery.triggerReply, constants::resultFailure()),
         }),
         constants::resultFailure()},
    }, originalAbortReply);
  }

  std::vector<Expression::Case::Branch> outcomeCases;

  if (errorHandler)
  {
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
  }

  // While a handler is active, the wrapped outcome is hidden. When no handler
  // consumes it, preserve the original outcome.
  outcomeCases.push_back({
      equal(handlerState, reference("None")),
      originalOutcome,
  });
  wrapped.outcome = cases(std::move(outcomeCases), constants::eventNone());

  std::vector<GuardedValue> handlerNext;

  if (errorHandler)
  {
    const auto startAsyncRecovery =
        allOf({
            equal(handlerState, reference("None")),
            logicalOr(
                equal(originalTriggerReply, constants::resultFailure()),
                equal(originalOutcome, constants::eventFailure())),
            equal(recovery.triggerReply, constants::resultSuccess()),
        });

    handlerNext.push_back({startAsyncRecovery, reference("Recovering")});
    handlerNext.push_back({
        allOf({
            equal(handlerState, reference("Recovering")),
            logicalOr(
                equal(recovery.outcome, constants::eventSuccess()),
                equal(recovery.outcome, constants::eventFailure())),
        }),
        reference("None"),
    });
  }

  if (abortHandler)
  {
    const auto abortSource =
        logicalOr(
            wrapped.abort,
            equal(originalOutcome, constants::eventAborted()));

    const auto startAsyncAbortHandler =
        allOf({
            equal(handlerState, reference("None")),
            abortSource,
            equal(abortRecovery.triggerReply, constants::resultSuccess()),
        });

    handlerNext.push_back({startAsyncAbortHandler, reference("Aborting")});
    handlerNext.push_back({
        allOf({
            equal(handlerState, reference("Aborting")),
            logicalOr(
                equal(abortRecovery.outcome, constants::eventSuccess()),
                equal(abortRecovery.outcome, constants::eventFailure())),
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
