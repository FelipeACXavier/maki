#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_semantic_compiler.h"
#include "ast/koda_compiler.h"

namespace
{
using namespace koda;
using namespace koda::nuxmv;

ir::PStrategy failure(const std::string& id)
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = id;
  strategy->value = ir::Strategy::Failure{};
  return strategy;
}

ir::PStrategy sequence(std::vector<ir::PStrategy> items)
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = "sequence";

  ir::Strategy::Sequence value;
  value.items = std::move(items);
  strategy->value = std::move(value);
  return strategy;
}

CompilerOptions options()
{
  CompilerOptions result;
  result.outputDir = "./out";
  return result;
}

TEST(NuXmvSequenceLowering, RejectsEmptySequence)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);

  Behaviour behaviour;
  EXPECT_FALSE(compiler.compileStrategy(sequence({}), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());
}

TEST(NuXmvSequenceLowering, ProducesFlatCompositeState)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);

  Behaviour behaviour;
  ASSERT_TRUE(
      compiler.compileStrategy(sequence({failure("a"), failure("b")}), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  ASSERT_GE(behaviour.variables.size(), 3u);
  EXPECT_EQ(behaviour.variables.front().name, "sequence_state");
  EXPECT_EQ(behaviour.variables.front().domain, "{Idle, Error, Child0, Child1}");

  // Sequence + two child state machines are flattened into one Behaviour.
  EXPECT_EQ(behaviour.nextAssignments.size(), 3u);
}

TEST(NuXmvSequenceLowering, CreatesSynchronousChildTriggerChain)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);

  Behaviour behaviour;
  ASSERT_TRUE(
      compiler.compileStrategy(sequence({failure("a"), failure("b")}), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  bool foundChild0 = false;
  bool foundChild1 = false;

  for (const auto& definition : behaviour.definitions)
  {
    if (definition.name == "sequence_child0_trigger")
    {
      foundChild0 = true;
      const auto expression = toString(definition.expression);
      EXPECT_NE(expression.find("trigger"), std::string::npos);
    }

    if (definition.name == "sequence_child1_trigger")
    {
      foundChild1 = true;
      const auto expression = toString(definition.expression);
      EXPECT_NE(expression.find("RESULT_DONE"), std::string::npos);
    }
  }

  EXPECT_TRUE(foundChild0);
  EXPECT_TRUE(foundChild1);
}

TEST(NuXmvSequenceLowering, TriggerReplyContainsDoneClosure)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto compilerOptions = options();
  SemanticCompiler compiler(program, symbols, compilerOptions);

  Behaviour behaviour;
  ASSERT_TRUE(
      compiler.compileStrategy(sequence({failure("a"), failure("b")}), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  const auto reply = toString(behaviour.triggerReply);
  EXPECT_NE(reply.find("RESULT_DONE"), std::string::npos);
  EXPECT_NE(reply.find("a_state"), std::string::npos);
  EXPECT_NE(reply.find("b_state"), std::string::npos);
}

}  // namespace
