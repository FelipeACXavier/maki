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

  // First determine which event symbols correspond to callable Dezyne action ports
  for (const auto& component : program.components)
  {
    if (component.kind != ir::ComponentKind::Capability)
      continue;

    for (const auto& event : component.events)
      if (isActionEvent(event.kind))
        mActionEvents.insert(event.symbol);
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

  RETURN_ON_FAILURE(createActionInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createSignalInterface(mModel, mOptions.outputDir));
  RETURN_ON_FAILURE(createTypes(mModel, mOptions.outputDir));

  return VoidResult();
}

VoidResult LoweringPass::lowerCapability(const ir::Component& capability)
{
  auto* component = mModel.findComponent(componentName(capability.name));
  if (!component)
    return VoidResult::Failed("Missing declared Dezyne capability: " + capability.name);

  std::ostringstream out;
  out << "import iaction.dzn;\n";
  out << "import isignal.dzn;\n\n";

  out << std::format("component {} {{\n", componentName(capability.name));

  for (const auto& event : capability.events)
  {
    LOG_TRACE("Declaring event {} of type {} - {}", event.name, (int)event.kind, capability.symbol);
    if (isActionEvent(event.kind))
    {
      const auto count = std::max<std::uint32_t>(1, mCallCounts[event.symbol]);
      for (std::uint32_t i = 0; i < count; ++i)
      {
        const auto name = count == 1 ? event.name : std::format("{}_{}", event.name, i + 1);
        mModel.declarePort(component->symbol, name, PortDirection::Provides, PortProtocol::Action, {event.symbol, event.span});
        out << std::format("  provides iaction {};\n", name);
      }
    }
    else if (event.kind == ir::EventKind::Out)
    {
      out << std::format("  provides isignal {};\n", event.name);
    }
    else if (event.kind == ir::EventKind::Abort)
    {
      out << std::format("  provides iaction {};\n", event.name);
    }
  }
  out << "}\n";
  mModel.setGeneratedFile(component->fileName, out.str(), {capability.symbol, capability.span});
  return {};
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
  std::set<koda::SymbolId> importedCapabilities;
  for (const auto& arg : task.arguments)
    if (arg.type.isNamed())
    {
      const auto named = arg.type.namedType();
      if (named.id && named.id.value() != std::to_string(InvalidSymbol))
        importedCapabilities.insert(std::stoul(named.id.value()));
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

        continue;
      }

      auto targetPort = sourceName(call.target);
      if (mActionEvents.contains(call.target))
      {
        const auto count = std::max<std::uint32_t>(1, mCallCounts[call.target]);
        if (count > 1)
          targetPort = std::format("{}_{}", targetPort, call.targetOrdinal);
      }

      connections.push_back({
          .lhs = flowInstance + "." + call.localPort,
          .rhs = sourceName(call.receiver) + "." + targetPort,
          .span = call.span,
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

  // Make sure the arbiters exists in case they are needed
  RETURN_ON_FAILURE(createNecessaryArbiter(componentId, connections));

  // Emit any instances introduced during topology lowering (alarms/arbiters).
  // out.str("");
  // out.clear();
  out << "import iaction.dzn;\n";
  out << "import isignal.dzn;\n";
  for (auto symbol : importedCapabilities)
    out << std::format("import a_{}.dzn;\n", lower(sourceName(symbol)));

  for (const auto& flow : task.flows)
    out << std::format("import {}.dzn;\n", lower(flow.name));

  if (hasAlarm)
    out << "import alarm.dzn;\n";

  // Helper creation may have reallocated the model's component storage.
  component = mModel.getComponent(componentId);
  if (!component)
    return VoidResult::Failed("Missing Dezyne task after lowering: " + task.name);

  std::vector<std::string> seenImports;
  for (const auto& instance : component->instances)
    if (const auto* s = mModel.mSymbols.get(instance.symbol); s && instance.typeName.starts_with("caction_arbiter"))
    {
      std::string name = std::format("action_arbiter{}", instance.typeName.substr(std::string("caction_arbiter").size()));
      if (std::count(seenImports.begin(), seenImports.end(), name) > 0)
        continue;

      out << std::format("import {}.dzn;\n", name);
      seenImports.push_back(name);
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
    const auto protocol = call.kind == CallUse::Kind::Signal ? PortProtocol::Signal : PortProtocol::Action;
    mModel.declarePort(componentId, call.localPort, PortDirection::Requires, protocol, {call.target, call.span});
  }

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
    if (const auto* symbol = mModel.mSymbols.get(instance.symbol); symbol && instance.typeName.starts_with("caction_arbiter"))
    {
      const auto name = std::format("action_arbiter{}", instance.typeName.substr(std::string("caction_arbiter").size()));

      if (std::count(seenImports.begin(), seenImports.end(), name) > 0)
        continue;

      out << std::format("import {}.dzn;\n", name);
      seenImports.push_back(name);
    }
  }

  out << std::format("\ncomponent {} {{\n", flowName(flow.name));

  for (const auto& port : component->ports)
  {
    const auto* symbol = mModel.mSymbols.get(port.symbol);
    if (!symbol)
      continue;

    const auto direction = port.direction == PortDirection::Provides ? "provides" : "requires";
    const auto protocol = port.protocol == PortProtocol::Signal ? "isignal" : port.protocol == PortProtocol::Alarm ? "ialarm" : "iaction";

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

  return FlowResult{requiredCalls, state.alarms};
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

    RETURN_ON_FAILURE_AS(createSequenceComponent(mModel, mOptions.outputDir, items.size(), flow.symbol), std::string);

    const auto id = state.sequence++;
    const auto instance = std::format("s{}", id);
    state.imports.insert(std::format("sequence{}.dzn", items.size()));
    state.definitions.push_back(std::format("csequence{} {}", items.size(), instance));
    mModel.declareInstance(state.component, instance, std::format("csequence{}", items.size()), {std::nullopt, strategy->span});
    for (std::size_t i = 0; i < items.size(); ++i)
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
    state.imports.insert("parallel.dzn");
    state.definitions.push_back("cparallel " + instance);
    mModel.declareInstance(state.component, instance, "cparallel", {std::nullopt, strategy->span});
    for (std::size_t i = 0; i < count; ++i)
    {
      auto child = lowerStrategy(flow, p->items[i], state);
      if (!child.IsSuccess())
        return child;

      state.connections.push_back({.lhs = std::format("{}.action{}", instance, i + 1),  // TODO: Remoce the +1
                                   .rhs = child.Value()});
    }

    RETURN_ON_FAILURE_AS(createParallelComponent(mModel, mOptions.outputDir, count, flow.symbol), std::string);

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

    const auto instance = std::format("w{}", state.within++);
    const auto alarm = std::format("alarm{}", state.alarm++);
    state.imports.insert("within.dzn");
    state.imports.insert("ialarm.dzn");
    state.definitions.push_back("cwithin " + instance);
    mModel.declareInstance(state.component, instance, "cwithin", {std::nullopt, strategy->span});
    mModel.declarePort(state.component, alarm, PortDirection::Requires, PortProtocol::Alarm, {std::nullopt, strategy->span});
    state.alarms.push_back(alarm);
    state.connections.push_back({.lhs = instance + ".action1", .rhs = body.Value()});
    state.connections.push_back({.lhs = instance + ".action2", .rhs = fallback.Value()});
    state.connections.push_back({.lhs = instance + ".alarm", .rhs = alarm});

    RETURN_ON_FAILURE_AS(createAlarmComponent(mModel, mOptions.outputDir), std::string);
    RETURN_ON_FAILURE_AS(createAlarmInterface(mModel, mOptions.outputDir), std::string);

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
  else if (std::holds_alternative<ir::Strategy::Continue>(strategy->value))
  {
    return std::string("continue");
  }
  else if (auto p = std::get_if<ir::Strategy::Call>(&strategy->value))
  {
    auto endpoint = lowerCall(p->call, state, false);
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
  return Result<std::string>::Failed("Unknown strategy in Dezyne lowering");
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
  auto signal = lowerCall(*handler->emitter, state, true);
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

Result<std::string> LoweringPass::lowerCall(const ir::Call& call, FlowState& state, bool signal)
{
  if (call.kind == ir::CallKind::Flow)
  {
    if (signal)
      return Result<std::string>::Failed("Flow call cannot be used as a signal");

    const auto local = sourceName(call.target);

    state.calls.push_back({
        .kind = CallUse::Kind::Flow,
        .localPort = local,
        .receiver = InvalidSymbol,
        .target = call.target,
        .localOrdinal = 0,
        .targetOrdinal = 0,
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
        .localPort = local,
        .receiver = call.receiver,
        .target = call.target,
        .localOrdinal = localOrdinal,
        .targetOrdinal = targetOrdinal,
        .span = call.span,
    });

    return local;
  }

  // Abort/action/signal resources that do not have multiplicity keep one
  // flow-local port. Repeated internal users will be connected through a
  // flow-local arbiter.
  const auto local = std::format("{}_{}", sourceName(call.receiver), sourceName(call.target));

  state.calls.push_back({
      .kind = signal ? CallUse::Kind::Signal : CallUse::Kind::Action,
      .localPort = local,
      .receiver = call.receiver,
      .target = call.target,
      .localOrdinal = 0,
      .targetOrdinal = 0,
      .span = call.span,
  });

  return local;
}

VoidResult LoweringPass::createNecessaryArbiter(SymbolId componentId, std::vector<Connection>& connections)
{
  std::map<std::string, std::vector<std::size_t>> clients;

  for (std::size_t i = 0; i < connections.size(); ++i)
    if (connections[i].lhs != "api")
      clients[connections[i].rhs].push_back(i);

  std::uint32_t arbiterId = 0;
  for (const auto& [resource, uses] : clients)
  {
    if (uses.size() < 2)
      continue;

    RETURN_ON_FAILURE(createActionArbiterComponent(mModel, mOptions.outputDir, static_cast<std::uint32_t>(uses.size()), componentId));

    const auto name = std::format("arbiter{}_{}", uses.size(), arbiterId++);
    mModel.declareInstance(componentId, name, std::format("caction_arbiter{}", uses.size()), {componentId});

    for (std::size_t i = 0; i < uses.size(); ++i)
      connections[uses[i]].rhs = std::format("{}.client{}", name, i);

    connections.push_back({
        .lhs = name + ".resource",
        .rhs = resource,
        .span = connections[uses.front()].span,
    });
  }

  return {};
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