#include <gtest/gtest.h>

#include "ast/emitters/nuxmv/nuxmv_semantic_compiler.h"
#include "ast/koda_compiler.h"

namespace
{
using namespace koda;
using namespace koda::nuxmv;

ir::PStrategy failure(std::string id)
{
  auto s = std::make_shared<ir::Strategy>();
  s->id = std::move(id);
  s->value = ir::Strategy::Failure{};
  return s;
}

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

TEST(NuXmvCoreComposites, RepeatCompiles)
{
  ir::Strategy::Repeat r;
  r.body = failure("body");
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("repeat", r), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_EQ(b.variables.front().domain, "{Idle, Running, Error}");
}

TEST(NuXmvCoreComposites, JoinCompilesFlat)
{
  ir::Strategy::Join j;
  j.items = {failure("a"), failure("b")};
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("join", j), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_EQ(b.variables.front().domain, "{Idle, Running, Error}");
  EXPECT_GE(b.nextAssignments.size(), 3u);
}

TEST(NuXmvCoreComposites, EitherCompilesFlat)
{
  ir::Strategy::Either e;
  e.items = {failure("a"), failure("b")};
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("either", e), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_EQ(b.variables.front().domain, "{Idle, Running, Error}");
}

TEST(NuXmvCoreComposites, WithinCompilesWithAbstractTimeout)
{
  ir::Strategy::Within w;
  w.seconds = 5;
  w.body = failure("body");
  w.fallback = failure("fallback");
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(make("within", w), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  ASSERT_GE(b.variables.size(), 3u);
  EXPECT_EQ(b.variables.front().name, "within_timeout");
}
}  // namespace
