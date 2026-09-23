#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_semantic_compiler.h"
#include "ast/koda_compiler.h"

namespace
{
using namespace koda;
using namespace koda::nuxmv;

CompilerOptions options()
{
  CompilerOptions o;
  o.outputDir = "./out";
  return o;
}

template <class T>
ir::PStrategy make(std::string id, T value)
{
  auto strategy = std::make_shared<ir::Strategy>();
  strategy->id = std::move(id);
  strategy->value = std::move(value);
  return strategy;
}

bool hasDefinition(const Behaviour& behaviour, const std::string& name)
{
  for (const auto& definition : behaviour.definitions)
    if (definition.name == name)
      return true;
  return false;
}

TEST(NuXmvMaterialization, CompositeUsesNamedChildInterface)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto opts = options();
  SemanticCompiler compiler(program, symbols, opts);

  ir::Strategy::Sequence sequence;
  sequence.items.push_back(make("first", ir::Strategy::Failure{}));
  sequence.items.push_back(make("second", ir::Strategy::Failure{}));

  Behaviour behaviour;
  ASSERT_TRUE(
      compiler.compileStrategy(make("sequence", std::move(sequence)), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  EXPECT_TRUE(hasDefinition(behaviour, "first_trigger_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "first_abort_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "first_reset_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "first_outcome"));

  EXPECT_TRUE(hasDefinition(behaviour, "second_trigger_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "second_outcome"));

  // The parent may contain its own case expressions, but child semantics must be
  // referenced by name rather than copied recursively into every occurrence.
  const auto triggerReply = toString(behaviour.triggerReply);
  EXPECT_NE(triggerReply.find("first_trigger_reply"), std::string::npos);
  EXPECT_NE(triggerReply.find("second_trigger_reply"), std::string::npos);
}

TEST(NuXmvMaterialization, NestedCompositionKeepsIntermediateBoundaries)
{
  ir::Program program;
  SymbolRegistry symbols;
  auto opts = options();
  SemanticCompiler compiler(program, symbols, opts);

  ir::Strategy::Sequence inner;
  inner.items.push_back(make("a", ir::Strategy::Failure{}));
  inner.items.push_back(make("b", ir::Strategy::Failure{}));

  ir::Strategy::Sequence outer;
  outer.items.push_back(make("inner", std::move(inner)));
  outer.items.push_back(make("c", ir::Strategy::Failure{}));

  Behaviour behaviour;
  ASSERT_TRUE(compiler.compileStrategy(make("outer", std::move(outer)), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());

  EXPECT_TRUE(hasDefinition(behaviour, "a_trigger_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "b_trigger_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "inner_trigger_reply"));
  EXPECT_TRUE(hasDefinition(behaviour, "c_trigger_reply"));

  // This is the key regression check: outer composition sees the inner
  // behaviour through its materialized interface, not its complete expression.
  EXPECT_NE(toString(behaviour.triggerReply).find("inner_trigger_reply"), std::string::npos);
}
}  // namespace
