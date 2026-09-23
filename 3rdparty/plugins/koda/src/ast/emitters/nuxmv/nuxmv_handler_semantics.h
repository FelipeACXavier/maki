#pragma once

#include <functional>

#include "ast/koda_ir.h"
#include "nuxmv_model.h"
#include "result.h"

namespace koda::nuxmv
{

using HandlerBodyCompiler = std::function<VoidResult(const ir::PStrategy&, const PExpression&, const PExpression&, const PExpression&, Behaviour&)>;

// Wraps `wrapped` with all supported handlers in source order.
//
// Error handler:
//   wrapped failure -> trigger recovery
//   recovery Done    -> EVENT_SUCCESS
//   recovery Success -> wait for recovery async outcome
//   recovery Failure -> EVENT_FAILURE
//
// Abort handler:
//   any abort indication from the wrapped behaviour triggers the handler:
//     * external parent abort command
//     * wrapped EVENT_ABORTED
//   successful handler completion turns the handled abort into success.
//
// Emitter / EmitterContinue are intentionally left unsupported here.
VoidResult applyHandlers(const std::string& ownerId, const std::vector<ir::PHandler>& handlers, const HandlerBodyCompiler& compileBody, Behaviour& wrapped);

}  // namespace koda::nuxmv
