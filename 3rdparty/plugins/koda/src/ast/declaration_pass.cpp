#include "declaration_pass.h"

#include <format>

namespace koda
{

VoidResult DeclarationPass::run(const System& system)
{
  mSymbolRegistry.clear();

  // First register every top-level component.
  // This is what makes forward references legal.
  for (const auto& component : system.components)
  {
    const auto kind = component->kind == Component::Kind::Task ? SymbolKind::Task : SymbolKind::Capability;
    auto result = mSymbolRegistry.declare(kind, component->name, Type::Unknown(), component->span);
    if (!result.IsSuccess())
      return VoidResult::Failed(result.ErrorMessage());
  }

  // Then populate each component scope.
  for (const auto& component : system.components)
  {
    auto result = declareComponent(component);
    if (!result.IsSuccess())
      return result;
  }

  return VoidResult{};
}

VoidResult DeclarationPass::declareComponent(const PComponent& component)
{
  const auto ownerOpt = mSymbolRegistry.component(component->name);
  if (!ownerOpt)
    return VoidResult::Failed(std::format("Internal compiler error: component '{}' was not registered", component->name));

  const auto owner = *ownerOpt;
  for (const auto& arg : component->args)
  {
    const auto type = mSymbolRegistry.resolveType(arg->a).value_or(Type::Unknown());
    auto result = mSymbolRegistry.declare(SymbolKind::Argument, arg->b, type, arg->span, owner);
    if (!result.IsSuccess())
      return VoidResult::Failed(result.ErrorMessage());
  }

  for (const auto& statement : component->statements)
  {
    auto result = declareStatement(statement, owner);
    if (!result.IsSuccess())
      return result;
  }

  return VoidResult{};
}

VoidResult DeclarationPass::declareStatement(const PStatement& statement, SymbolId owner)
{
  if (auto block = std::get_if<PVarsBlock>(&statement->node); block && *block)
  {
    for (const auto& var : (*block)->vars)
    {
      const auto type = mSymbolRegistry.resolveType(var->varType).value_or(Type::Unknown());
      auto result = mSymbolRegistry.declare(SymbolKind::Variable, var->name, type, var->span, owner);
      if (!result.IsSuccess())
        return VoidResult::Failed(result.ErrorMessage());
    }
  }
  else if (auto block = std::get_if<PStrategyBlock>(&statement->node); block && *block)
  {
    for (const auto& flow : (*block)->flows)
    {
      auto symbolId = mSymbolRegistry.declare(SymbolKind::Flow, flow->name, Type::Void(), flow->span, owner);
      if (!symbolId.IsSuccess())
        return VoidResult::Failed(symbolId.ErrorMessage());

      // We also need to declare the flow arguments, of course
      for (const auto& arg : flow->args)  // This is an awful name, by the way...
      {
        // No need for arg->a since we dont use types in flow arguments
        auto argId = mSymbolRegistry.declare(SymbolKind::Argument, arg->b, Type::Custom("FlowTag"), arg->span, symbolId.Value());
        if (!argId.IsSuccess())
          return VoidResult::Failed(argId.ErrorMessage());
      }
    }
  }
  else if (auto ros = std::get_if<PRosDef>(&statement->node); ros && *ros)
  {
    return declareRosDef(*ros, owner);
  }
  else if (auto action = std::get_if<PActionDef>(&statement->node); action && *action)
  {
    for (const auto& ros : (*action)->rosDefs)
    {
      auto result = declareRosDef(ros, owner);
      if (!result.IsSuccess())
        return result;
    }
  }

  return VoidResult{};
}

VoidResult DeclarationPass::declareRosDef(const PRosDef& ros, SymbolId owner)
{
  if (!ros || !ros->def)
    return VoidResult::Failed("Invalid ROS/event declaration");

  const auto eventType = Type::Custom(ros->toString());
  auto result = mSymbolRegistry.declare(SymbolKind::Event, ros->def->name, eventType, ros->def->span, owner);
  if (!result.IsSuccess())
    return result;

  // Also gotta declare the arguments in the definition
  for (const auto& arg : ros->def->args)
  {
    auto argId = mSymbolRegistry.declare(SymbolKind::Argument, arg->b, mSymbolRegistry.resolveType(arg->a).value_or(Type::Unknown()), arg->span, result.Value());
    if (!argId.IsSuccess())
      return VoidResult::Failed(argId.ErrorMessage());
  }

  return VoidResult();
}

}  // namespace koda
