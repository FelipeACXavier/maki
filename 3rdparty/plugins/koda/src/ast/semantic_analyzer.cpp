#include "semantic_analyzer.h"

#include <format>

#include "logging.h"

namespace koda
{
namespace
{
bool compatible(const types::TypeReference& expected, const types::TypeReference& actual)
{
  if (!expected.isValid() || !actual.isValid())
    return true;
  if (expected.kind() == actual.kind())
    return true;

  return expected.isNumeric() && actual.isNumeric();
}
}  // namespace

SemanticAnalyzer::SemanticAnalyzer(SymbolRegistry& symbols, types::TypeRegistry& types, types::Blackboard& blackboard)
    : mSymbols(symbols)
    , mBlackboard(blackboard)
    , mTypeRegistry(types)
{
}

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

  return VoidResult();
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

  return VoidResult();
}

VoidResult SemanticAnalyzer::collectRosSignature(const PRosDef& ros, SymbolId owner)
{
  if (!ros || !ros->def)
    return VoidResult();

  const auto event = mSymbols.lookupChild(owner, ros->def->name);
  if (!event)
    return VoidResult::Failed(std::format("Unknown event '{}'", ros->def->name));

  std::vector<types::TypeReference> args;
  for (const auto& arg : ros->def->args)
  {
    types::TypeReference typeRef;
    const auto* typeDef = mTypeRegistry.findByName(arg->a.toString());
    if (typeDef)
      typeRef = typeDef->toReference();

    args.push_back(typeRef);
  }

  mModel.eventArguments[*event] = std::move(args);
  return VoidResult();
}

VoidResult SemanticAnalyzer::analyzeComponent(const PComponent& component)
{
  const auto owner = mSymbols.component(component->name);
  if (!owner)
    return VoidResult::Failed(std::format("Unknown component '{}'", component->name));

  // Resolve argument types now that pass 1 has registered all components.
  for (const auto& arg : component->args)
  {
    auto symbolId = mSymbols.lookupChild(*owner, arg->a.toString());
    if (!symbolId)
      continue;

    if (auto* symbol = mSymbols.get(*symbolId))
    {
      types::TypeReference typeRef;
      const auto* typeDef = mTypeRegistry.findByName(arg->a.toString());
      if (typeDef)
        typeRef = typeDef->toReference();

      symbol->type = typeRef;
    }
  }

  for (const auto& statement : component->statements)
  {
    auto result = analyzeStatement(statement, *owner);
    if (!result.IsSuccess())
      return result;
  }
  return VoidResult();
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
    // Collect them but do not analyze all of them already
    for (const auto& flow : (*block)->flows)
    {
      auto flowSymbol = mSymbols.lookup(flow->name, owner);
      if (!flowSymbol)
        return VoidResult::Failed(std::format("Unknown flow '{}'", flow->name));

      mFlows[*flowSymbol] = flow;
    }

    // Only start from the entry flow.
    auto main = mSymbols.lookup("main", owner);
    if (!main)
      return VoidResult::Failed("Task has no main flow");

    return analyzeFlow(*main);
  }
  return VoidResult();
}

VoidResult SemanticAnalyzer::analyzeFlow(SymbolId flowId)
{
  const auto it = mFlows.find(flowId);
  if (it == mFlows.end())
    return VoidResult::Failed("Unknown flow");

  // Prevent recursive expansion.
  // TODO: Maybe we want this in the future?
  if (mActiveFlows.contains(flowId))
    return VoidResult::Failed("Recursive flow reference detected");

  mActiveFlows.insert(flowId);

  const auto result = analyzeStrategy(it->second->strategy, flowId);

  mActiveFlows.erase(flowId);

  return result;
}

VoidResult SemanticAnalyzer::analyzeStrategy(const PStrategy& strategy, SymbolId owner)
{
  if (!strategy)
    return VoidResult();

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
  else if (auto p = std::get_if<PRef>(&strategy->v); p && *p)
  {
    auto id = mSymbols.lookup((*p)->name, owner);
    const auto* symbol = id ? mSymbols.get(*id) : nullptr;
    if (!symbol || symbol->kind != SymbolKind::Flow)
      return VoidResult::Failed(std::format("Unknown flow '{}' at {}", (*p)->name, strategy->span.toString()));

    mModel.flowRefs[strategy.get()] = *id;

    return analyzeFlow(*id);
  }
  else if (auto p = std::get_if<PTaskCall>(&strategy->v); p && *p)
  {
    auto call = resolveCall((*p)->call, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed(call.ErrorMessage());

    mModel.calls[(*p)->call.get()] = call.Value();

    auto dataResult = resolveCapabilityData((*p)->call, call.Value());
    LOG_WARN_ON_FAILURE(dataResult);

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

  return VoidResult();
}

Result<ResolvedArgumentSource> SemanticAnalyzer::resolveArgumentSource(const PExpr& expr, const types::TypeReference& expectedType, SymbolId owner)
{
  if (!expr)
    return ResolvedArgumentSource{.kind = ArgumentSourceKind::Infer};

  // if (auto ref = std::get_if<PRefExpr>(&expr->v))
  // {
  //   auto slot = mBlackboard.lookup((*ref)->name);
  //   if (slot)
  //   {
  //     const auto* bb = mBlackboard.get(*slot);

  //     if (!mTypeRegistry.isAssignable(bb->type, expectedType))
  //       return Result<ResolvedArgumentSource>::Failed("Blackboard value has incompatible type");

  //     return ResolvedArgumentSource{.kind = ArgumentSourceKind::Blackboard, .slot = *slot};
  //   }
  // }

  // Otherwise treat it as a normal KODA expression.
  auto value = analyzeExpr(expr, owner);
  if (!value.IsSuccess())
    return Result<ResolvedArgumentSource>::Failed(value.ErrorMessage());
  if (!compatible(expectedType, value.Value()))
    return Result<ResolvedArgumentSource>::Failed("Argument has incompatible type. Expected {}, got {}", expectedType.toString(),
                                                  value.Value().toString());

  return ResolvedArgumentSource{.kind = ArgumentSourceKind::Literal};
}

VoidResult SemanticAnalyzer::resolveCapabilityData(const PEventCall& astCall, const ResolvedCall& call)
{
  // All of this is pure double checking
  const auto* receiverSymbol = mSymbols.get(call.receiver);
  if (!receiverSymbol)
    return VoidResult::Failed("Invalid call receiver");

  auto componentResult = resolveComponentType(*receiverSymbol, astCall->span);
  if (!componentResult.IsSuccess())
    return VoidResult::Failed(componentResult.ErrorMessage());

  auto capabilityId = componentResult.Value();
  LOG_DEBUG("Resolving data for name: {} rec: {}, Call: {} {}, Cap: {}", astCall->name, astCall->receiver, call.receiver, call.target, capabilityId);

  auto event = mSymbols.get(call.target);
  if (!event)
    return VoidResult::Failed("No event with symbol id: {}", call.target);

  if (event->type.toString() == "Trigger")
  {
    mBlackboard.print();
    LOG_DEBUG("  Is trigger: {} {}", event->name, event->id);
    // 1. Resolve consumed values.
    for (uint32_t i = 0; i < mModel.eventArguments[event->id].size(); ++i)
    {
      auto input = mModel.eventArguments[event->id][i];
      if (i < astCall->args.size())
      {
        auto resolvedArg = resolveArgumentSource(astCall->args.at(i), input, event->id);
        if (!resolvedArg)
          LOG_WARNING(resolvedArg.ErrorMessage());
        else
        {
          LOG_INFO("  No need to infer, type is defined:");
          astCall->args.at(i)->print("  ", true);
        }

        continue;
      }

      auto candidates = mBlackboard.availableCompatible(input, mTypeRegistry);
      if (candidates.empty())
        return VoidResult::Failed(std::format("No available value for input '{}'", input.toString()));
      if (candidates.size() > 1)
        return VoidResult::Failed(std::format("Ambiguous value for input '{}'", input.toString()));

      // mModel.inputBindings[astCall.get()][input.id] = candidates.front()->id;
    }

    if (astCall->receiver.empty())
    {
      // If we are dealing with a capability call, we need to make the data available as well.
      LOG_DEBUG("  Inferred is return: {} {}", event->name, event->id);
      auto* returnEvent = mSymbols.returnEventOf(capabilityId);
      if (!returnEvent)
        return VoidResult::Failed(std::format("Async capability call with no return '{}'", astCall->name));

      for (const auto& output : mModel.eventArguments[returnEvent->id])
      {
        const auto slot = mBlackboard.declare(astCall->id, output.toString(), output, std::to_string(capabilityId));
        mBlackboard.makeAvailable(slot);
        // mModel.outputBindings[astCall.get()][output.id] = slot;
      }
    }
  }
  else if (event->type.toString() == "Return")
  {
    mBlackboard.print();
    LOG_DEBUG("  Is return: {} {}", event->name, event->id);
    // 2. Create slots for produced values.
    for (const auto& output : mModel.eventArguments[event->id])
    {
      const auto slot = mBlackboard.declare(astCall->id, output.toString(), output, std::to_string(capabilityId));

      mBlackboard.makeAvailable(slot);
      // mModel.outputBindings[astCall.get()][output.id] = slot;
    }
  }

  return VoidResult();
}

VoidResult SemanticAnalyzer::analyzeHandler(const PStrategyHandler& handler, SymbolId owner)
{
  if (!handler)
    return VoidResult();

  if (handler->emitter)
  {
    auto call = resolveCall(handler->emitter, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed(call.ErrorMessage());
    mModel.calls[handler->emitter.get()] = call.Value();
  }

  if (handler->body)
    return analyzeStrategy(handler->body, owner);

  return VoidResult();
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

    const auto component = componentResult.Value();

    // Find the trigger component of this capability
    Symbol triggerEventSymbol;
    for (const auto eventId : mSymbols.children(component, SymbolKind::Event))
    {
      const auto* eventSymbol = mSymbols.get(eventId);
      if (!eventSymbol)
        continue;

      if (eventSymbol->type.toString() == "Trigger")
      {
        triggerEventSymbol = *eventSymbol;
        break;
      }
    }

    if (triggerEventSymbol.name.empty())
      return Result<ResolvedCall>::Failed("Could not find trigger event for: {}", call->name);

    // Unqualified invocation means the capability's trigger/default action.
    std::vector<std::string> args;
    for (const auto& arg : call->args)
    {
      auto type = analyzeExpr(arg, owner);
      if (!type.IsSuccess())
        return Result<ResolvedCall>::Failed(type.ErrorMessage());
    }
    return ResolvedCall{ResolvedCallKind::CapabilityTrigger, receiver, triggerEventSymbol.id, triggerEventSymbol.type};
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
    return Result<ResolvedCall>::Failed(
        std::format("Component '{}' has no event '{}' at {}", mSymbols.get(component)->name, call->name, call->span.toString()));

  const auto signature = mModel.eventArguments.find(*event);
  if (signature != mModel.eventArguments.end() && signature->second.size() != call->args.size())
    return Result<ResolvedCall>::Failed(std::format("Event '{}.{}' expects {} arguments, got {} at {}", call->receiver, call->name,
                                                    signature->second.size(), call->args.size(), call->span.toString()));

  for (std::size_t i = 0; i < call->args.size(); ++i)
  {
    auto actual = analyzeExpr(call->args[i], owner);
    if (!actual.IsSuccess())
      return Result<ResolvedCall>::Failed(actual.ErrorMessage());

    if (signature != mModel.eventArguments.end() && !compatible(signature->second[i], actual.Value()))
      return Result<ResolvedCall>::Failed(std::format("Argument {} of '{}.{}' has incompatible type at {}. Expected {} but got {}", i + 1,
                                                      call->receiver, call->name, call->args[i]->span.toString(), signature->second[i].toString(),
                                                      actual.Value().toString()));
  }

  return ResolvedCall{ResolvedCallKind::Event, receiver, *event, eventSymbol->type};
}

Result<types::TypeReference> SemanticAnalyzer::analyzeExpr(const PExpr& expr, SymbolId owner)
{
  if (!expr)
    return types::TypeReference{};

  types::TypeReference type;
  if (std::holds_alternative<PStr>(expr->v))
    type = types::TypeReference::createString();
  else if (std::holds_alternative<PInt>(expr->v))
    type = types::TypeReference::createInt();
  else if (std::holds_alternative<PFloat>(expr->v))
    type = types::TypeReference::createReal();
  else if (auto p = std::get_if<PId>(&expr->v); p && *p)
  {
    auto id = resolveValue((*p)->value, owner, expr->span);
    if (!id.IsSuccess())
      return Result<types::TypeReference>::Failed(id.ErrorMessage());
    const auto* symbol = mSymbols.get(id.Value());
    type = symbol ? symbol->type : types::TypeReference{};
  }
  else if (auto p = std::get_if<PCall>(&expr->v); p && *p)
  {
    auto call = resolveCall((*p)->value, owner);
    if (!call.IsSuccess())
      return Result<types::TypeReference>::Failed(call.ErrorMessage());

    mModel.calls[(*p)->value.get()] = call.Value();
    type = call.Value().returnType;
  }
  else if (auto p = std::get_if<PNeg>(&expr->v); p && *p)
  {
    auto inner = analyzeExpr((*p)->value, owner);
    if (!inner.IsSuccess())
      return inner;
    if (!inner.Value().isNumeric() && inner.Value().kind() != types::TypeReferenceKind::Unknown)
      return Result<types::TypeReference>::Failed(std::format("Unary '-' requires a numeric operand at {}", expr->span.toString()));
    type = inner.Value();
  }
  else if (auto p = std::get_if<PNot>(&expr->v); p && *p)
  {
    auto inner = analyzeExpr((*p)->value, owner);
    if (!inner.IsSuccess())
      return inner;
    type = types::TypeReference::createBool();
  }
  else if (auto p = std::get_if<PBinOp>(&expr->v); p && *p)
  {
    auto lhs = analyzeExpr((*p)->a, owner);
    if (!lhs.IsSuccess())
      return lhs;

    types::TypeReference rhsType;
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
        type = types::TypeReference::createBool();
        break;
      case K::Addition:
      case K::Subtraction:
      case K::Multiplication:
      case K::Division:
        if ((!lhs.Value().isNumeric() || !rhsType.isNumeric()) && lhs.Value().kind() != types::TypeReferenceKind::Unknown &&
            rhsType.kind() != types::TypeReferenceKind::Unknown)
          return Result<types::TypeReference>::Failed(std::format("Arithmetic operator requires numeric operands at {}", expr->span.toString()));

        type = types::isFloatingPoint(lhs.Value().primitiveKind()) || types::isFloatingPoint(rhsType.primitiveKind())
                   ? types::TypeReference::createReal()
                   : types::TypeReference::createInt();
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

  if (value.type.isNamed())
  {
    auto named = value.type.namedType();
    if (named.id && named.id.value() != std::to_string(InvalidSymbol))
      return std::stoul(named.id.value());
  }

  return Result<SymbolId>::Failed(std::format("'{}' is not a capability/component at {}", value.name, span.toString()));
}

}  // namespace koda
