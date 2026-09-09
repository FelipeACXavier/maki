#include "dezyne_lowering_pass.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <sstream>

#include "ast/koda_compiler.h"
#include "dezyne_library_emitter.h"
#include "logging.h"

namespace koda::dezyne
{
bool isActionEvent(ir::EventKind kind)
{
  return kind == ir::EventKind::Trigger || kind == ir::EventKind::In;
}

LoweringPass::LoweringPass(Model& model, const SymbolRegistry& symbols, const koda::CompilerOptions& options)
    : mModel(model)
    , mSymbols(symbols)
    , mOptions(options)
{
}

VoidResult LoweringPass::run(const ir::Program& program)
{
  mCallCounts.clear();
  mCallOrdinals.clear();
  mActionEvents.clear();
  mFlows.clear();
  mCapabilities.clear();

  // First determine which event symbols correspond to callable Dezyne action ports
  for (const auto& component : program.components)
  {
    if (component.kind != ir::ComponentKind::Capability)
      continue;

    for (const auto& action : component.actions)
      for (const auto& event : action.events)
        if (isActionEvent(event.kind))
          mActionEvents.insert(event.symbol);
        else if (event.kind == ir::EventKind::Abort)
          mAbortEvents.insert(event.symbol);

    mCapabilities[component.symbol] = &component;
  }

  // Then count their usages across all flows.
  for (const auto& component : program.components)
    for (const auto& flow : component.flows)
      countTriggers(flow.strategy);

  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Capability)
      RETURN_ON_FAILURE(lowerCapability(component));

  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Task)
      RETURN_ON_FAILURE(lowerTask(component));

  RETURN_ON_FAILURE(createConditionInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createExternalInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createActionInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createSignalInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createAbortInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createTypes(mModel, mOptions.outputDir));

  return VoidResult();
}

VoidResult LoweringPass::lowerCapability(const ir::Component& capability)
{
  auto* component = mModel.findComponent(componentName(capability.name));
  if (!component)
    return VoidResult::Failed("Missing declared Dezyne capability: " + capability.name);

  std::ostringstream out;
  out << "import iexternal.dzn;\n";
  out << "import isignal.dzn;\n\n";
  out << std::format("component {} {{\n", componentName(capability.name));
  for (const auto& action : capability.actions)
  {
    for (const auto& event : action.events)
    {
      if (isActionEvent(event.kind))
      {
        // Remove the port before in case the name remains the same during declaration
        if (auto port = mModel.findPort(component->symbol, event.name))
          mModel.removePort(component->symbol, port->symbol);

        const auto count = std::max<std::uint32_t>(0, mCallCounts[event.symbol]);
        for (std::uint32_t i = 0; i < count; ++i)
        {
          const auto name = count == 1 ? event.name : std::format("{}_{}", event.name, i + 1);
          mModel.declarePort(component->symbol, name, PortDirection::Provides, PortProtocol::External, {event.symbol, event.span});
          out << std::format("  provides iexternal {};\n", name);
        }
      }
      else if (event.kind == ir::EventKind::Out)
      {
        out << std::format("  provides isignal {};\n", event.name);
      }
      else if (event.kind == ir::EventKind::Abort || event.kind == ir::EventKind::Return || event.kind == ir::EventKind::Error)
      {
        // Abort is exposed once by the armour.
        //
        // Return and Error are represented by the corresponding
        // iexternal interaction protocol rather than separate ports.
        if (auto port = mModel.findPort(component->symbol, event.name))
          mModel.removePort(component->symbol, port->symbol);
      }
    }
  }
  out << "}\n";

  mModel.setGeneratedFile(component->fileName, out.str(), {capability.symbol, capability.span});

  return VoidResult();
}

VoidResult LoweringPass::lowerTask(const ir::Component& task)
{
  mFlows.clear();
  for (const auto& flow : task.flows)
  {
    auto result = lowerFlow(flow);
    if (!result.IsSuccess())
      return VoidResult::Failed(result.ErrorMessage());

    mFlows[flow.symbol] = result.Value();
  }

  const auto* component = mModel.findComponent(componentName(task.name));
  if (!component)
    return VoidResult::Failed("Missing declared Dezyne task: " + task.name);

  // Keep a stable ID instead of a pointer across model mutations.
  const auto componentId = component->symbol;

  std::ostringstream out;
  std::map<koda::SymbolId, std::string> importedCapabilities;
  for (const auto& arg : task.arguments)
    if (arg.type.isNamed())
    {
      const auto named = arg.type.namedType();
      if (named.id && named.id.value() != std::to_string(InvalidSymbol))
        importedCapabilities[std::stoul(named.id.value())] = arg.name;
    }

  bool hasAlarm = false;
  for (const auto& [_, result] : mFlows)
    hasAlarm = hasAlarm || !result.alarms.empty();

  std::vector<Connection> connections;
  if (!task.flows.empty())
  {
    // Find and connect the ports of the main flow
    auto it = std::find_if(task.flows.begin(), task.flows.end(), [](const ir::Flow& flow) { return flow.name == "main"; });
    const auto& entry = it == task.flows.end() ? task.flows.front() : *it;
    connections.push_back({
        .lhs = "api",
        .rhs = "f_" + lower(entry.name) + ".api",
        .span = entry.span,
    });
  }

  // We must update the alarm name since we have multiple alarms at the top level
  uint32_t alarmId = 0;
  for (const auto& flow : task.flows)
  {
    const auto flowInstance = "f_" + lower(flow.name);
    const auto found = mFlows.find(flow.symbol);
    if (found == mFlows.end())
      continue;

    for (const auto& call : found->second.calls)
    {
      if (call.kind == CallUse::Kind::Flow)
      {
        connections.push_back({
            .lhs = flowInstance + "." + call.localPort,
            .rhs = "f_" + lower(sourceName(call.target)) + ".api",
            .span = call.span,
        });

        mModel.declareCallSite({
            .kind = CallSiteKind::Flow,
            .flow = call.flow,
            .receiver = koda::InvalidSymbol,
            .target = call.target,

            .localPort = call.localPort,
            .targetPort = {},

            .localOrdinal = call.localOrdinal,
            .targetOrdinal = call.targetOrdinal,

            .arguments = call.arguments,
            .inputSlots = call.inputSlots,
            .outputSlots = call.outputSlots,

            .origin = {.sourceSymbol = call.target, .sourceSpan = call.span},
        });

        continue;
      }

      auto targetPort = sourceName(call.target);
      if (mActionEvents.contains(call.target))
      {
        const auto count = std::max<std::uint32_t>(1, mCallCounts[call.target]);
        if (count > 1)
          targetPort = std::format("{}_{}", targetPort, call.targetOrdinal);
      }

      const auto receiverName = sourceName(call.receiver);
      const auto armourName = lower(receiverName) + "_armour";
      std::string resourcePort = targetPort;
      if (call.kind == CallUse::Kind::Abort)
        resourcePort = "abort";

      connections.push_back({
          .lhs = flowInstance + "." + call.localPort,
          .rhs = armourName + "." + resourcePort,
          .span = call.span,
          .kind = CallUse::toPortProtocol(call.kind),
      });

      CallSiteKind kind = call.toCallSiteKind();
      const auto* receiver = mSymbols.get(call.receiver);  // This is the instantiation of a capability
      if (!receiver)
        return VoidResult::Failed("No receiver for call {} in {} ({})", call.localPort, task.name, call.target);

      // We need to get the actual capability, which can be found by using the capability type
      const auto receiverComponent = mSymbols.get(receiver->id);
      if (!receiverComponent)
        return VoidResult::Failed("Unknown capability type for {} ({}) in {}", receiver->name, receiver->id, task.name);

      if (mOptions.traceability)
      {
        // We need the whole tree, so
        auto emitterBase = std::format("sut.{}.{}", sourceName(call.receiver), targetPort);
        LOG_DEBUG("Mapping {} to {}", call.traceId, emitterBase);
        mOptions.traceability->mapEmitter(emitterBase, call.traceId);
        mOptions.traceability->mapEmitter(std::format("{}.trigger", emitterBase), call.traceId);
        mOptions.traceability->mapEmitter(std::format("{}.abort", emitterBase), call.traceId);

        auto eventName = targetPort;
        int ordinalIndex = targetPort.find_last_of("_");
        if (ordinalIndex >= 0)
          eventName = eventName.substr(0, ordinalIndex);

        mOptions.traceability->mapEvent(std::format("{}.trigger", emitterBase), std::format("{} {}", sourceName(call.receiver), eventName));
        mOptions.traceability->mapEvent(std::format("{}.reset", emitterBase), std::format("{} recover", sourceName(call.receiver)));
        mOptions.traceability->mapEvent(std::format("{}.abort", emitterBase), std::format("{} abort", sourceName(call.receiver)));
      }

      mModel.declareCallSite({
          .kind = kind,
          .flow = call.flow,
          .receiver = receiverComponent->id,
          .target = call.target,

          .localPort = call.localPort,
          .targetPort = targetPort,

          .localOrdinal = call.localOrdinal,
          .targetOrdinal = call.targetOrdinal,

          .arguments = call.arguments,
          .inputSlots = call.inputSlots,
          .outputSlots = call.outputSlots,

          .origin = {.sourceSymbol = call.target, .sourceSpan = call.span},
      });
    }

    for (const auto& alarm : found->second.alarms)
    {
      auto alarmName = std::format("alarm{}", alarmId++);
      mModel.declareInstance(componentId, alarmName, "calarm", {componentId, flow.span});
      connections.push_back({
          .lhs = flowInstance + "." + alarm,  // The port remains the same since that is set based on the
          .rhs = alarmName + ".api",
          .span = flow.span,
      });
    }
  }

  for (const auto& arg : task.arguments)
  {
    if (!arg.type.isNamed())
      continue;

    const auto named = arg.type.namedType();

    if (!named.id || named.id.value() == std::to_string(InvalidSymbol))
      continue;

    const auto capabilityId = std::stoul(named.id.value());
    const auto found = mCapabilities.find(capabilityId);
    if (found == mCapabilities.end())
      continue;

    const auto& capability = *found->second;
    const auto externalInstance = lower(arg.name);
    const auto armourInstance = externalInstance + "_armour";
    std::vector<std::string> ports;
    for (const auto& action : capability.actions)
      for (const auto& event : action.events)
      {
        if (!isActionEvent(event.kind))
          continue;

        const auto count = std::max<std::uint32_t>(0, mCallCounts[event.symbol]);
        for (std::uint32_t i = 0; i < count; ++i)
        {
          const auto port = count == 1 ? event.name : std::format("{}_{}", event.name, i + 1);

          connections.push_back({
              .lhs = std::format("{}.r_{}", armourInstance, port),
              .rhs = std::format("{}.{}", externalInstance, port),
              .span = arg.span,
          });

          ports.push_back(port);
        }
      }

    bool hasAbort = true;
    const auto instances = mSymbols.instancesOf(capabilityId);
    for (const auto& instance : instances)
      hasAbort = hasAbort && usesCapabilityAbort(instance);

    LOG_DEBUG("Generating armour for {} {} abort", externalInstance, hasAbort ? "with" : "without");
    LibraryComponent libArmour;
    ASSIGN_OR_RETURN_ON_FAILURE(libArmour, createCapabilityArmour(mModel, mOptions.outputDir, externalInstance, hasAbort, ports, componentId));
    mModel.declareInstance(componentId, armourInstance, libArmour.name);
  }

  // Make sure the arbiters exists in case they are needed
  RETURN_ON_FAILURE(createNecessaryArbiter(componentId, connections));

  // Emit any instances introduced during topology lowering (alarms/arbiters).
  out << "import iaction.dzn;\n";
  out << "import isignal.dzn;\n";
  for (const auto& [symbol, name] : importedCapabilities)
  {
    out << std::format("import a_{}.dzn;\n", lower(sourceName(symbol)));
    out << std::format("import {}_armour.dzn;\n", lower(name));  // TODO: This should contain the actual name
  }

  for (const auto& flow : task.flows)
  {
    out << std::format("import {}.dzn;\n", lower(flow.name));
    const auto flowInstance = std::format("f_{}", sourceName(flow.symbol));
    const auto resultIt = mFlows.find(flow.symbol);

    if (resultIt == mFlows.end())
      return VoidResult::Failed(std::format("Missing lowering result for flow '{}'", sourceName(flow.symbol)));

    const auto& flowResult = resultIt->second;
    for (const auto& condition : flowResult.conditions)
    {
      // const auto conditionId = conditionInstanceId++;
      const auto conditionName = std::format("{}_{}", flow.name, condition.localPort);
      const auto conditionType = std::format("c{}", conditionName);

      out << std::format("import {}.dzn;\n", conditionName);
      RETURN_ON_FAILURE(createConditionComponent(mModel, mOptions.outputDir, conditionName, componentId));
      mModel.declareInstance(componentId, conditionName, conditionType, {componentId, condition.span});

      connections.push_back({
          .lhs = std::format("{}.{}", flowInstance, condition.localPort),
          .rhs = conditionName + ".api",
          .span = condition.span,
          .kind = PortProtocol::Condition,
      });
    }
  }

  if (hasAlarm)
    out << "import alarm.dzn;\n";

  // Helper creation may have reallocated the model's component storage.
  component = mModel.getComponent(componentId);
  if (!component)
    return VoidResult::Failed("Missing Dezyne task after lowering: " + task.name);

  std::vector<std::string> seenImports;
  for (const auto& instance : component->instances)
  {
    const auto* symbol = mModel.mSymbols.get(instance.symbol);
    if (!symbol)
      continue;

    LOG_TRACE("Adding import: {} {}", symbol->name, instance.typeName);
    if (instance.typeName.starts_with("caction_arbiter"))
    {
      const auto suffix = instance.typeName.substr(std::string("caction_arbiter").size());
      const auto name = std::format("action_arbiter{}", suffix);
      if (std::count(seenImports.begin(), seenImports.end(), name) == 0)
      {
        out << std::format("import {}.dzn;\n", name);
        seenImports.push_back(name);
      }
    }
    else if (instance.typeName.starts_with("cabort_arbiter"))
    {
      const auto suffix = instance.typeName.substr(std::string("cabort_arbiter").size());
      const auto name = std::format("abort_arbiter{}", suffix);
      if (std::count(seenImports.begin(), seenImports.end(), name) == 0)
      {
        out << std::format("import {}.dzn;\n", name);
        seenImports.push_back(name);
      }
    }
  }

  out << std::format("\ncomponent {} {{\n", componentName(task.name));
  out << "  provides iaction api;\n\n";
  out << "  system {\n";

  for (const auto& instance : component->instances)
    if (const auto* symbol = mModel.mSymbols.get(instance.symbol))
      out << std::format("    {} {};\n", instance.typeName, symbol->name);

  std::string current = "";
  std::uint32_t connectionId = 0;
  for (const auto& connection : connections)
  {
    auto leftPort = portFromString(connection.lhs);
    if (leftPort.instance != current)
    {
      out << "\n";
      current = leftPort.instance;
    }

    out << std::format("    {} <=> {};\n", connection.lhs, connection.rhs);
    mModel.declareConnection(componentId, std::to_string(connectionId++), connection.lhs, connection.rhs, {componentId, connection.span});
  }

  out << "  }\n}\n";

  mModel.setGeneratedFile(component->fileName, out.str(), {task.symbol, task.span});
  return {};
}

std::vector<LoweringPass::CallUse> LoweringPass::uniqueRequiredPorts(const FlowState& state) const
{
  std::vector<CallUse> result;
  std::set<std::tuple<CallUse::Kind, koda::SymbolId, koda::SymbolId, std::uint32_t>> seen;

  for (const auto& call : state.calls)
  {
    // Trigger and In events get distinct flow-local ports when the same event
    // is used multiple times. Other resources deliberately collapse to one
    // external port so a flow-local arbiter can fan out internal uses.
    const std::uint32_t ordinal = mActionEvents.contains(call.target) ? call.localOrdinal : 0;
    const auto key = std::make_tuple(call.kind, call.receiver, call.target, ordinal);

    if (seen.insert(key).second)
      result.push_back(call);
  }

  return result;
}

Result<LoweringPass::FlowResult> LoweringPass::lowerFlow(const ir::Flow& flow)
{
  const auto* initialComponent = mModel.findComponent(flowName(flow.name));
  if (!initialComponent)
    return Result<FlowResult>::Failed("Missing declared Dezyne flow: " + flow.name);

  // Keep only the stable ID across lowering because helper declaration may
  // reallocate Model::mComponents.
  const auto componentId = initialComponent->symbol;

  FlowState state;
  state.component = componentId;
  state.flow = flow.symbol;

  auto endpoint = lowerStrategy(flow, flow.strategy, state);
  if (!endpoint.IsSuccess())
    return Result<FlowResult>::Failed(endpoint.ErrorMessage());

  const auto requiredCalls = uniqueRequiredPorts(state);

  // Exact call-site ports are only known after lowering. Remove stale
  // required action/signal ports declared earlier and replace them with the
  // actual flow interface. Keep provides ports (api) and alarm ports.
  if (auto* flowComponent = mModel.getComponent(componentId))
  {
    std::erase_if(flowComponent->ports,
                  [](const Port& port) { return port.direction == PortDirection::Requires && port.protocol != PortProtocol::Alarm; });
  }

  for (const auto& call : requiredCalls)
  {
    const auto protocol = CallUse::toPortProtocol(call.kind);
    mModel.declarePort(componentId, call.localPort, PortDirection::Requires, protocol, {call.target, call.span});
  }

  for (const auto& condition : state.conditions)
    mModel.declarePort(componentId, condition.localPort, PortDirection::Requires, PortProtocol::Condition, {componentId, condition.span});

  // Resolve repeated uses of the same external resource inside the flow.
  RETURN_ON_FAILURE_AS(createNecessaryArbiter(componentId, state.connections), LoweringPass::FlowResult);

  // Reacquire after all model mutations.
  const auto* component = mModel.getComponent(componentId);
  if (!component)
    return Result<FlowResult>::Failed("Missing Dezyne flow after lowering: " + flow.name);

  std::ostringstream out;
  std::vector<std::string> seenImports;

  state.imports.insert("iaction.dzn");
  for (const auto& import : state.imports)
  {
    if (std::count(seenImports.begin(), seenImports.end(), import) > 0)
      continue;

    out << "import " << import << ";\n";
    seenImports.push_back(import);
  }

  for (const auto& instance : component->instances)
  {
    const auto* symbol = mModel.mSymbols.get(instance.symbol);
    if (!symbol)
      continue;

    LOG_DEBUG("Adding import: {} {}", symbol->name, instance.typeName);
    if (instance.typeName.starts_with("caction_arbiter"))
    {
      const auto suffix = instance.typeName.substr(std::string("caction_arbiter").size());
      const auto name = std::format("action_arbiter{}", suffix);
      if (std::count(seenImports.begin(), seenImports.end(), name) == 0)
      {
        out << std::format("import {}.dzn;\n", name);
        seenImports.push_back(name);
      }
    }
    else if (instance.typeName.starts_with("cabort_arbiter"))
    {
      const auto suffix = instance.typeName.substr(std::string("cabort_arbiter").size());
      const auto name = std::format("abort_arbiter{}", suffix);
      if (std::count(seenImports.begin(), seenImports.end(), name) == 0)
      {
        out << std::format("import {}.dzn;\n", name);
        seenImports.push_back(name);
      }
    }
  }

  out << std::format("\ncomponent {} {{\n", flowName(flow.name));

  for (const auto& port : component->ports)
  {
    const auto* symbol = mModel.mSymbols.get(port.symbol);
    if (!symbol)
      continue;

    const auto direction = port.direction == PortDirection::Provides ? "provides" : "requires";
    const auto protocol = portToString(port.protocol);

    out << std::format("  {} {} {};\n", direction, protocol, symbol->name);
  }

  out << "\n  system {\n";

  for (const auto& instance : component->instances)
    if (const auto* symbol = mModel.mSymbols.get(instance.symbol))
      out << std::format("    {} {};\n", instance.typeName, symbol->name);

  out << "\n    api <=> " << endpoint.Value() << ";\n";

  std::string current = "api";
  for (const auto& connection : state.connections)
  {
    const auto leftPort = portFromString(connection.lhs);
    if (leftPort.instance != current)
    {
      out << "\n";
      current = leftPort.instance;
    }

    out << "    " << connection.lhs << " <=> " << connection.rhs << ";\n";
  }

  out << "  }\n}\n";

  mModel.setGeneratedFile(component->fileName, out.str(), {flow.symbol, flow.span});

  return FlowResult{requiredCalls, state.alarms, state.conditions};
}

LoweringPass::PortRef LoweringPass::portFromString(const std::string& ref) const
{
  auto index = ref.find_first_of(".");
  if (index == std::string::npos)
    return PortRef{ref, ""};

  return PortRef{ref.substr(0, index), ref.substr(index + 1, ref.size() - index)};
}

Result<std::string> LoweringPass::lowerStrategy(const ir::Flow& flow, const ir::PStrategy& strategy, FlowState& state)
{
  if (!strategy)
    return Result<std::string>::Failed("Invalid Dezyne strategy");

  if (auto p = std::get_if<ir::Strategy::Sequence>(&strategy->value))
  {
    std::vector<ir::PStrategy> items;
    for (const auto& item : p->items)
      if (!std::holds_alternative<ir::Strategy::Continue>(item->value))
        items.push_back(item);

    if (items.size() == 1)
      return lowerStrategy(flow, items.front(), state);

    if (items.empty())
      return std::string("continue");

    LibraryComponent libComponent;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(libComponent, createSequenceComponent(mModel, mOptions.outputDir, items.size(), flow.symbol), std::string);

    const auto id = state.sequence++;
    const auto instance = std::format("s{}", id);
    state.imports.insert(libComponent.filename);
    state.definitions.push_back(std::format("{} {}", libComponent.name, instance));
    mModel.declareInstance(state.component, instance, libComponent.name, {std::nullopt, strategy->span});
    for (size_t i = 0; i < items.size(); ++i)
    {
      auto child = lowerStrategy(flow, items[i], state);
      if (!child.IsSuccess())
        return child;

      state.connections.push_back({.lhs = std::format("{}.action{}", instance, i), .rhs = child.Value()});
    }
    return instance + ".api";
  }
  else if (auto p = std::get_if<ir::Strategy::Join>(&strategy->value))
  {
    const auto instance = std::format("p{}", state.join++);
    const auto count = p->items.size();

    LibraryComponent libComponent;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(libComponent, createParallelComponent(mModel, mOptions.outputDir, count, flow.symbol), std::string);

    state.imports.insert(libComponent.filename);
    state.definitions.push_back(std::format("{} {}", libComponent.name, instance));
    mModel.declareInstance(state.component, instance, libComponent.name, {std::nullopt, strategy->span});

    for (size_t i = 0; i < count; ++i)
    {
      auto child = lowerStrategy(flow, p->items[i], state);
      if (!child.IsSuccess())
        return child;

      state.connections.push_back({.lhs = std::format("{}.action{}", instance, i), .rhs = child.Value()});
    }

    return instance + ".api";
  }
  else if (std::holds_alternative<ir::Strategy::Either>(strategy->value))
  {
    return Result<std::string>::Failed("Dezyne lowering: either is not implemented at " + strategy->span.toString());
  }
  else if (auto p = std::get_if<ir::Strategy::Within>(&strategy->value))
  {
    auto body = lowerStrategy(flow, p->body, state);
    if (!body.IsSuccess())
      return body;

    auto fallback = lowerStrategy(flow, p->fallback, state);
    if (!fallback.IsSuccess())
      return fallback;

    RETURN_ON_FAILURE_AS(createAlarmComponent(mModel, mOptions.outputDir), std::string);
    RETURN_ON_FAILURE_AS(createAlarmInterface(mModel, mOptions.outputDir), std::string);

    LibraryComponent libWithin;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(libWithin, createWithinComponent(mModel, mOptions.outputDir, p->seconds, flow.symbol), std::string);

    const auto instance = std::format("w{}", state.within++);
    const auto alarm = std::format("alarm{}", state.alarm++);
    state.imports.insert(libWithin.filename);
    state.imports.insert("ialarm.dzn");
    state.definitions.push_back(libWithin.name + instance);
    mModel.declareInstance(state.component, instance, libWithin.name, {std::nullopt, strategy->span});
    mModel.declarePort(state.component, alarm, PortDirection::Requires, PortProtocol::Alarm, {std::nullopt, strategy->span});
    state.alarms.push_back(alarm);
    state.connections.push_back({.lhs = instance + ".actionDo", .rhs = body.Value()});
    state.connections.push_back({.lhs = instance + ".actionElse", .rhs = fallback.Value()});
    state.connections.push_back({.lhs = instance + ".alarm", .rhs = alarm});

    return instance + ".api";
  }
  else if (auto p = std::get_if<ir::Strategy::Repeat>(&strategy->value))
  {
    auto endpoint = lowerStrategy(flow, p->body, state);
    if (!endpoint.IsSuccess())
      return endpoint;

    std::string current = endpoint.Value();
    for (const auto& handler : p->handlers)
    {
      state.previous = current;
      auto wrapped = lowerHandler(flow, handler, state);
      if (!wrapped.IsSuccess())
        return wrapped;

      if (handler->kind == ir::HandlerKind::Error)
        RETURN_ON_FAILURE_AS(createErrorHandlerComponent(mModel, mOptions.outputDir, flow.symbol), std::string);
      else if (handler->kind == ir::HandlerKind::Abort)
        RETURN_ON_FAILURE_AS(createAbortHandlerComponent(mModel, mOptions.outputDir, flow.symbol), std::string);

      current = wrapped.Value();
    }

    const bool every = p->iterations > 0;
    const auto count = every ? state.every++ : state.repeat++;
    const auto instance = std::format("{}{}", every ? "e" : "r", count);
    const auto type = every ? "cevery" : "crepeat";
    state.imports.insert(every ? "every.dzn" : "repeat.dzn");
    state.definitions.push_back(std::string(type) + " " + instance);
    mModel.declareInstance(state.component, instance, type, {std::nullopt, strategy->span});
    state.connections.push_back({.lhs = instance + ".action", .rhs = current});
    if (every)
    {
      const auto alarm = std::format("alarm{}", state.alarm++);
      state.imports.insert("ialarm.dzn");
      mModel.declarePort(state.component, alarm, PortDirection::Requires, PortProtocol::Alarm, {std::nullopt, strategy->span});
      state.alarms.push_back(alarm);
      state.connections.push_back({.lhs = instance + ".alarm", .rhs = alarm});

      RETURN_ON_FAILURE_AS(createAlarmComponent(mModel, mOptions.outputDir), std::string);
      RETURN_ON_FAILURE_AS(createAlarmInterface(mModel, mOptions.outputDir), std::string);
      RETURN_ON_FAILURE_AS(createEveryComponent(mModel, mOptions.outputDir, flow.symbol), std::string);
    }
    else
    {
      RETURN_ON_FAILURE_AS(createRepeatComponent(mModel, mOptions.outputDir, flow.symbol), std::string);
    }
    return instance + ".api";
  }
  else if (std::holds_alternative<ir::Strategy::End>(strategy->value))
  {
    return std::string("end");
  }
  else if (std::holds_alternative<ir::Strategy::Failure>(strategy->value))
  {
    LibraryComponent libFailure;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(libFailure, createFailureComponent(mModel, mOptions.outputDir, flow.symbol), std::string);

    const auto instance = std::format("f{}", state.failure++);
    state.imports.insert(libFailure.filename);
    state.definitions.push_back(libFailure.name + instance);
    mModel.declareInstance(state.component, instance, libFailure.name, {std::nullopt, strategy->span});
    return instance + ".api";
  }
  else if (std::holds_alternative<ir::Strategy::Continue>(strategy->value))
  {
    return std::string("continue");
  }
  else if (auto p = std::get_if<ir::Strategy::Call>(&strategy->value))
  {
    auto endpoint = lowerCall(p->call, state, false, strategy->id);
    if (!endpoint.IsSuccess())
      return endpoint;

    std::string current = endpoint.Value();
    for (const auto& handler : p->handlers)
    {
      state.previous = current;
      auto wrapped = lowerHandler(flow, handler, state);

      if (!wrapped.IsSuccess())
        return wrapped;

      if (handler->kind == ir::HandlerKind::Error)
        RETURN_ON_FAILURE_AS(createErrorHandlerComponent(mModel, mOptions.outputDir, flow.symbol), std::string);
      else if (handler->kind == ir::HandlerKind::Abort)
        RETURN_ON_FAILURE_AS(createAbortHandlerComponent(mModel, mOptions.outputDir, flow.symbol), std::string);

      current = wrapped.Value();
    }

    return current;
  }
  else if (auto p = std::get_if<ir::Strategy::Choose>(&strategy->value))
  {
    if (p->options.empty())
      return Result<std::string>::Failed("Dezyne lowering: choose contains no branches at " + strategy->span.toString());

    const auto id = state.choose++;
    const auto instance = std::format("ch{}", id);

    std::vector<std::string> endpoints;
    endpoints.reserve(p->options.size());

    std::size_t conditionCount = 0;
    bool seenElse = false;

    for (std::size_t branchIndex = 0; branchIndex < p->options.size(); ++branchIndex)
    {
      const auto& branch = p->options[branchIndex];

      // ------------------------------------------------------------
      // Lower the branch body.
      auto child = lowerStrategy(flow, branch.strategy, state);

      if (!child.IsSuccess())
        return child;

      endpoints.push_back(child.Value());

      // ------------------------------------------------------------
      // A branch with a condition gets an icondition input.
      if (branch.condition)
      {
        if (seenElse)
          return Result<std::string>::Failed("Dezyne lowering: conditional branch after else in choose at " + strategy->span.toString());

        // conditionIndex is LOCAL to this selector:
        //
        // selector.condition0
        // selector.condition1
        // ...
        const auto selectorConditionIndex = conditionCount++;

        // conditionName must be unique within the containing flow.
        const auto flowConditionIndex = state.condition++;
        const auto conditionName = std::format("condition{}", flowConditionIndex);

        state.conditions.push_back({
            .localPort = conditionName,
            .expression = *branch.condition,
            .span = branch.condition->span,
        });

        state.connections.push_back({
            .lhs = std::format("{}.condition{}", instance, selectorConditionIndex),
            .rhs = conditionName,
            .span = branch.condition->span,
            .kind = PortProtocol::Condition,
        });
      }
      else
      {
        // No condition means the default/else branch.
        if (seenElse)
          return Result<std::string>::Failed("Dezyne lowering: choose contains multiple else branches at " + strategy->span.toString());

        seenElse = true;

        if (branchIndex != p->options.size() - 1)
          return Result<std::string>::Failed("Dezyne lowering: else branch must be last in choose at " + strategy->span.toString());
      }
    }

    // For now we require a total choose.
    if (!seenElse)
      return Result<std::string>::Failed("Dezyne lowering: choose requires an else branch at " + strategy->span.toString());

    // --------------------------------------------------------------
    // Create selector component.
    RETURN_ON_FAILURE_AS(createSelectorComponent(mModel, mOptions.outputDir, conditionCount, endpoints.size(), flow.symbol), std::string);

    const auto selectoName = std::format("selector{}", endpoints.size());
    const auto type = std::format("c{}", selectoName);
    state.imports.insert(std::format("{}.dzn", selectoName));
    state.definitions.push_back(std::format("{} {}", type, instance));
    mModel.declareInstance(state.component, instance, type, {std::nullopt, strategy->span});

    // --------------------------------------------------------------
    // Connect branch actions.
    for (std::size_t i = 0; i < endpoints.size(); ++i)
    {
      state.connections.push_back({
          .lhs = std::format("{}.flow{}", instance, i),
          .rhs = endpoints[i],
          .span = strategy->span,
          .kind = PortProtocol::Action,
      });
    }

    return instance + ".api";
  }

  return Result<std::string>::Failed("Unknown strategy in Dezyne lowering");
}

Result<std::string> LoweringPass::lowerExpression(const ir::Flow& flow, const ir::PExpression& expression, FlowState& state)
{
  if (auto expr = std::get_if<ir::Expression::Literal>(&expression->value))
  {
    // Literals are copied directly
    return expr->text;
  }
  else if (auto expr = std::get_if<ir::Expression::DataExpr>(&expression->value))
  {
    return std::format("{}.{}", expr->capability, expr->data);
  }
  else if (auto expr = std::get_if<ir::Expression::Reference>(&expression->value))
  {
    return std::format("{}", expr->symbol);
  }
  else if (auto expr = std::get_if<ir::Expression::CallExpr>(&expression->value))
  {
    std::string toReturn;
    auto call = expr->call;
    if (call.receiver != InvalidSymbol)
    {
      toReturn += sourceName(call.receiver);
      toReturn += ".";
    }

    toReturn += sourceName(call.target);
    toReturn += "()";
    return toReturn;
  }
  else if (auto expr = std::get_if<ir::Expression::Unary>(&expression->value))
  {
    auto subexpr = lowerExpression(flow, expr->value, state);
    RETURN_ON_FAILURE(subexpr);
    return std::format("{}{}", expr->op, subexpr.Value());
  }
  else if (auto expr = std::get_if<ir::Expression::Binary>(&expression->value))
  {
    auto left = lowerExpression(flow, expr->lhs, state);
    RETURN_ON_FAILURE(left);
    auto right = lowerExpression(flow, expr->rhs, state);
    RETURN_ON_FAILURE(right);
    return std::format("{} {} {}", left.Value(), expr->op, right.Value());
  }
  else if (auto expr = std::get_if<ir::Expression::RecordLiteral>(&expression->value))
  {
    std::string toReturn = "{";
    bool first = true;
    for (const auto& field : expr->fields)
    {
      if (!first)
        toReturn += ", ";

      auto subexpr = lowerExpression(flow, field.value, state);
      RETURN_ON_FAILURE(subexpr);
      toReturn += std::format("{} = {}", field.name, subexpr.Value());
    }
    return toReturn + "}";
  }
  else if (auto expr = std::get_if<ir::Expression::ListLiteral>(&expression->value))
  {
    std::string toReturn = "[";
    bool first = true;
    for (const auto& field : expr->fields)
    {
      if (!first)
        toReturn += ", ";

      auto subexpr = lowerExpression(flow, field, state);
      RETURN_ON_FAILURE(subexpr);
      toReturn += subexpr.Value();
    }
    return toReturn + "]";
  }
  else if (auto expr = std::get_if<ir::Expression::MapLiteral>(&expression->value))
  {
    std::string toReturn = "{";
    bool first = true;
    for (const auto& field : expr->fields)
    {
      if (!first)
        toReturn += ", ";

      auto keyexpr = lowerExpression(flow, field.key, state);
      RETURN_ON_FAILURE(keyexpr);
      auto subexpr = lowerExpression(flow, field.value, state);
      RETURN_ON_FAILURE(subexpr);
      toReturn += std::format("{} = {}", keyexpr.Value(), subexpr.Value());
    }
    return toReturn + "}";
  }

  return Result<std::string>::Failed("Unknown expression type at {}", expression->span.toString());
}

Result<std::string> LoweringPass::lowerHandler(const ir::Flow& flow, const ir::PHandler& handler, FlowState& state)
{
  if (!handler)
    return Result<std::string>::Failed("Invalid Dezyne handler");

  if (handler->kind == ir::HandlerKind::Abort || handler->kind == ir::HandlerKind::Error)
  {
    const bool abort = handler->kind == ir::HandlerKind::Abort;
    const auto instance = std::format("{}{}", abort ? "ah" : "fh", abort ? state.abortHandler++ : state.errorHandler++);
    const auto type = abort ? "cabort_handler" : "cerror_handler";
    state.imports.insert(abort ? "abort_handler.dzn" : "error_handler.dzn");
    state.definitions.push_back(std::string(type) + " " + instance);
    mModel.declareInstance(state.component, instance, type, {std::nullopt, handler->span});
    auto body = lowerStrategy(flow, handler->body, state);
    if (!body.IsSuccess())
      return body;
    state.connections.push_back({.lhs = instance + ".action", .rhs = state.previous});
    state.connections.push_back({.lhs = instance + ".handler", .rhs = body.Value()});
    return instance + ".api";
  }

  if (!handler->emitter)
    return Result<std::string>::Failed("Emitter handler has no signal call");

  const bool cont = handler->kind == ir::HandlerKind::EmitterContinue;
  const auto instance = std::format("sh{}", state.signalHandler++);
  const auto type = cont ? "csignal_continue" : "csignal_handler";
  state.imports.insert(cont ? "signal_continue.dzn" : "signal_handler.dzn");
  state.definitions.push_back(std::string(type) + " " + instance);
  mModel.declareInstance(state.component, instance, type, {std::nullopt, handler->span});
  auto signal = lowerCall(*handler->emitter, state, true, "");
  if (!signal.IsSuccess())
    return signal;

  auto body = lowerStrategy(flow, handler->body, state);
  if (!body.IsSuccess())
    return body;

  state.connections.push_back({.lhs = instance + ".signal", .rhs = signal.Value()});
  state.connections.push_back({.lhs = instance + ".action", .rhs = state.previous});
  state.connections.push_back({.lhs = instance + ".handler", .rhs = body.Value()});
  return instance + ".api";
}

Result<std::string> LoweringPass::lowerCall(const ir::Call& call, FlowState& state, bool signal, const std::string& traceId)
{
  if (call.kind == ir::CallKind::Flow)
  {
    if (signal)
      return Result<std::string>::Failed("Flow call cannot be used as a signal");

    const auto local = sourceName(call.target);

    state.calls.push_back({
        .kind = CallUse::Kind::Flow,
        .flow = state.flow,
        .localPort = local,
        .receiver = InvalidSymbol,
        .target = call.target,
        .localOrdinal = 0,
        .targetOrdinal = 0,
        .arguments = call.arguments,
        .inputSlots = call.inputSlots,
        .outputSlots = call.outputSlots,
        .traceId = traceId,
        .span = call.span,
    });

    return local;
  }

  // Trigger and In events both become callable iaction ports and may occur
  // multiple times.
  if (mActionEvents.contains(call.target))
  {
    if (signal)
      return Result<std::string>::Failed("Action event cannot be used as a signal");

    // Local ordinal names the required port inside this flow.
    const auto localOrdinal = ++state.eventOrdinals[call.target];

    // Target ordinal selects the corresponding numbered capability port at
    // task level.
    const auto targetOrdinal = ++mCallOrdinals[call.target];

    const auto count = std::max<std::uint32_t>(1, mCallCounts[call.target]);

    std::string base;
    if (call.kind == ir::CallKind::CapabilityTrigger)
    {
      // Preserve the historical trigger naming: drive_1, drive_2, ...
      base = sourceName(call.receiver);
    }
    else
    {
      // A capability may expose multiple In events, so include the event:
      // siren_start, siren_stop, ...
      base = std::format("{}_{}", sourceName(call.receiver), sourceName(call.target));
    }

    const auto local = count == 1 ? base : std::format("{}_{}", base, localOrdinal);

    state.calls.push_back({
        .kind = call.kind == ir::CallKind::CapabilityTrigger ? CallUse::Kind::Trigger : CallUse::Kind::Action,

        .flow = state.flow,
        .localPort = local,
        .receiver = call.receiver,
        .target = call.target,

        .localOrdinal = localOrdinal,
        .targetOrdinal = targetOrdinal,

        .arguments = call.arguments,
        .inputSlots = call.inputSlots,
        .outputSlots = call.outputSlots,

        .traceId = traceId,
        .span = call.span,
    });

    return local;
  }

  // Abort/action/signal resources that do not have multiplicity keep one
  // flow-local port. Repeated internal users will be connected through a
  // flow-local arbiter.
  bool isAbort = mAbortEvents.contains(call.target);
  const auto local = std::format("{}_{}", sourceName(call.receiver), sourceName(call.target));
  if (isAbort && !signal)
  {
    const auto abortPort = local;
    const auto abortCall = std::format("acall{}", state.abortCall++);

    RETURN_ON_FAILURE_AS(createAbortCallComponent(mModel, mOptions.outputDir, state.component), std::string);

    state.imports.insert("iabort.dzn");
    state.imports.insert("abort_call.dzn");
    mModel.declareInstance(state.component, abortCall, "cabort_call", {call.target, call.span});

    // Internal orchestration uses iaction. cabort_call converts that to the capability-level iabort.
    state.connections.push_back({
        .lhs = abortCall + ".action",
        .rhs = abortPort,
        .span = call.span,
        .kind = PortProtocol::Abort,
    });

    state.calls.push_back({
        .kind = CallUse::Kind::Abort,
        .flow = state.flow,
        .localPort = abortPort,
        .receiver = call.receiver,
        .target = call.target,
        .localOrdinal = 0,
        .targetOrdinal = 0,
        .arguments = call.arguments,
        .inputSlots = call.inputSlots,
        .outputSlots = call.outputSlots,
        .traceId = traceId,
        .span = call.span,
    });

    return abortCall + ".api";
  }

  state.calls.push_back({
      .kind = signal ? CallUse::Kind::Signal : CallUse::Kind::Action,
      .flow = state.flow,
      .localPort = local,
      .receiver = call.receiver,
      .target = call.target,
      .localOrdinal = 0,
      .targetOrdinal = 0,
      .arguments = call.arguments,
      .inputSlots = call.inputSlots,
      .outputSlots = call.outputSlots,
      .traceId = traceId,
      .span = call.span,
  });

  return local;
}

VoidResult LoweringPass::createNecessaryArbiter(SymbolId componentId, std::vector<Connection>& connections)
{
  struct ClientInfo
  {
    PortProtocol kind;
    std::size_t size;
  };

  std::map<std::string, std::vector<ClientInfo>> clients;

  for (std::size_t i = 0; i < connections.size(); ++i)
    if (connections[i].lhs != "api")
      clients[connections[i].rhs].push_back({.kind = connections[i].kind, .size = i});

  std::uint32_t actionArbiterId = 0;
  std::uint32_t abortArbiterId = 0;

  for (const auto& [resource, uses] : clients)
  {
    if (uses.size() < 2)
      continue;

    LOG_DEBUG("Creating arbiter for {} with kind {}", resource, portToString(uses.front().kind));
    if (uses.at(0).kind == PortProtocol::Abort)
    {
      RETURN_ON_FAILURE(createAbortArbiterComponent(mModel, mOptions.outputDir, uses.size(), componentId));

      const auto name = std::format("abort_arbiter{}_{}", uses.size(), abortArbiterId++);
      mModel.declareInstance(componentId, name, std::format("cabort_arbiter{}", uses.size()), {componentId});

      for (std::size_t i = 0; i < uses.size(); ++i)
        connections[uses[i].size].rhs = std::format("{}.client{}", name, i);

      connections.push_back({
          .lhs = name + ".resource",
          .rhs = resource,
          .span = connections[uses.front().size].span,
          .kind = PortProtocol::Abort,
      });
    }
    else
    {
      RETURN_ON_FAILURE(createActionArbiterComponent(mModel, mOptions.outputDir, uses.size(), componentId));

      const auto name = std::format("arbiter{}_{}", uses.size(), actionArbiterId++);
      mModel.declareInstance(componentId, name, std::format("caction_arbiter{}", uses.size()), {componentId});

      for (std::size_t i = 0; i < uses.size(); ++i)
        connections[uses[i].size].rhs = std::format("{}.client{}", name, i);

      connections.push_back({
          .lhs = name + ".resource",
          .rhs = resource,
          .span = connections[uses.front().size].span,
      });
    }
  }

  return VoidResult();
}

PortProtocol LoweringPass::protocolOfResource(SymbolId componentId, const std::string& resource) const
{
  const auto dot = resource.find('.');

  // Resource is a port directly on this component:
  //
  //   drive_abort
  if (dot == std::string::npos)
  {
    if (const auto* port = mModel.findPort(componentId, resource))
      return port->protocol;

    return PortProtocol::Unknown;
  }

  // Resource is a port on an instance:
  //
  //   drivearmour.abort
  const auto instanceName = resource.substr(0, dot);
  const auto portName = resource.substr(dot + 1);

  const auto* component = mModel.getComponent(componentId);
  if (!component)
    return PortProtocol::Unknown;

  const auto instanceIt = std::find_if(component->instances.begin(), component->instances.end(), [&](const auto& instance) {
    const auto* symbol = mModel.mSymbols.get(instance.symbol);

    return symbol && symbol->name == instanceName;
  });

  if (instanceIt == component->instances.end())
    return PortProtocol::Unknown;

  const auto* instanceType = mModel.findComponent(instanceIt->typeName);

  if (!instanceType)
    return PortProtocol::Unknown;

  if (const auto* port = mModel.findPort(instanceType->symbol, portName))
    return port->protocol;

  return PortProtocol::Unknown;
}

void LoweringPass::countTriggers(const ir::PStrategy& strategy)
{
  if (!strategy)
    return;

  if (auto p = std::get_if<ir::Strategy::Sequence>(&strategy->value))
  {
    for (const auto& x : p->items)
      countTriggers(x);
  }
  else if (auto p = std::get_if<ir::Strategy::Join>(&strategy->value))
  {
    for (const auto& x : p->items)
      countTriggers(x);
  }
  else if (auto p = std::get_if<ir::Strategy::Either>(&strategy->value))
  {
    for (const auto& x : p->items)
      countTriggers(x);
  }
  else if (auto p = std::get_if<ir::Strategy::Within>(&strategy->value))
  {
    countTriggers(p->body);
    countTriggers(p->fallback);
    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
  else if (auto p = std::get_if<ir::Strategy::Repeat>(&strategy->value))
  {
    countTriggers(p->body);
    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
  else if (auto p = std::get_if<ir::Strategy::Call>(&strategy->value))
  {
    if (p->call.kind != ir::CallKind::Flow && mActionEvents.contains(p->call.target))
      ++mCallCounts[p->call.target];

    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
}

void LoweringPass::countHandlerTriggers(const ir::PHandler& handler)
{
  if (!handler)
    return;

  if (handler->emitter && mActionEvents.contains(handler->emitter->target))
    ++mCallCounts[handler->emitter->target];

  countTriggers(handler->body);
}

bool LoweringPass::usesCapabilityAbort(SymbolId receiver) const
{
  LOG_TRACE("Looking for abort use with receiver: {}", receiver);
  for (const auto& [_, flow] : mFlows)
  {
    for (const auto& call : flow.calls)
    {
      LOG_TRACE("  Receiver {} vs {} and type: {}", call.receiver, receiver, (int)call.kind);
      if (call.kind == CallUse::Kind::Abort && call.receiver == receiver)
        return true;
    }
  }

  return false;
}

std::string LoweringPass::sourceName(koda::SymbolId id) const
{
  const auto* symbol = mSymbols.get(id);
  return symbol ? symbol->name : "unknown";
}

std::string LoweringPass::lower(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return value;
}

std::string LoweringPass::componentName(const std::string& name)
{
  return "c" + lower(name);
}

std::string LoweringPass::flowName(const std::string& name)
{
  return "f" + lower(name);
}

}  // namespace koda::dezyne