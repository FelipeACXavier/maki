#include "ast/emitters/nuxmv/nuxmv_handler_semantics.h"

#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_constants.h"
#include "ast/koda_compiler.h"

namespace
{
using namespace koda;
using namespace koda::nuxmv;

ir::PStrategy failureBody(const std::string& id)
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = id;
  strategy->value = ir::Strategy::Failure{};
  return strategy;
}

ir::PHandler handler(ir::HandlerKind kind, const std::string& id)
{
  auto result = std::make_shared<ir::Handler>();
  result->kind = kind;
  result->body = failureBody(id);
  return result;
}

Behaviour wrapped()
{
  Behaviour b;
  b.id = "wrapped";
  b.trigger = reference("trigger");
  b.abort = reference("abort");
  b.reset = reference("reset");
  b.triggerReply = reference("wrapped_reply");
  b.abortReply = reference("wrapped_abort_reply");
  b.resetReply = reference("wrapped_reset_reply");
  b.outcome = reference("wrapped_event");
  return b;
}

HandlerBodyCompiler fakeCompiler()
{
  return [](const ir::PStrategy& body, const PExpression&, const PExpression&, const PExpression&, Behaviour& result) -> VoidResult {
    result.id = body->id;
    result.triggerReply = reference(body->id + "_reply");
    result.abortReply = reference(body->id + "_abort_reply");
    result.resetReply = reference(body->id + "_reset_reply");
    result.outcome = reference(body->id + "_event");
    return {};
  };
}

TEST(NuXmvHandlerSemantics, RecoveryConsumesWrappedFailure)
{
  auto b = wrapped();
  ASSERT_TRUE(applyHandlers("owner", {handler(ir::HandlerKind::Error, "recovery")}, fakeCompiler(), b).IsSuccess());

  ASSERT_EQ(b.variables.size(), 1u);
  EXPECT_EQ(b.variables.front().domain, "{None, Recovering}");

  const auto trigger = toString(b.triggerReply);
  EXPECT_NE(trigger.find("RESULT_FAILURE"), std::string::npos);
  EXPECT_NE(trigger.find("recovery_reply"), std::string::npos);

  const auto outcome = toString(b.outcome);
  EXPECT_NE(outcome.find("wrapped_event"), std::string::npos);
  EXPECT_NE(outcome.find("recovery_event"), std::string::npos);
  EXPECT_NE(outcome.find("EVENT_SUCCESS"), std::string::npos);
}

TEST(NuXmvHandlerSemantics, AbortHandlerConsumesExternalAbort)
{
  auto b = wrapped();
  ASSERT_TRUE(applyHandlers("owner", {handler(ir::HandlerKind::Abort, "abort_handler")}, fakeCompiler(), b).IsSuccess());

  EXPECT_EQ(b.variables.front().domain, "{None, Aborting}");

  bool found = false;
  for (const auto& d : b.definitions)
  {
    if (d.name == "owner_abort_handler_trigger")
    {
      found = true;
      EXPECT_NE(toString(d.expression).find("abort"), std::string::npos);
    }
  }
  EXPECT_TRUE(found);
}

TEST(NuXmvHandlerSemantics, AbortHandlerConsumesInternalAbortedEvent)
{
  auto b = wrapped();
  ASSERT_TRUE(applyHandlers("owner", {handler(ir::HandlerKind::Abort, "abort_handler")}, fakeCompiler(), b).IsSuccess());

  bool found = false;
  for (const auto& d : b.definitions)
  {
    if (d.name == "owner_abort_handler_trigger")
    {
      found = true;
      const auto text = toString(d.expression);
      EXPECT_NE(text.find("wrapped_event"), std::string::npos);
      EXPECT_NE(text.find("EVENT_ABORTED"), std::string::npos);
    }
  }
  EXPECT_TRUE(found);
}

TEST(NuXmvHandlerSemantics, ErrorAndAbortHandlersCanCoexist)
{
  auto b = wrapped();
  ASSERT_TRUE(
      applyHandlers("owner", {handler(ir::HandlerKind::Error, "recovery"), handler(ir::HandlerKind::Abort, "abort_handler")}, fakeCompiler(), b).IsSuccess());

  EXPECT_EQ(b.variables.front().domain, "{None, Recovering, Aborting}");
}

TEST(NuXmvHandlerSemantics, EmitterHandlerIsExplicitlyUnsupported)
{
  auto b = wrapped();
  auto emitter = handler(ir::HandlerKind::Emitter, "emitter");
  EXPECT_FALSE(applyHandlers("owner", {emitter}, fakeCompiler(), b).IsSuccess());
}

}  // namespace
