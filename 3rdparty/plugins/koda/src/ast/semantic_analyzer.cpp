#include "semantic_analyzer.h"

#include <format>

#include "logging.h"

namespace koda
{
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
      const auto symbolId = mSymbols.lookupChild(owner, var->name);
      const auto* symbol = symbolId ? mSymbols.get(*symbolId) : nullptr;
      if (!symbol)
        return VoidResult::Failed(std::format("Unknown variable '{}'", var->name));

      auto init = analyzeExpr(var->init, owner, symbol->type);
      if (!init.IsSuccess())
        return VoidResult::Failed(init.ErrorMessage());

      if (!compatible(symbol->type, init.Value()))
        return VoidResult::Failed(std::format("Initializer for '{}' has incompatible type at {}", var->name, var->span.toString()));

      const auto slot = mBlackboard.declare(std::to_string(*symbolId), var->name, symbol->type, "mission");
      mBlackboard.makeAvailable(slot);

      // Useful later when resolving `drive(target1)`.
      mModel.variableSlots[*symbolId] = slot;
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
  else if (auto p = std::get_if<PTaskCall>(&strategy->v); p && *p)
  {
    auto call = resolveCall((*p)->call, owner);
    if (!call.IsSuccess())
      return VoidResult::Failed("analyzeStrategy: {}", call.ErrorMessage());

    mModel.calls[(*p)->call.get()] = call.Value();

    if (call.Value().kind == ResolvedCallKind::Flow)
    {
      auto result = analyzeFlowCall((*p)->call, call.Value(), owner);
      if (!result.IsSuccess())
        return result;
    }
    else
    {
      auto dataResult = resolveCapabilityData((*p)->call, call.Value(), owner);
      if (!dataResult.IsSuccess())
        return dataResult;
    }

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

  if (auto idExpr = std::get_if<PId>(&expr->v); idExpr && *idExpr)
  {
    // In case we have the default placeholder
    if ((*idExpr)->value == "_")
      return ResolvedArgumentSource{.kind = ArgumentSourceKind::Infer};

    auto symbolResult = resolveValue((*idExpr)->value, owner, expr->span);
    if (!symbolResult.IsSuccess())
      return Result<ResolvedArgumentSource>::Failed(symbolResult.ErrorMessage());

    const SymbolId symbolId = symbolResult.Value();
    if (!mFlowArgumentStack.empty())
    {
      const auto& context = mFlowArgumentStack.back();
      if (auto it = context.find(symbolId); it != context.end())
        return it->second;
    }

    const auto* symbol = mSymbols.get(symbolId);
    if (!symbol)
      return Result<ResolvedArgumentSource>::Failed("Invalid symbol for argument '{}'", (*idExpr)->value);

    // Is this symbol backed by a blackboard slot?
    auto slotIt = mModel.variableSlots.find(symbolId);
    if (slotIt != mModel.variableSlots.end())
    {
      const auto* slot = mBlackboard.get(slotIt->second);
      if (!slot)
        return Result<ResolvedArgumentSource>::Failed("Blackboard slot for '{}' does not exist", (*idExpr)->value);

      if (!compatible(expectedType, slot->type))
        return Result<ResolvedArgumentSource>::Failed("Blackboard value '{}' has incompatible type. Expected '{}', got '{}'", (*idExpr)->value,
                                                      expectedType.toString(), slot->type.toString());

      return ResolvedArgumentSource{.kind = ArgumentSourceKind::Blackboard, .slot = slot->id};
    }
  }

  // Otherwise treat it as a normal KODA expression.
  auto value = analyzeExpr(expr, owner);
  if (!value.IsSuccess())
    return Result<ResolvedArgumentSource>::Failed(value.ErrorMessage());
  if (!compatible(expectedType, value.Value()))
    return Result<ResolvedArgumentSource>::Failed("Argument has incompatible type. Expected {}, got {}", expectedType.toString(),
                                                  value.Value().toString());

  return ResolvedArgumentSource{.kind = ArgumentSourceKind::Literal};
}

VoidResult SemanticAnalyzer::resolveCapabilityData(const PEventCall& astCall, const ResolvedCall& call, SymbolId owner)
{
  // All of this is pure double checking
  const auto* receiverSymbol = mSymbols.get(call.receiver);
  if (!receiverSymbol)
    return VoidResult::Failed("Invalid call receiver");

  auto componentResult = resolveComponentType(*receiverSymbol, astCall->span);
  if (!componentResult.IsSuccess())
    return VoidResult::Failed(componentResult.ErrorMessage());

  auto capabilityId = componentResult.Value();
  auto event = mSymbols.get(call.target);
  if (!event)
    return VoidResult::Failed("No event with symbol id: {}", call.target);

  if (event->type.toString() == "Trigger")
  {
    // 1. Resolve consumed values.
    for (uint32_t i = 0; i < mModel.eventArguments[event->id].size(); ++i)
    {
      const auto& input = mModel.eventArguments[event->id][i];
      PExpr argument = nullptr;
      if (i < astCall->args.size())
        argument = astCall->args[i];

      auto source = resolveArgumentSource(argument, input, owner);  // use the call's lexical owner, not event->id
      if (!source.IsSuccess())
        return VoidResult::Failed(source.ErrorMessage());

      switch (source.Value().kind)
      {
        case ArgumentSourceKind::Literal:
          break;

        case ArgumentSourceKind::Blackboard:
          // mModel.inputBindings[astCall.get()][i] = *source.Value().slot;
          break;

        case ArgumentSourceKind::Infer:
        {
          auto candidates = mBlackboard.availableCompatible(input, mTypeRegistry);
          if (candidates.empty())
            return VoidResult::Failed("No available value for input '{}'", input.toString());
          if (candidates.size() > 1)
            LOG_WARNING("Ambiguous value for input '{}'", input.toString());

          // mModel.inputBindings[astCall.get()][i] = candidates.front()->id;
          break;
        }
      }
    }

    if (astCall->receiver.empty())
    {
      // If we are dealing with a capability call, we need to make the data available as well.
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
      return VoidResult::Failed("analyzeHandler: {}", call.ErrorMessage());
    mModel.calls[handler->emitter.get()] = call.Value();
  }

  if (handler->body)
    return analyzeStrategy(handler->body, owner);

  return VoidResult();
}

VoidResult SemanticAnalyzer::analyzeFlowCall(const PEventCall& astCall, const ResolvedCall& call, SymbolId callerOwner)
{
  const auto it = mFlows.find(call.receiver);
  if (it == mFlows.end())
    return VoidResult::Failed("Unknown flow: {}", astCall->name);

  const auto& flow = it->second;
  if (flow->args.size() != astCall->args.size())
    return VoidResult::Failed("Number of arguments in call to '{}' does not match expectation", astCall->name);

  FlowArgumentContext context;
  for (size_t i = 0; i < flow->args.size(); ++i)
  {
    const auto& formal = flow->args[i];
    const auto& actual = astCall->args[i];

    // The formal should already have a symbol owned by the flow.
    auto formalId = mSymbols.lookupChild(call.receiver, formal->b);
    if (!formalId)
      return VoidResult::Failed("Unknown parameter '{}' for flow '{}'", formal->b, astCall->name);

    // For now the actual expression determines the source.
    auto actualType = analyzeExpr(actual, callerOwner);
    if (!actualType.IsSuccess())
      return VoidResult::Failed(actualType.ErrorMessage());

    auto source = resolveArgumentSource(actual, actualType.Value(), callerOwner);
    if (!source.IsSuccess())
      return VoidResult::Failed(source.ErrorMessage());

    context[*formalId] = source.Value();
  }

  mFlowArgumentStack.push_back(std::move(context));

  auto result = analyzeFlow(call.receiver);

  mFlowArgumentStack.pop_back();

  return result;
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

    if (receiverSymbol->kind == SymbolKind::Flow)
    {
      const auto flowIt = mFlows.find(receiver);
      if (flowIt == mFlows.end())
        return Result<ResolvedCall>::Failed("Unknown flow '{}'", call->name);

      const auto& flow = flowIt->second;
      if (flow->args.size() != call->args.size())
        return Result<ResolvedCall>::Failed("Flow '{}' expects {} arguments, got {}", call->name, flow->args.size(), call->args.size());

      return ResolvedCall{ResolvedCallKind::Flow, receiver, InvalidSymbol, types::TypeReference{}};
    }

    auto componentResult = resolveComponentType(*receiverSymbol, call->span);
    if (!componentResult.IsSuccess())
      return Result<ResolvedCall>::Failed(componentResult.ErrorMessage());

    const auto component = componentResult.Value();

    // Find the trigger component of this capability
    const Symbol* triggerEventSymbol = mSymbols.triggerEventOf(component);
    if (!triggerEventSymbol)
      return Result<ResolvedCall>::Failed("Could not find trigger event for: {}", call->name);

    // Unqualified invocation means the capability's trigger/default action.
    std::vector<std::string> args;
    for (const auto& arg : call->args)
    {
      auto type = analyzeExpr(arg, owner);
      if (!type.IsSuccess())
        return Result<ResolvedCall>::Failed(type.ErrorMessage());
    }
    return ResolvedCall{ResolvedCallKind::CapabilityTrigger, receiver, triggerEventSymbol->id, triggerEventSymbol->type};
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

Result<types::TypeReference> SemanticAnalyzer::analyzeExpr(const PExpr& expr, SymbolId owner, const types::TypeReference& expected)
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
  else if (std::holds_alternative<PBool>(expr->v))
    type = types::TypeReference::createBool();
  else if (auto p = std::get_if<PId>(&expr->v); p && *p)
  {
    if ((*p)->value == "_")
    {
      type = types::TypeReference{};
    }
    else
    {
      auto id = resolveValue((*p)->value, owner, expr->span);
      if (!id.IsSuccess())
        return Result<types::TypeReference>::Failed(id.ErrorMessage());

      const auto* symbol = mSymbols.get(id.Value());
      type = symbol ? symbol->type : types::TypeReference{};
    }
  }
  else if (auto p = std::get_if<PCall>(&expr->v); p && *p)
  {
    auto call = resolveCall((*p)->value, owner);
    if (!call.IsSuccess())
      return Result<types::TypeReference>::Failed("analyzeExpr: {}", call.ErrorMessage());

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
  else if (auto p = std::get_if<PRecordLiteral>(&expr->v); p && *p)
  {
    if (!expected.isValid())
      return Result<types::TypeReference>::Failed(std::format("Cannot infer type of record literal at {}", expr->span.toString()));

    const auto* definition = mTypeRegistry.resolve(expected);
    if (!definition || !definition->isRecord())
      return Result<types::TypeReference>::Failed(std::format("Type '{}' is not a record at {}", expected.toString(), expr->span.toString()));

    const auto& recordDef = definition->record();
    std::set<std::string> initialized;
    for (const auto& field : (*p)->fields)
    {
      if (!initialized.insert(field->name).second)
        return Result<types::TypeReference>::Failed(std::format("Field '{}' initialized more than once at {}", field->name, field->span.toString()));

      auto fieldIt = std::find_if(recordDef.fields.begin(), recordDef.fields.end(),
                                  [name = field->name](const types::FieldDefinition& f) { return name == f.name; });
      if (fieldIt == recordDef.fields.end())
        return Result<types::TypeReference>::Failed(std::format("Unknown field '{}' in type '{}'", field->name, expected.toString()));

      auto valueType = analyzeExpr(field->value, owner, fieldIt->type);
      if (!valueType.IsSuccess())
        return valueType;

      if (!compatible(fieldIt->type, valueType.Value()))
      {
        return Result<types::TypeReference>::Failed("Field '{}' expects '{}' ({}) but got '{}' ({})", field->name, fieldIt->type.toString(),
                                                    types::toString(fieldIt->type.kind()), valueType.Value().toString(),
                                                    types::toString(valueType.Value().kind()));
      }
    }

    type = expected;
  }

  else
  {
    return Result<types::TypeReference>::Failed("Unknown expression type at: {}", expr->span.toString());
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

bool SemanticAnalyzer::compatible(const types::TypeReference& expected, const types::TypeReference& actual)
{
  if (!expected.isValid() || !actual.isValid())
    return true;

  const auto* expectedDef = mTypeRegistry.resolve(expected);
  const auto* actualDef = mTypeRegistry.resolve(actual);

  if (!expectedDef || !actualDef)
    return false;

  if (expectedDef->id == actualDef->id)
    return true;

  if (expectedDef->isPrimitive() && actualDef->isPrimitive())
    return types::isPrimitiveAssignable(actualDef->primitive().primitive, expectedDef->primitive().primitive);

  return false;
}

}  // namespace koda
