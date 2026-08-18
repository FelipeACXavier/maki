#include "dezyne_declaration_pass.h"

#include <algorithm>
#include <cctype>
#include <format>

#include "ast/koda_compiler.h"
#include "logging.h"

namespace koda::dezyne
{
DeclarationPass::DeclarationPass(Model& model, const SymbolRegistry& symbols, const koda::CompilerOptions& options)
    : mModel(model)
    , mSymbols(symbols)
    , mOptions(options)
{
}

VoidResult DeclarationPass::run(const ir::Program& program)
{
  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Capability)
      RETURN_ON_FAILURE(declareCapability(component));

  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Task)
      RETURN_ON_FAILURE(declareTask(component));

  return {};
}

VoidResult DeclarationPass::declareCapability(const ir::Component& capability)
{
  const auto file = std::format("{}/a_{}.dzn", mOptions.outputDir, lower(capability.name));
  const auto component = mModel.declareComponent(componentName(capability.name), file, {capability.symbol, capability.span});

  for (const auto& event : capability.events)
  {
    auto protocol = event.kind == ir::EventKind::Out ? PortProtocol::Signal : PortProtocol::Action;
    mModel.declarePort(component, event.name, PortDirection::Provides, protocol, {event.symbol, event.span});
  }

  return {};
}

VoidResult DeclarationPass::declareTask(const ir::Component& task)
{
  const auto file = std::format("{}/{}_task.dzn", mOptions.outputDir, lower(task.name));
  const auto component = mModel.declareComponent(componentName(task.name), file, {task.symbol, task.span});
  mModel.declarePort(component, "api", PortDirection::Provides, PortProtocol::Action, {task.symbol, task.span});

  for (const auto& arg : task.arguments)
  {
    if (!arg.type.isNamed())
      continue;

    const auto named = arg.type.namedType();
    if (!named.id || named.id.value() == std::to_string(InvalidSymbol))
      continue;

    mModel.declareInstance(component, arg.name, componentName(sourceName(std::stoul(named.id.value()))), {arg.symbol, arg.span});
  }

  for (const auto& flow : task.flows)
  {
    mModel.declareInstance(component, "f_" + lower(flow.name), flowName(flow.name), {flow.symbol, flow.span});
    RETURN_ON_FAILURE(declareFlow(task, flow));
  }
  return {};
}

VoidResult DeclarationPass::declareFlow(const ir::Component& task, const ir::Flow& flow)
{
  const auto file = std::format("{}/{}.dzn", mOptions.outputDir, lower(flow.name));

  FlowState state;
  state.component = mModel.declareComponent(flowName(flow.name), file, {flow.symbol, flow.span}, false, task.symbol);
  mModel.declarePort(state.component, "api", PortDirection::Provides, PortProtocol::Action, {flow.symbol, flow.span});

  return declareStrategy(flow.strategy, state);
}

VoidResult DeclarationPass::declareStrategy(const ir::PStrategy& strategy, FlowState& state)
{
  if (!strategy)
    return VoidResult::Failed("Dezyne declaration pass: invalid strategy");

  if (auto p = std::get_if<ir::Strategy::Sequence>(&strategy->value))
    for (const auto& item : p->items)
      RETURN_ON_FAILURE(declareStrategy(item, state));
  else if (auto p = std::get_if<ir::Strategy::Join>(&strategy->value))
    for (const auto& item : p->items)
      RETURN_ON_FAILURE(declareStrategy(item, state));
  else if (auto p = std::get_if<ir::Strategy::Either>(&strategy->value))
    for (const auto& item : p->items)
      RETURN_ON_FAILURE(declareStrategy(item, state));
  else if (auto p = std::get_if<ir::Strategy::Within>(&strategy->value))
  {
    RETURN_ON_FAILURE(declareStrategy(p->body, state));
    RETURN_ON_FAILURE(declareStrategy(p->fallback, state));
    for (const auto& handler : p->handlers)
      RETURN_ON_FAILURE(declareHandler(handler, state));
  }
  else if (auto p = std::get_if<ir::Strategy::Repeat>(&strategy->value))
  {
    RETURN_ON_FAILURE(declareStrategy(p->body, state));
    for (const auto& handler : p->handlers)
      RETURN_ON_FAILURE(declareHandler(handler, state));
  }
  else if (auto p = std::get_if<ir::Strategy::FlowRef>(&strategy->value))
  {
    mModel.declarePort(state.component, sourceName(p->flow), PortDirection::Requires, PortProtocol::Action, {p->flow, strategy->span});
  }
  else if (auto p = std::get_if<ir::Strategy::TaskCall>(&strategy->value))
  {
    RETURN_ON_FAILURE(declareCall(p->call, state, false));
    for (const auto& handler : p->handlers)
      RETURN_ON_FAILURE(declareHandler(handler, state));
  }
  return {};
}

VoidResult DeclarationPass::declareHandler(const ir::PHandler& handler, FlowState& state)
{
  if (!handler)
    return VoidResult::Failed("Dezyne declaration pass: invalid handler");
  if (handler->emitter)
    RETURN_ON_FAILURE(declareCall(*handler->emitter, state, true));
  return declareStrategy(handler->body, state);
}

VoidResult DeclarationPass::declareCall(const ir::Call& call, FlowState& state, bool signal)
{
  std::string name;
  PortProtocol protocol = signal ? PortProtocol::Signal : PortProtocol::Action;

  if (call.kind == ir::CallKind::CapabilityTrigger)
  {
    const auto ordinal = ++state.triggerOrdinals[call.receiver];
    name = std::format("{}_{}", sourceName(call.receiver), ordinal);
  }
  else
  {
    name = std::format("{}_{}", sourceName(call.receiver), sourceName(call.target));
  }

  mModel.declarePort(state.component, name, PortDirection::Requires, protocol, {call.target, call.span});

  return {};
}

std::string DeclarationPass::sourceName(koda::SymbolId id) const
{
  const auto* symbol = mSymbols.get(id);
  return symbol ? symbol->name : "unknown";
}

std::string DeclarationPass::lower(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return value;
}

std::string DeclarationPass::componentName(const std::string& name)
{
  return "c" + lower(name);
}

std::string DeclarationPass::flowName(const std::string& name)
{
  return "f" + lower(name);
}

}  // namespace koda::dezyne
