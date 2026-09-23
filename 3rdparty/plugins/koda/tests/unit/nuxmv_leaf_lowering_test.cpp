#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_constants.h"
#include "ast/emitters/nuxmv/nuxmv_semantic_compiler.h"
#include "ast/koda_compiler.h"

namespace
{
using namespace koda;
using namespace koda::nuxmv;

ir::PStrategy makeFailure()
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = "failure";
  strategy->value = ir::Strategy::Failure{};
  return strategy;
}

ir::PStrategy makeCall(SymbolId target, const std::string& id)
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = id;

  ir::Strategy::Call value;
  value.call.kind = ir::CallKind::Event;
  value.call.target = target;
  strategy->value = value;
  return strategy;
}

ir::Program programWithEvent(SymbolId eventId, ir::EventKind kind)
{
  ir::Program program;
  ir::Component capability;
  capability.kind = ir::ComponentKind::Capability;
  capability.name = "drive";

  ir::Action action;
  ir::Event event;
  event.symbol = eventId;
  event.kind = kind;
  event.name = kind == ir::EventKind::Abort ? "abort" : "trigger";
  action.events.push_back(event);
  capability.actions.push_back(action);

  program.components.push_back(capability);
  return program;
}

CompilerOptions options()
{
  CompilerOptions value;
  value.outputDir = "./out";
  return value;
}

TEST(NuXmvLeafLowering, FailureHasExpectedInterface)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);
  Behaviour behaviour;

  ASSERT_TRUE(compiler.compileStrategy(makeFailure(), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  EXPECT_EQ(behaviour.id, "failure");
  ASSERT_EQ(behaviour.variables.size(), 1u);
  EXPECT_EQ(behaviour.variables.front().domain, "{Idle, Error}");
  EXPECT_EQ(toString(behaviour.outcome), "EVENT_NONE");
  ASSERT_EQ(behaviour.nextAssignments.size(), 1u);
  ASSERT_EQ(behaviour.constraints.size(), 1u);

  const auto reply = toString(behaviour.triggerReply);
  EXPECT_NE(reply.find("RESULT_FAILURE"), std::string::npos);
}

TEST(NuXmvLeafLowering, ActionCreatesGenericActionProtocol)
{
  constexpr SymbolId eventId = 42;
  auto program = programWithEvent(eventId, ir::EventKind::Trigger);
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);
  Behaviour behaviour;

  ASSERT_TRUE(compiler.compileStrategy(makeCall(eventId, "drive"), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  EXPECT_EQ(behaviour.id, "drive");
  ASSERT_EQ(behaviour.variables.size(), 5u);
  EXPECT_EQ(behaviour.variables[0].name, "drive_trigger_result");
  EXPECT_EQ(behaviour.variables[3].name, "drive_event");
  EXPECT_EQ(behaviour.variables[4].domain, "{Idle, Running, Error}");
  ASSERT_EQ(behaviour.nextAssignments.size(), 1u);
  ASSERT_EQ(behaviour.constraints.size(), 1u);

  const auto triggerReply = toString(behaviour.triggerReply);
  EXPECT_NE(triggerReply.find("drive_trigger_result"), std::string::npos);

  const auto outcome = toString(behaviour.outcome);
  EXPECT_NE(outcome.find("drive_event"), std::string::npos);
}

TEST(NuXmvLeafLowering, AbortEventBecomesAbortCall)
{
  constexpr SymbolId eventId = 84;
  auto program = programWithEvent(eventId, ir::EventKind::Abort);
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);
  Behaviour behaviour;

  ASSERT_TRUE(compiler.compileStrategy(makeCall(eventId, "stop_drive"), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  EXPECT_EQ(behaviour.id, "stop_drive");
  ASSERT_EQ(behaviour.variables.size(), 2u);
  EXPECT_EQ(behaviour.variables[0].name, "stop_drive_abort_result");
  EXPECT_EQ(behaviour.variables[1].domain, "{Idle, Error}");
  EXPECT_EQ(toString(behaviour.outcome), "EVENT_NONE");

  const auto triggerReply = toString(behaviour.triggerReply);
  EXPECT_NE(triggerReply.find("RESULT_DONE"), std::string::npos);
  EXPECT_NE(triggerReply.find("RESULT_RUNNING"), std::string::npos);
  EXPECT_NE(triggerReply.find("RESULT_FAILURE"), std::string::npos);
}

TEST(NuXmvLeafLowering, CompositeIsRejectedAtLeafStage)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);

  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = "sequence";
  strategy->value = ir::Strategy::Sequence{};

  Behaviour behaviour;
  EXPECT_FALSE(compiler.compileStrategy(strategy, reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());
}

}  // namespace
