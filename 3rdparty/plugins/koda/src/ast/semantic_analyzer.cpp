#include "semantic_analyzer.h"

#include <format>

#include "logging.h"

namespace koda
{
namespace
{
bool compatible(const Type& expected, const Type& actual)
{
  if (!expected.valid() || !actual.valid())
    return true;
  if (expected.kind == actual.kind)
    return true;
  return expected.isNumeric() && actual.isNumeric();
}
}  // namespace

VoidResult SemanticAnalyzer::run(const System& system)
{
  mModel = {};
  auto signatures = collectEventSignatures(system);
  if (!signatures.IsSuccess())
    return signatures;

  for (const auto& component : system.components)
  {
    auto result = analyzeComponent(component);
    if (!result.IsSuccess())
      return result;
  }
  return VoidResult{};
}

VoidResult SemanticAnalyzer::collectEventSignatures(const System& system)
{
  for (const auto& component : system.components)
  {
    const auto owner = mSymbols.component(component->name);
    if (!owner)
      continue;

    for (const auto& statement : component->statements)
    {
      if (auto ros = std::get_if<PRosDef>(&statement->node); ros && *ros)
      {
        auto result = collectRosSignature(*ros, *owner);
        if (!result.IsSuccess())
          return result;
      }
      else if (auto action = std::get_if<PActionDef>(&statement->node); action && *action)
      {
        for (const auto& ros : (*action)->rosDefs)
        {
          auto result = collectRosSignature(ros, *owner);
          if (!result.IsSuccess())
            return result;
        }
      }
    }
  }
  return VoidResult{};
}

VoidResult SemanticAnalyzer::collectRosSignature(const PRosDef& ros, SymbolId owner)
{
  if (!ros || !ros->def)
    return VoidResult{};

  const auto event = mSymbols.lookupChild(owner, ros->def->name);
  if (!event)
    return VoidResult::Failed(std::format("Unknown event '{}'", ros->def->name));

  std::vector<Type> args;
  for (const auto& arg : ros->def->args)
    args.push_back(mSymbols.resolveType(arg->a).value_or(Type::Unknown()));

  mModel.eventArguments[*event] = std::move(args);
  return VoidResult{};
}

VoidResult SemanticAnalyzer::analyzeComponent(const PComponent& component)
{
  const auto owner = mSymbols.component(component->name);
  if (!owner)
    return VoidResult::Failed(std::format("Unknown component '{}'", component->name));

  // Resolve argument types now that pass 1 has registered all components.
  for (const auto& arg : component->args)
  {
    auto symbolId = mSymbols.lookupChild(*owner, arg->a);
    if (!symbolId)
      continue;
    if (auto* symbol = mSymbols.get(*symbolId))
      symbol->type = mSymbols.resolveType(arg->b).value_or(Type::Unknown());
  }

  for (const auto& statement : component->statements)
  {
    auto result = analyzeStatement(statement, *owner);
    if (!result.IsSuccess())
      return result;
  }
  return VoidResult{};
}

VoidResult SemanticAnalyzer::analyzeStatement(const PStatement& statement, SymbolId owner)
{
  if (auto vars = std::get_if<PVarsBlock>(&statement->node); vars && *vars)
  {
    for (const auto& var : (*vars)->vars)
    {
      auto init = analyzeExpr(var->init, owner);
      if (!init.IsSuccess())
        return VoidResult::Failed(init.ErrorMessage());
      auto fallback = analyzeExpr(var->fallback, owner);
      if (!fallback.IsSuccess())
        return VoidResult::Failed(fallback.ErrorMessage());

      const auto symbolId = mSymbols.lookupChild(owner, var->name);
      const auto* symbol = symbolId ? mSymbols.get(*symbolId) : nullptr;
      if (symbol && !compatible(symbol->type, init.Value()))
        return VoidResult::Failed(std::format("Initializer for '{}' has incompatible type at {}", var->name, var->span.toString()));
      if (symbol && !compatible(symbol->type, fallback.Value()))
        return VoidResult::Failed(std::format("Fallback for '{}' has incompatible type at {}", var->name, var->span.toString()));
    }
  }
  else if (auto block = std::get_if<PStrategyBlock>(&statement->node); block && *block)
  {
    for (const auto& flow : (*block)->flows)
    {
      auto flowSymbol = mSymbols.lookup(flow->name, owner);
      auto result = analyzeStrategy(flow->strategy, flowSymbol ? *flowSymbol : owner);
      if (!result.IsSuccess())
        return result;
    }
  }
  return VoidResult{};
}

VoidResult SemanticAnalyzer::analyzeStrategy(const PStrategy& strategy, SymbolId owner)
{
  if (!strategy)
    return VoidResult{};

  if (auto p = std::get_if<PSeq>(&strategy->v); p && *p)
  {
    for (const auto& child : (*p)->alts)
    {
      auto r = analyzeStrategy(child, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PJoin>(&strategy->v); p && *p)
  {
    for (const auto& child : (*p)->alts)
    {
      auto r = analyzeStrategy(child, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PEither>(&strategy->v); p && *p)
  {
    for (const auto& child : (*p)->alts)
    {
      auto r = analyzeStrategy(child, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PLet>(&strategy->v); p && *p)
  {
    auto call = resolveCall((*p)->call, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed(call.ErrorMessage());
    mModel.calls[(*p)->call.get()] = call.Value();

    // Koda currently has component-level flow scope; locals are registered here so later
    // sequence elements can resolve them. If lexical blocks are added, make ScopeId separate from SymbolId.
    if (!mSymbols.lookupLocal((*p)->name, owner))
    {
      auto local = mSymbols.declare(SymbolKind::Local, (*p)->name, call.Value().returnType, strategy->span, owner);
      if (!local.IsSuccess())
        return VoidResult::Failed(local.ErrorMessage());
    }
  }
  else if (auto p = std::get_if<PWithin>(&strategy->v); p && *p)
  {
    auto a = analyzeStrategy((*p)->a, owner);
    if (!a.IsSuccess())
      return a;
    auto b = analyzeStrategy((*p)->b, owner);
    if (!b.IsSuccess())
      return b;
    for (const auto& h : (*p)->handlers)
    {
      auto r = analyzeHandler(h, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PIfElse>(&strategy->v); p && *p)
  {
    auto cond = analyzeExpr((*p)->cond, owner);
    if (!cond.IsSuccess())
      return VoidResult::Failed(cond.ErrorMessage());
    if (cond.Value().kind != TypeKind::Bool && cond.Value().kind != TypeKind::Unknown)
      return VoidResult::Failed(std::format("if condition must be boolean at {}", strategy->span.toString()));
    auto a = analyzeStrategy((*p)->a, owner);
    if (!a.IsSuccess())
      return a;
    if ((*p)->b)
    {
      auto b = analyzeStrategy((*p)->b, owner);
      if (!b.IsSuccess())
        return b;
    }
  }
  else if (auto p = std::get_if<PRepeat>(&strategy->v); p && *p)
  {
    auto a = analyzeStrategy((*p)->a, owner);
    if (!a.IsSuccess())
      return a;
    for (const auto& h : (*p)->handlers)
    {
      auto r = analyzeHandler(h, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PGuard>(&strategy->v); p && *p)
  {
    auto cond = analyzeExpr((*p)->cond, owner);
    if (!cond.IsSuccess())
      return VoidResult::Failed(cond.ErrorMessage());
    if (cond.Value().kind != TypeKind::Bool && cond.Value().kind != TypeKind::Unknown)
      return VoidResult::Failed(std::format("guard condition must be boolean at {}", strategy->span.toString()));
  }
  else if (auto p = std::get_if<PRef>(&strategy->v); p && *p)
  {
    auto id = mSymbols.lookup((*p)->name, owner);
    const auto* symbol = id ? mSymbols.get(*id) : nullptr;
    if (!symbol || symbol->kind != SymbolKind::Flow)
      return VoidResult::Failed(std::format("Unknown flow '{}' at {}", (*p)->name, strategy->span.toString()));
    mModel.flowRefs[strategy.get()] = *id;
  }
  else if (auto p = std::get_if<PTaskCall>(&strategy->v); p && *p)
  {
    auto call = resolveCall((*p)->call, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed(call.ErrorMessage());
    mModel.calls[(*p)->call.get()] = call.Value();
    for (const auto& h : (*p)->handlers)
    {
      auto r = analyzeHandler(h, owner);
      if (!r.IsSuccess())
        return r;
    }
  }
  else if (auto p = std::get_if<PParen>(&strategy->v); p && *p)
  {
    return analyzeStrategy((*p)->a, owner);
  }

  return VoidResult{};
}

VoidResult SemanticAnalyzer::analyzeHandler(const PStrategyHandler& handler, SymbolId owner)
{
  if (!handler)
    return VoidResult{};
  if (handler->emitter)
  {
    auto call = resolveCall(handler->emitter, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed(call.ErrorMessage());
    mModel.calls[handler->emitter.get()] = call.Value();
  }
  if (handler->body)
    return analyzeStrategy(handler->body, owner);
  return VoidResult{};
}

Result<ResolvedCall> SemanticAnalyzer::resolveCall(const PEventCall& call, SymbolId owner)
{
  if (!call)
    return Result<ResolvedCall>::Failed("Invalid event call");

  if (call->receiver.empty())
  {
    auto receiverResult = resolveValue(call->name, owner, call->span);
    if (!receiverResult.IsSuccess())
      return Result<ResolvedCall>::Failed(receiverResult.ErrorMessage());
    const auto receiver = receiverResult.Value();
    const auto* receiverSymbol = mSymbols.get(receiver);
    if (!receiverSymbol)
      return Result<ResolvedCall>::Failed("Invalid call receiver");

    auto componentResult = resolveComponentType(*receiverSymbol, call->span);
    if (!componentResult.IsSuccess())
      return Result<ResolvedCall>::Failed(componentResult.ErrorMessage());

    // Unqualified invocation means the capability's trigger/default action.
    for (const auto& arg : call->args)
    {
      auto type = analyzeExpr(arg, owner);
      if (!type.IsSuccess())
        return Result<ResolvedCall>::Failed(type.ErrorMessage());
    }
    return ResolvedCall{ResolvedCallKind::CapabilityTrigger, receiver, componentResult.Value(), Type::Void()};
  }

  auto receiverResult = resolveValue(call->receiver, owner, call->span);
  if (!receiverResult.IsSuccess())
    return Result<ResolvedCall>::Failed(receiverResult.ErrorMessage());
  const auto receiver = receiverResult.Value();
  const auto* receiverSymbol = mSymbols.get(receiver);
  if (!receiverSymbol)
    return Result<ResolvedCall>::Failed("Invalid call receiver");

  auto componentResult = resolveComponentType(*receiverSymbol, call->span);
  if (!componentResult.IsSuccess())
    return Result<ResolvedCall>::Failed(componentResult.ErrorMessage());
  const auto component = componentResult.Value();

  const auto event = mSymbols.lookupChild(component, call->name);
  const auto* eventSymbol = event ? mSymbols.get(*event) : nullptr;
  if (!eventSymbol || eventSymbol->kind != SymbolKind::Event)
    return Result<ResolvedCall>::Failed(std::format("Component '{}' has no event '{}' at {}", mSymbols.get(component)->name, call->name, call->span.toString()));

  const auto signature = mModel.eventArguments.find(*event);
  if (signature != mModel.eventArguments.end() && signature->second.size() != call->args.size())
    return Result<ResolvedCall>::Failed(std::format("Event '{}.{}' expects {} arguments, got {} at {}", call->receiver, call->name, signature->second.size(), call->args.size(), call->span.toString()));

  for (std::size_t i = 0; i < call->args.size(); ++i)
  {
    auto actual = analyzeExpr(call->args[i], owner);
    if (!actual.IsSuccess())
      return Result<ResolvedCall>::Failed(actual.ErrorMessage());
    if (signature != mModel.eventArguments.end() && !compatible(signature->second[i], actual.Value()))
      return Result<ResolvedCall>::Failed(std::format("Argument {} of '{}.{}' has incompatible type at {}. Expected {} but got {}", i + 1, call->receiver, call->name, call->args[i]->span.toString(), signature->second[i].toString(), actual.Value().toString()));
  }

  return ResolvedCall{ResolvedCallKind::Event, receiver, *event, eventSymbol->type};
}

Result<Type> SemanticAnalyzer::analyzeExpr(const PExpr& expr, SymbolId owner)
{
  if (!expr)
    return Type::Unknown();
  Type type = Type::Unknown();

  if (std::holds_alternative<PStr>(expr->v))
    type = Type::String();
  else if (std::holds_alternative<PInt>(expr->v))
    type = Type::Int();
  else if (std::holds_alternative<PFloat>(expr->v))
    type = Type::Float();
  else if (auto p = std::get_if<PId>(&expr->v); p && *p)
  {
    auto id = resolveValue((*p)->value, owner, expr->span);
    if (!id.IsSuccess())
      return Result<Type>::Failed(id.ErrorMessage());
    const auto* symbol = mSymbols.get(id.Value());
    type = symbol ? symbol->type : Type::Unknown();
  }
  else if (auto p = std::get_if<PCall>(&expr->v); p && *p)
  {
    auto call = resolveCall((*p)->value, owner);
    if (!call.IsSuccess())
      return Result<Type>::Failed(call.ErrorMessage());
    mModel.calls[(*p)->value.get()] = call.Value();
    type = call.Value().returnType;
  }
  else if (auto p = std::get_if<PNeg>(&expr->v); p && *p)
  {
    auto inner = analyzeExpr((*p)->value, owner);
    if (!inner.IsSuccess())
      return inner;
    if (!inner.Value().isNumeric() && inner.Value().kind != TypeKind::Unknown)
      return Result<Type>::Failed(std::format("Unary '-' requires a numeric operand at {}", expr->span.toString()));
    type = inner.Value();
  }
  else if (auto p = std::get_if<PNot>(&expr->v); p && *p)
  {
    auto inner = analyzeExpr((*p)->value, owner);
    if (!inner.IsSuccess())
      return inner;
    type = Type::Bool();
  }
  else if (auto p = std::get_if<PBinOp>(&expr->v); p && *p)
  {
    auto lhs = analyzeExpr((*p)->a, owner);
    if (!lhs.IsSuccess())
      return lhs;
    Type rhsType = Type::Unknown();
    if ((*p)->b)
    {
      auto rhs = analyzeExpr((*p)->b, owner);
      if (!rhs.IsSuccess())
        return rhs;
      rhsType = rhs.Value();
    }

    using K = Expr::BinOp::Kind;
    switch ((*p)->operation)
    {
      case K::Equal:
      case K::NotEqual:
      case K::GreaterThan:
      case K::GreaterEqual:
      case K::LessThan:
      case K::LessEqual:
      case K::Disjunction:
      case K::Conjunction:
        type = Type::Bool();
        break;
      case K::Addition:
      case K::Subtraction:
      case K::Multiplication:
      case K::Division:
        if ((!lhs.Value().isNumeric() || !rhsType.isNumeric()) && lhs.Value().kind != TypeKind::Unknown && rhsType.kind != TypeKind::Unknown)
          return Result<Type>::Failed(std::format("Arithmetic operator requires numeric operands at {}", expr->span.toString()));
        type = lhs.Value().kind == TypeKind::Float || rhsType.kind == TypeKind::Float ? Type::Float() : Type::Int();
        break;
      default:
        type = lhs.Value();
        break;
    }
  }
  else if (auto p = std::get_if<PEParen>(&expr->v); p && *p)
  {
    auto inner = analyzeExpr((*p)->value, owner);
    if (!inner.IsSuccess())
      return inner;

    type = inner.Value();
  }

  mModel.expressionTypes[expr.get()] = type;
  return type;
}

Result<SymbolId> SemanticAnalyzer::resolveValue(const std::string& name, SymbolId owner, const Span& span) const
{
  auto id = mSymbols.lookup(name, owner);
  if (!id)
    return Result<SymbolId>::Failed(std::format("Unknown identifier '{}' with owner {} at {}", name, owner, span.toString()));

  return *id;
}

Result<SymbolId> SemanticAnalyzer::resolveComponentType(const Symbol& value, const Span& span) const
{
  if (value.kind == SymbolKind::Capability)
    return value.id;

  if (value.type.kind == TypeKind::Component && value.type.symbol != InvalidSymbol)
    return value.type.symbol;

  return Result<SymbolId>::Failed(std::format("'{}' is not a capability/component at {}", value.name, span.toString()));
}

}  // namespace koda
