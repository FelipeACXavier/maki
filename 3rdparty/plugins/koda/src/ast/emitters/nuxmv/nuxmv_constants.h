#pragma once

#include "nuxmv_expression.h"

namespace koda::nuxmv
{
namespace constants
{
inline PExpression resultNone()    { return reference("RESULT_NONE"); }
inline PExpression resultSuccess() { return reference("RESULT_SUCCESS"); }
inline PExpression resultFailure() { return reference("RESULT_FAILURE"); }
inline PExpression resultRunning() { return reference("RESULT_RUNNING"); }
inline PExpression resultError()   { return reference("RESULT_ERROR"); }
inline PExpression resultDone()    { return reference("RESULT_DONE"); }

inline PExpression commandNone()    { return reference("CMD_NONE"); }
inline PExpression commandTrigger() { return reference("CMD_TRIGGER"); }
inline PExpression commandAbort()   { return reference("CMD_ABORT"); }
inline PExpression commandReset()   { return reference("CMD_RESET"); }

inline PExpression eventNone()    { return reference("EVENT_NONE"); }
inline PExpression eventSuccess() { return reference("EVENT_SUCCESS"); }
inline PExpression eventFailure() { return reference("EVENT_FAILURE"); }
inline PExpression eventAborted() { return reference("EVENT_ABORTED"); }
}  // namespace constants

}  // namespace koda::nuxmv
