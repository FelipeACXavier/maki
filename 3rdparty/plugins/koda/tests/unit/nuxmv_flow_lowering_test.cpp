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

ir::PStrategy flowCall(SymbolId target, std::vector<ir::PHandler> handlers = {})
{
  ir::Strategy::Call c;
  c.call.kind = ir::CallKind::Flow;
  c.call.target = target;
  c.handlers = std::move(handlers);
  return make("flow_call", std::move(c));
}

TEST(NuXmvFlowLowering, ResolvesTargetBySymbol)
{
  constexpr SymbolId S = 42;
  ir::Flow f;
  f.symbol = S;
  f.name = "child";
  f.strategy = make("body", ir::Strategy::Failure{});
  ir::Component component;
  component.flows.push_back(f);
  ir::Program p;
  p.components.push_back(component);
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;

  const auto ret = c.compileStrategy(flowCall(S), reference("trigger"), reference("abort"), reference("reset"), b);
  ASSERT_TRUE(ret.IsSuccess()) << ret.ErrorMessage();
  EXPECT_NE(toString(b.triggerReply).find("RESULT_FAILURE"), std::string::npos);
}

TEST(NuXmvFlowLowering, RejectsUnknownTarget)
{
  ir::Program p;
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  EXPECT_FALSE(c.compileStrategy(flowCall(999), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
}

TEST(NuXmvFlowLowering, SameFlowCanBeInlinedMoreThanOnce)
{
  constexpr SymbolId S = 7;
  ir::Flow f;
  f.symbol = S;
  f.name = "child";
  f.strategy = make("body", ir::Strategy::Failure{});
  ir::Component component;
  component.flows.push_back(f);
  ir::Program p;
  p.components.push_back(component);
  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ir::Strategy::Sequence seq;
  seq.items = {flowCall(S), flowCall(S)};

  ASSERT_TRUE(c.compileStrategy(make("sequence", std::move(seq)), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());
  EXPECT_FALSE(b.id.empty());
}

TEST(NuXmvFlowLowering, CallHandlersWrapInlinedFlow)
{
  constexpr SymbolId S = 13;
  ir::Flow f;
  f.symbol = S;
  f.name = "child";
  f.strategy = make("body", ir::Strategy::Failure{});
  ir::Component component;
  component.flows.push_back(f);
  ir::Program p;
  p.components.push_back(component);

  auto recovery = std::make_shared<ir::Handler>();
  recovery->kind = ir::HandlerKind::Error;
  recovery->body = make("recovery", ir::Strategy::Failure{});

  SymbolRegistry sy;
  auto o = options();
  SemanticCompiler c(p, sy, o);
  Behaviour b;
  ASSERT_TRUE(c.compileStrategy(flowCall(S, {recovery}), reference("trigger"), reference("abort"), reference("reset"), b).IsSuccess());

  bool found = false;
  for (const auto& v : b.variables)
    if (v.domain.find("Recovering") != std::string::npos)
      found = true;
  EXPECT_TRUE(found);
}
}  // namespace
