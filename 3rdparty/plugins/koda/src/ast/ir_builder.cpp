#include "ir_builder.h"

#include <format>

#include "logging.h"

namespace koda
{
Result<ir::Program> IRBuilder::build(const System& system) const
{
  ir::Program program;
  for (const auto& component : system.components)
  {
    auto built = buildComponent(component);
    if (!built.IsSuccess())
      return Result<ir::Program>::Failed(built.ErrorMessage());
    program.components.push_back(built.Value());
  }
  return program;
}

Result<ir::Component> IRBuilder::buildComponent(const PComponent& component) const
{
  const auto ownerOpt = mSymbols.component(component->name);
  if (!ownerOpt)
    return Result<ir::Component>::Failed(std::format("Unknown component '{}'", component->name));

  const auto owner = *ownerOpt;

  ir::Component out;
  out.symbol = owner;
  out.kind = component->kind == Component::Kind::Task ? ir::ComponentKind::Task : ir::ComponentKind::Capability;
  out.name = component->name;
  out.span = component->span;

  for (const auto& arg : component->args)
    out.arguments.push_back(buildArg(arg, owner));

  for (const auto& statement : component->statements)
  {
    if (auto vars = std::get_if<PVarsBlock>(&statement->node); vars && *vars)
    {
      for (const auto& var : (*vars)->vars)
      {
        const auto id = mSymbols.lookupChild(owner, var->name);
        const auto* symbol = id ? mSymbols.get(*id) : nullptr;
        auto initial = buildExpr(var->init, owner);
        if (!initial.IsSuccess())
          return Result<ir::Component>::Failed(initial.ErrorMessage());

        auto fallback = buildExpr(var->fallback, owner);
        if (!fallback.IsSuccess())
          return Result<ir::Component>::Failed(fallback.ErrorMessage());

        out.variables.push_back(ir::Variable{
            .symbol = id.value_or(InvalidSymbol),
            .name = var->name,
            .type = symbol ? symbol->type : Type::Unknown(),
            .initial = initial.Value(),
            .fallback = fallback.Value(),
            .span = var->span,
        });
      }
    }
    else if (auto flows = std::get_if<PStrategyBlock>(&statement->node); flows && *flows)
    {
      for (const auto& flow : (*flows)->flows)
      {
        // auto flowSymbol = mSymbols.lookup(flow->name, owner);
        auto built = buildFlow(flow, owner);
        if (!built.IsSuccess())
          return Result<ir::Component>::Failed(built.ErrorMessage());

        out.flows.push_back(built.Value());
      }
    }
    else if (auto ros = std::get_if<PRosDef>(&statement->node); ros && *ros)
    {
      appendRosDef(*ros, owner, out);
    }
    else if (auto action = std::get_if<PActionDef>(&statement->node); action && *action)
    {
      out.title = (*action)->label1;
      out.message = (*action)->label2;
      for (const auto& ros : (*action)->rosDefs)
        appendRosDef(ros, owner, out);
    }
  }

  return out;
}

ir::Argument IRBuilder::buildArg(const koda::PArgument kodaArg, SymbolId owner) const
{
  const auto child = mSymbols.lookupChild(owner, kodaArg->b);
  const auto* symbol = child ? mSymbols.get(*child) : nullptr;

  ir::Argument argument;
  argument.symbol = child.value_or(InvalidSymbol);
  argument.type = symbol ? symbol->type : Type::Unknown();
  argument.name = kodaArg->b;
  argument.mode = ir::Argument::ModeFromKind(kodaArg->kind);
  argument.span = kodaArg->span;

  // LOG_DEBUG("Created argument %s with symbol: %u and type: %s, arg: %s", argument.name.c_str(), argument.symbol, argument.type.toString().c_str(), kodaArg->toString().c_str());

  return argument;
}

void IRBuilder::appendRosDef(const PRosDef& ros, SymbolId owner, ir::Component& out) const
{
  if (!ros || !ros->def)
    return;

  const auto id = mSymbols.lookupChild(owner, ros->def->name);
  const auto* symbol = id ? mSymbols.get(*id) : nullptr;

  ir::Event event;
  event.symbol = id.value_or(InvalidSymbol);
  event.name = ros->def->name;
  event.type = symbol ? symbol->type : Type::Unknown();
  event.span = ros->def->span;
  event.kind = ir::Event::fromRosKind(ros->kind);

  for (const auto& arg : ros->def->args)
    event.arguments.push_back(buildArg(arg, id.value_or(InvalidSymbol)));

  out.events.push_back(std::move(event));
}

Result<ir::Flow> IRBuilder::buildFlow(const PFlow& flow, SymbolId owner) const
{
  auto id = mSymbols.lookupChild(owner, flow->name).value_or(InvalidSymbol);
  if (id == InvalidSymbol)
    return Result<ir::Flow>::Failed(std::format("Unknown flow {} with owner {}", flow->name, owner));

  auto strategy = buildStrategy(flow->strategy, id);
  RETURN_ON_FAILURE_AS(strategy, ir::Flow);

  ir::Flow irflow;
  irflow.symbol = id;
  irflow.name = flow->name;
  irflow.strategy = strategy.Value();
  irflow.span = flow->span;

  for (const auto& arg : flow->args)
    irflow.arguments.push_back(buildArg(arg, id));

  return irflow;
}

Result<ir::PStrategy> IRBuilder::buildStrategy(const PStrategy& strategy, SymbolId owner) const
{
  if (!strategy)
    return Result<ir::PStrategy>::Failed("Invalid strategy");

  auto out = std::make_shared<ir::Strategy>();
  out->span = strategy->span;

  if (auto p = std::get_if<PSeq>(&strategy->v); p && *p)
  {
    ir::Strategy::Sequence x;
    for (const auto& child : (*p)->alts)
    {
      auto b = buildStrategy(child, owner);
      if (!b.IsSuccess())
        return b;

      x.items.push_back(b.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PJoin>(&strategy->v); p && *p)
  {
    ir::Strategy::Join x;
    for (const auto& child : (*p)->alts)
    {
      auto b = buildStrategy(child, owner);
      if (!b.IsSuccess())
        return b;

      x.items.push_back(b.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PEither>(&strategy->v); p && *p)
  {
    ir::Strategy::Either x;
    for (const auto& child : (*p)->alts)
    {
      auto b = buildStrategy(child, owner);
      if (!b.IsSuccess())
        return b;

      x.items.push_back(b.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PLet>(&strategy->v); p && *p)
  {
    auto call = buildCall((*p)->call, owner);
    if (!call.IsSuccess())
      return Result<ir::PStrategy>::Failed(call.ErrorMessage());

    out->value = ir::Strategy::Let{
        .symbol = mSymbols.lookupLocal((*p)->name, owner).value_or(InvalidSymbol),
        .call = call.Value(),
    };
  }
  else if (auto p = std::get_if<PWithin>(&strategy->v); p && *p)
  {
    auto a = buildStrategy((*p)->a, owner);
    if (!a.IsSuccess())
      return a;

    auto b = buildStrategy((*p)->b, owner);
    if (!b.IsSuccess())
      return b;

    ir::Strategy::Within x{(*p)->seconds, a.Value(), b.Value(), {}};
    for (const auto& h : (*p)->handlers)
    {
      auto bh = buildHandler(h, owner);
      if (!bh.IsSuccess())
        return Result<ir::PStrategy>::Failed(bh.ErrorMessage());

      x.handlers.push_back(bh.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PIfElse>(&strategy->v); p && *p)
  {
    auto c = buildExpr((*p)->cond, owner);
    if (!c.IsSuccess())
      return Result<ir::PStrategy>::Failed(c.ErrorMessage());

    auto a = buildStrategy((*p)->a, owner);
    if (!a.IsSuccess())
      return a;

    ir::PStrategy b;
    if ((*p)->b)
    {
      auto bb = buildStrategy((*p)->b, owner);
      if (!bb.IsSuccess())
        return bb;

      b = bb.Value();
    }
    out->value = ir::Strategy::IfElse{c.Value(), a.Value(), b};
  }
  else if (auto p = std::get_if<PRepeat>(&strategy->v); p && *p)
  {
    auto body = buildStrategy((*p)->a, owner);
    if (!body.IsSuccess())
      return body;

    ir::Strategy::Repeat x{(*p)->seconds, (*p)->iterations, body.Value(), {}};
    for (const auto& h : (*p)->handlers)
    {
      auto bh = buildHandler(h, owner);
      if (!bh.IsSuccess())
        return Result<ir::PStrategy>::Failed(bh.ErrorMessage());

      x.handlers.push_back(bh.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PGuard>(&strategy->v); p && *p)
  {
    auto c = buildExpr((*p)->cond, owner);
    if (!c.IsSuccess())
      return Result<ir::PStrategy>::Failed(c.ErrorMessage());

    out->value = ir::Strategy::Guard{c.Value()};
  }
  else if (std::holds_alternative<PEnd>(strategy->v))
    out->value = ir::Strategy::End{};
  else if (std::holds_alternative<PContinue>(strategy->v))
    out->value = ir::Strategy::Continue{};
  else if (std::holds_alternative<PRef>(strategy->v))
  {
    auto it = mSemantics.flowRefs.find(strategy.get());
    if (it == mSemantics.flowRefs.end())
      return Result<ir::PStrategy>::Failed("Unresolved flow reference");

    out->value = ir::Strategy::FlowRef{it->second};
  }
  else if (auto p = std::get_if<PTaskCall>(&strategy->v); p && *p)
  {
    auto call = buildCall((*p)->call, owner);
    if (!call.IsSuccess())
      return Result<ir::PStrategy>::Failed(call.ErrorMessage());

    ir::Strategy::TaskCall x{call.Value(), {}};
    for (const auto& h : (*p)->handlers)
    {
      auto bh = buildHandler(h, owner);
      if (!bh.IsSuccess())
        return Result<ir::PStrategy>::Failed(bh.ErrorMessage());

      x.handlers.push_back(bh.Value());
    }
    out->value = std::move(x);
  }
  else if (auto p = std::get_if<PParen>(&strategy->v); p && *p)
  {
    return buildStrategy((*p)->a, owner);
  }
  else
    return Result<ir::PStrategy>::Failed("Unsupported strategy node");

  return out;
}

Result<ir::PHandler> IRBuilder::buildHandler(const PStrategyHandler& handler, SymbolId owner) const
{
  auto out = std::make_shared<ir::Handler>();
  out->span = handler->span;
  switch (handler->kind)
  {
    case StrategyHandler::Kind::OnError:
      out->kind = ir::HandlerKind::Error;
      break;
    case StrategyHandler::Kind::OnAbort:
      out->kind = ir::HandlerKind::Abort;
      break;
    case StrategyHandler::Kind::OnEmitter:
      out->kind = ir::HandlerKind::Emitter;
      break;
    case StrategyHandler::Kind::OnEmitterContinue:
      out->kind = ir::HandlerKind::EmitterContinue;
      break;
    default:
      break;
  }

  if (handler->emitter)
  {
    auto c = buildCall(handler->emitter, owner);
    if (!c.IsSuccess())
      return Result<ir::PHandler>::Failed(c.ErrorMessage());

    out->emitter = c.Value();
  }

  if (handler->body)
  {
    auto b = buildStrategy(handler->body, owner);
    if (!b.IsSuccess())
      return Result<ir::PHandler>::Failed(b.ErrorMessage());

    out->body = b.Value();
  }

  return out;
}

Result<ir::Call> IRBuilder::buildCall(const PEventCall& call,
                                      SymbolId owner) const
{
  auto it = mSemantics.calls.find(call.get());
  if (it == mSemantics.calls.end())
    return Result<ir::Call>::Failed(std::format("Unresolved call at {}", call->span.toString()));

  ir::Call out;
  out.kind = it->second.kind == ResolvedCallKind::CapabilityTrigger
                 ? ir::CallKind::CapabilityTrigger
                 : ir::CallKind::Event;
  out.receiver = it->second.receiver;
  out.target = it->second.target;
  out.span = call->span;
  for (const auto& arg : call->args)
  {
    auto e = buildExpr(arg, owner);
    if (!e.IsSuccess())
      return Result<ir::Call>::Failed(e.ErrorMessage());

    out.arguments.push_back(e.Value());
  }

  return out;
}

Result<ir::PExpression> IRBuilder::buildExpr(const PExpr& expr, SymbolId owner) const
{
  if (!expr)
    return Result<ir::PExpression>::Failed("Invalid expression");

  auto out = std::make_shared<ir::Expression>();
  out->span = expr->span;
  auto typeIt = mSemantics.expressionTypes.find(expr.get());
  out->type = typeIt == mSemantics.expressionTypes.end() ? Type::Unknown()
                                                         : typeIt->second;

  if (auto p = std::get_if<PStr>(&expr->v); p && *p)
    out->value = ir::Expression::Literal{(*p)->value, Type::String()};
  else if (auto p = std::get_if<PInt>(&expr->v); p && *p)
    out->value = ir::Expression::Literal{std::to_string((*p)->value), Type::Int()};
  else if (auto p = std::get_if<PFloat>(&expr->v); p && *p)
    out->value = ir::Expression::Literal{std::to_string((*p)->value), Type::Float()};
  else if (auto p = std::get_if<PId>(&expr->v); p && *p)
  {
    auto id = mSymbols.lookup((*p)->value, owner);
    if (!id)
      return Result<ir::PExpression>::Failed(std::format("Unresolved identifier '{}' with owner {}", (*p)->value, owner));

    out->value = ir::Expression::Reference{*id};
  }
  else if (auto p = std::get_if<PCall>(&expr->v); p && *p)
  {
    auto c = buildCall((*p)->value, owner);
    if (!c.IsSuccess())
      return Result<ir::PExpression>::Failed(c.ErrorMessage());

    out->value = ir::Expression::CallExpr{c.Value()};
  }
  else if (auto p = std::get_if<PNeg>(&expr->v); p && *p)
  {
    auto v = buildExpr((*p)->value, owner);
    if (!v.IsSuccess())
      return v;

    out->value = ir::Expression::Unary{"-", v.Value()};
  }
  else if (auto p = std::get_if<PNot>(&expr->v); p && *p)
  {
    auto v = buildExpr((*p)->value, owner);
    if (!v.IsSuccess())
      return v;

    out->value = ir::Expression::Unary{"!", v.Value()};
  }
  else if (auto p = std::get_if<PBinOp>(&expr->v); p && *p)
  {
    auto a = buildExpr((*p)->a, owner);
    if (!a.IsSuccess())
      return a;

    ir::PExpression b;
    if ((*p)->b)
    {
      auto bb = buildExpr((*p)->b, owner);
      if (!bb.IsSuccess())
        return bb;

      b = bb.Value();
    }

    std::string op;
    switch ((*p)->operation)
    {
      case Expr::BinOp::Kind::Equal:
        op = "=";
        break;
      case Expr::BinOp::Kind::NotEqual:
        op = "!=";
        break;
      case Expr::BinOp::Kind::GreaterThan:
        op = ">";
        break;
      case Expr::BinOp::Kind::GreaterEqual:
        op = ">=";
        break;
      case Expr::BinOp::Kind::LessThan:
        op = "<";
        break;
      case Expr::BinOp::Kind::LessEqual:
        op = "<=";
        break;
      case Expr::BinOp::Kind::Addition:
        op = "+";
        break;
      case Expr::BinOp::Kind::Subtraction:
        op = "-";
        break;
      case Expr::BinOp::Kind::Multiplication:
        op = "*";
        break;
      case Expr::BinOp::Kind::Division:
        op = "/";
        break;
      case Expr::BinOp::Kind::Disjunction:
        op = "||";
        break;
      case Expr::BinOp::Kind::Conjunction:
        op = "&&";
        break;
      case Expr::BinOp::Kind::Negation:
        op = "!";
        break;
      case Expr::BinOp::Kind::Unary:
        op = "-";
        break;
      default:
        op = "?";
        break;
    }

    if (!b)
      out->value = ir::Expression::Unary{op, a.Value()};
    else
      out->value = ir::Expression::Binary{op, a.Value(), b};
  }
  else if (auto p = std::get_if<PEParen>(&expr->v); p && *p)
    return buildExpr((*p)->value, owner);
  else
    return Result<ir::PExpression>::Failed("Unsupported expression node");

  return out;
}

}  // namespace koda
