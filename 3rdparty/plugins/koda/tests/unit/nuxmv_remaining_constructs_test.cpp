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
  auto s = std::make_shared<ir::Strategy>();
  s->id = std::move(id);
  s->value = std::move(value);
  return s;
}

ir::PStrategy failure(std::string id)
{
  return make(std::move(id), ir::Strategy::Failure{});
}

TEST(NuXmvRemainingConstructs, EndIsSynchronous)
{
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("end", ir::Strategy::End{}), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_TRUE(b.variables.empty());
  EXPECT_NE(toString(b.triggerReply).find("RESULT_DONE"), std::string::npos);
}

TEST(NuXmvRemainingConstructs, ContinueIsSynchronous)
{
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("continue", ir::Strategy::Continue{}), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_TRUE(b.variables.empty());
  EXPECT_NE(toString(b.triggerReply).find("RESULT_DONE"), std::string::npos);
}

TEST(NuXmvRemainingConstructs, ChooseUsesOrderedConditions)
{
  ir::Strategy::Choose choose;
  ir::Strategy::Choose::When a;
  a.strategy = failure("a");
  ir::Strategy::Choose::When b;
  b.strategy = failure("b");
  choose.options = {a, b};
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour behaviour;
  ASSERT_TRUE(c.compileStrategy(make("choose", choose), reference("trigger"), reference("abort"), reference("reset"), behaviour).IsSuccess());
  EXPECT_EQ(behaviour.variables.front().domain, "{Idle, Error, Option0, Option1}");
  EXPECT_EQ(behaviour.variables[1].name, "choose_condition0");
  EXPECT_EQ(behaviour.variables[2].name, "choose_condition1");
}
}  // namespace
