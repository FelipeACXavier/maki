#include "dezyne_lowering_pass.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <sstream>

#include "dezyne_library_emitter.h"
#include "koda_compiler.h"
#include "logging.h"

namespace koda::dezyne
{
LoweringPass::LoweringPass(Model& model, const SymbolRegistry& symbols, const koda::CompilerOptions& options)
    : mModel(model)
    , mSymbols(symbols)
    , mOptions(options)
{
}

VoidResult LoweringPass::run(const ir::Program& program)
{
  mTriggerCounts.clear();
  mTriggerNames.clear();
  mFlows.clear();

  for (const auto& component : program.components)
    for (const auto& flow : component.flows)
      countTriggers(flow.strategy);

  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Capability)
      RETURN_ON_FAILURE(lowerCapability(component));

  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Task)
      RETURN_ON_FAILURE(lowerTask(component));

  return {};
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
    if (event.kind == ir::EventKind::Trigger)
    {
      mTriggerNames[capability.symbol] = event.name;
      const auto count = std::max<std::uint32_t>(1, mTriggerCounts[capability.symbol]);
      for (std::uint32_t i = 0; i < count; ++i)
      {
        const auto name = count == 1 ? event.name : std::format("{}_{}", event.name, i + 1);
        mModel.declarePort(component->symbol, name, PortDirection::Provides, PortProtocol::Action, {event.symbol, event.span});
        out << std::format("  provides iaction {};\n", name);
      }
    }
    else if (event.kind == ir::EventKind::Out)
      out << std::format("  provides isignal {};\n", event.name);
    else if (event.kind == ir::EventKind::Abort)
      out << std::format("  provides iaction {};\n", event.name);
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

  auto* component = mModel.findComponent(componentName(task.name));
  if (!component)
    return VoidResult::Failed("Missing declared Dezyne task: " + task.name);

  std::ostringstream out;
  // out << "import iaction.dzn;\n";
  // out << "import isignal.dzn;\n";

  std::set<koda::SymbolId> importedCapabilities;
  for (const auto& arg : task.arguments)
    if (arg.type.kind == TypeKind::Component && arg.type.symbol != koda::InvalidSymbol)
      importedCapabilities.insert(arg.type.symbol);

  // for (auto symbol : importedCapabilities)
  //   out << std::format("import a_{}.dzn;\n", lower(sourceName(symbol)));

  // for (const auto& flow : task.flows)
  //   out << std::format("import {}.dzn;\n", lower(flow.name));

  bool hasAlarm = false;
  for (const auto& [_, result] : mFlows)
    hasAlarm = hasAlarm || !result.alarms.empty();

  // if (hasAlarm)
  //   out << "import alarm.dzn;\n";

  // out << std::format("\ncomponent {} {{\n", componentName(task.name));
  // out << "  provides iaction api;\n\n  system {\n";

  // for (const auto& instance : component->instances)
  // {
  //   const auto* symbol = mModel.mSymbols.get(instance.symbol);
  //   if (symbol)
  //     out << std::format("    {} {};\n", instance.typeName, symbol->name);
  // }

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
      // Not only with trigger, but any port
      if (call.kind == CallUse::Kind::Trigger)
      {
        targetPort = triggerName(call.target);
        const auto count = std::max<std::uint32_t>(1, mTriggerCounts[call.target]);
        if (count > 1)
          targetPort = std::format("{}_{}", targetPort, call.ordinal);
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
      LOG_DEBUG("Adding alarm {} to flow instance {}", alarmName, flowInstance);
      mModel.declareInstance(component->symbol, alarmName, "calarm", {component->symbol, flow.span});
      connections.push_back({
          .lhs = flowInstance + "." + alarm,  // The port remains the same since that is set based on the
          .rhs = alarmName + ".api",
          .span = flow.span,
      });
    }
  }

  // Make sure the arbiters exists in case they are needed
  RETURN_ON_FAILURE(createNecessaryArbiter(*component, connections));

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
  {
    out << "import alarm.dzn;\n";
    RETURN_ON_FAILURE(ensureAlarmHelper());
  }

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
    mModel.declareConnection(component->symbol,
                             std::to_string(connectionId++),
                             connection.lhs,
                             connection.rhs, {component->symbol, connection.span});
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
    // Repeated trigger calls need separate ports:
    // drive_1 and drive_2.
    const std::uint32_t ordinal = call.kind == CallUse::Kind::Trigger ? call.ordinal : 0;
    const auto key = std::make_tuple(call.kind, call.receiver, call.target, ordinal);

    if (seen.insert(key).second)
      result.push_back(call);
  }

  return result;
}

Result<LoweringPass::FlowResult> LoweringPass::lowerFlow(const ir::Flow& flow)
{
  auto* component = mModel.findComponent(flowName(flow.name));
  if (!component)
    return Result<FlowResult>::Failed("Missing declared Dezyne flow: " + flow.name);

  FlowState state;
  state.component = component->symbol;
  auto endpoint = lowerStrategy(flow, flow.strategy, state);
  if (!endpoint.IsSuccess())
    return Result<FlowResult>::Failed(endpoint.ErrorMessage());

  // Make sure the arbiters exists in case they are needed
  RETURN_ON_FAILURE_AS(createNecessaryArbiter(*component, state.connections), LoweringPass::FlowResult);

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
    if (const auto* s = mModel.mSymbols.get(instance.symbol); s && instance.typeName.starts_with("caction_arbiter"))
    {
      std::string name = std::format("action_arbiter{}", instance.typeName.substr(std::string("caction_arbiter").size()));
      if (std::count(seenImports.begin(), seenImports.end(), name) > 0)
        continue;

      out << std::format("import {}.dzn;\n", name);
      seenImports.push_back(name);
    }

  out << std::format("\ncomponent {} {{\n", flowName(flow.name));
  for (const auto& port : component->ports)
  {
    const auto* symbol = mModel.mSymbols.get(port.symbol);
    if (!symbol)
      continue;

    const auto direction = port.direction == PortDirection::Provides ? "provides" : "requires";
    const auto protocol = port.protocol == PortProtocol::Signal  ? "isignal"
                          : port.protocol == PortProtocol::Alarm ? "ialarm"
                                                                 : "iaction";
    out << std::format("  {} {} {};\n", direction, protocol, symbol->name);
  }

  std::map<std::string, std::vector<std::size_t>> clients;
  for (std::size_t i = 0; i < state.connections.size(); ++i)
    if (state.connections[i].lhs != "api")
      clients[state.connections[i].rhs].push_back(i);

  out << "\n  system {\n";
  for (const auto& instance : component->instances)
    if (const auto* symbol = mModel.mSymbols.get(instance.symbol))
      out << std::format("    {} {};\n", instance.typeName, symbol->name);

  out << "\n    api <=> " << endpoint.Value() << ";\n";
  std::string current = "api";
  for (const auto& connection : state.connections)
  {
    auto leftPort = portFromString(connection.lhs);
    if (leftPort.instance != current)
    {
      out << "\n";
      current = leftPort.instance;
    }

    out << "    " << connection.lhs << " <=> " << connection.rhs << ";\n";
  }

  out << "  }\n}\n";

  mModel.setGeneratedFile(component->fileName, out.str(), {flow.symbol, flow.span});

  return FlowResult{uniqueRequiredPorts(state), state.alarms};
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

    {
      auto helper = ensureSequenceHelper(flow, static_cast<std::uint32_t>(items.size()));
      if (!helper.IsSuccess())
        return Result<std::string>::Failed(helper.ErrorMessage());
    }

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

      state.connections.push_back({.lhs = std::format("{}.action{}", instance, i),
                                   .rhs = child.Value()});
    }
    return instance + ".api";
  }
  if (auto p = std::get_if<ir::Strategy::Join>(&strategy->value))
  {
    const auto instance = std::format("p{}", state.join++);
    state.imports.insert("parallel.dzn");
    state.definitions.push_back("cparallel " + instance);
    mModel.declareInstance(state.component, instance, "cparallel", {std::nullopt, strategy->span});
    for (std::size_t i = 0; i < p->items.size(); ++i)
    {
      auto child = lowerStrategy(flow, p->items[i], state);
      if (!child.IsSuccess())
        return child;

      state.connections.push_back({.lhs = std::format("{}.action{}", instance, i + 1),  // TODO: Remoce the +1
                                   .rhs = child.Value()});
    }
    return instance + ".api";
  }
  if (std::holds_alternative<ir::Strategy::Either>(strategy->value))
    return Result<std::string>::Failed("Dezyne lowering: either is not implemented at " + strategy->span.toString());
  if (std::holds_alternative<ir::Strategy::Let>(strategy->value))
    return Result<std::string>::Failed("Dezyne lowering: let is not implemented at " + strategy->span.toString());
  if (auto p = std::get_if<ir::Strategy::Within>(&strategy->value))
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
    state.connections.push_back({.lhs = instance + ".action1",
                                 .rhs = body.Value()});
    state.connections.push_back({.lhs = instance + ".action2",
                                 .rhs = fallback.Value()});
    state.connections.push_back({.lhs = instance + ".alarm",
                                 .rhs = alarm});
    return instance + ".api";
  }
  if (std::holds_alternative<ir::Strategy::IfElse>(strategy->value))
    return Result<std::string>::Failed("Dezyne lowering: if/else is not implemented at " + strategy->span.toString());
  if (auto p = std::get_if<ir::Strategy::Repeat>(&strategy->value))
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
      current = wrapped.Value();
    }
    const bool every = p->iterations > 0;
    const auto instance = std::format("{}{}", every ? "e" : "r", every ? state.every++ : state.repeat++);
    const auto type = every ? "cevery" : "crepeat";
    state.imports.insert(every ? "every.dzn" : "repeat.dzn");
    state.definitions.push_back(std::string(type) + " " + instance);
    mModel.declareInstance(state.component, instance, type, {std::nullopt, strategy->span});
    state.connections.push_back({.lhs = instance + ".action1",
                                 .rhs = current});
    if (every)
    {
      const auto alarm = std::format("alarm{}", state.alarm++);
      state.imports.insert("ialarm.dzn");
      mModel.declarePort(state.component, alarm, PortDirection::Requires, PortProtocol::Alarm, {std::nullopt, strategy->span});
      state.alarms.push_back(alarm);
      state.connections.push_back({.lhs = instance + ".alarm",
                                   .rhs = alarm});
    }
    return instance + ".api";
  }
  if (std::holds_alternative<ir::Strategy::Guard>(strategy->value))
    return Result<std::string>::Failed("Dezyne lowering: guard is not implemented at " + strategy->span.toString());
  if (std::holds_alternative<ir::Strategy::End>(strategy->value))
    return std::string("end");
  if (std::holds_alternative<ir::Strategy::Continue>(strategy->value))
    return std::string("continue");
  if (auto p = std::get_if<ir::Strategy::FlowRef>(&strategy->value))
  {
    const auto port = sourceName(p->flow);
    state.calls.push_back({
        .kind = CallUse::Kind::Flow,
        .localPort = port,
        .receiver = koda::InvalidSymbol,  // Flow has no receiver
        .target = p->flow,
        .ordinal = 0,
        .span = strategy->span,
    });
    return port;
  }
  if (auto p = std::get_if<ir::Strategy::TaskCall>(&strategy->value))
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
    state.connections.push_back({.lhs = instance + ".action",
                                 .rhs = state.previous});
    state.connections.push_back({.lhs = instance + ".handler",
                                 .rhs = body.Value()});
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

  state.connections.push_back({.lhs = instance + ".signal",
                               .rhs = signal.Value()});
  state.connections.push_back({.lhs = instance + ".action",
                               .rhs = state.previous});
  state.connections.push_back({.lhs = instance + ".handler",
                               .rhs = body.Value()});
  return instance + ".api";
}

Result<std::string> LoweringPass::lowerCall(const ir::Call& call, FlowState& state, bool signal)
{
  if (call.kind == ir::CallKind::CapabilityTrigger)
  {
    if (signal)
      return Result<std::string>::Failed("Capability trigger cannot be used as a signal");

    const auto ordinal = ++mTriggerCount[call.receiver];
    const auto localOrdinal = ++state.triggerOrdinals[call.receiver];
    const auto local = std::format("{}_{}", sourceName(call.receiver), localOrdinal);
    state.calls.push_back({
        .kind = CallUse::Kind::Trigger,
        .localPort = local,
        .receiver = call.receiver,
        .target = call.target,
        .ordinal = ordinal,
        .span = call.span,
    });
    return local;
  }

  const auto local = std::format("{}_{}", sourceName(call.receiver), sourceName(call.target));
  state.calls.push_back({
      .kind = signal ? CallUse::Kind::Signal : CallUse::Kind::Action,
      .localPort = local,
      .receiver = call.receiver,
      .target = call.target,
      .ordinal = 0,
      .span = call.span,
  });
  return local;
}

VoidResult LoweringPass::ensureSequenceHelper(const ir::Flow& flow, std::uint32_t count)
{
  const auto name = std::format("csequence{}", count);
  const auto path = std::format("{}/sequence{}.dzn", mOptions.outputDir, count);
  const auto sequence = mModel.declareComponent(name, path, {flow.symbol}, true, flow.symbol);
  mModel.declarePort(sequence, "api", PortDirection::Provides, PortProtocol::Action);

  for (std::uint32_t i = 0; i < count; ++i)
    mModel.declarePort(sequence, std::format("action{}", i), PortDirection::Requires, PortProtocol::Action);

  std::ostringstream out;
  RETURN_ON_FAILURE(createSequenceComponent(count, out));

  mModel.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult LoweringPass::ensureArbiterHelper(const Component& component, std::uint32_t count)
{
  const auto name = std::format("caction_arbiter{}", count);
  const auto path = std::format("{}/action_arbiter{}.dzn", mOptions.outputDir, count);
  const auto arbiter = mModel.declareComponent(name, path, {component.symbol}, true, component.symbol);

  for (std::uint32_t i = 0; i < count; ++i)
    mModel.declarePort(arbiter, std::format("client{}", i), PortDirection::Provides, PortProtocol::Action, {component.symbol});

  mModel.declarePort(arbiter, "resource", PortDirection::Requires, PortProtocol::Action, {component.symbol});

  std::ostringstream out;
  RETURN_ON_FAILURE(createActionArbiterComponent(count, out));

  mModel.setGeneratedFile(path, out.str());

  return VoidResult();
}

VoidResult LoweringPass::ensureAlarmHelper()
{
  const auto componentPath = std::format("{}/lib/alarm.dzn", mOptions.outputDir);
  std::ostringstream componentOut;
  RETURN_ON_FAILURE(createAlarmComponent(componentOut));
  mModel.setGeneratedFile(componentPath, componentOut.str());

  const auto interfacePath = std::format("{}/lib/ialarm.dzn", mOptions.outputDir);
  std::ostringstream interfaceOut;
  RETURN_ON_FAILURE(createAlarmInterface(interfaceOut));
  mModel.setGeneratedFile(interfacePath, interfaceOut.str());

  return VoidResult();
}

VoidResult LoweringPass::createNecessaryArbiter(const Component& component, std::vector<Connection>& connections)
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

    RETURN_ON_FAILURE(ensureArbiterHelper(component, static_cast<std::uint32_t>(uses.size())));

    const auto name = std::format("arbiter{}_{}", uses.size(), arbiterId++);
    mModel.declareInstance(component.symbol, name, std::format("caction_arbiter{}", uses.size()), {component.symbol});

    for (std::size_t i = 0; i < uses.size(); ++i)
      connections[uses[i]].rhs = std::format("{}.client{}", name, i);

    connections.push_back({
        .lhs = name + ".resource",
        .rhs = resource,
        .span = connections[uses.front()].span,
    });
  }

  return VoidResult();
}

void LoweringPass::countTriggers(const ir::PStrategy& strategy)
{
  if (!strategy)
    return;
  if (auto p = std::get_if<ir::Strategy::Sequence>(&strategy->value))
    for (const auto& x : p->items)
      countTriggers(x);
  else if (auto p = std::get_if<ir::Strategy::Join>(&strategy->value))
    for (const auto& x : p->items)
      countTriggers(x);
  else if (auto p = std::get_if<ir::Strategy::Either>(&strategy->value))
    for (const auto& x : p->items)
      countTriggers(x);
  else if (auto p = std::get_if<ir::Strategy::Let>(&strategy->value))
  {
    if (p->call.kind == ir::CallKind::CapabilityTrigger)
      ++mTriggerCounts[p->call.target];
  }
  else if (auto p = std::get_if<ir::Strategy::Within>(&strategy->value))
  {
    countTriggers(p->body);
    countTriggers(p->fallback);
    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
  else if (auto p = std::get_if<ir::Strategy::IfElse>(&strategy->value))
  {
    countTriggers(p->thenBranch);
    countTriggers(p->elseBranch);
  }
  else if (auto p = std::get_if<ir::Strategy::Repeat>(&strategy->value))
  {
    countTriggers(p->body);
    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
  else if (auto p = std::get_if<ir::Strategy::TaskCall>(&strategy->value))
  {
    if (p->call.kind == ir::CallKind::CapabilityTrigger)
      ++mTriggerCounts[p->call.target];

    for (const auto& h : p->handlers)
      countHandlerTriggers(h);
  }
}

void LoweringPass::countHandlerTriggers(const ir::PHandler& handler)
{
  if (!handler)
    return;

  if (handler->emitter && handler->emitter->kind == ir::CallKind::CapabilityTrigger)
    ++mTriggerCounts[handler->emitter->target];

  countTriggers(handler->body);
}

std::string LoweringPass::sourceName(koda::SymbolId id) const
{
  const auto* symbol = mSymbols.get(id);
  return symbol ? symbol->name : "unknown";
}

std::string LoweringPass::triggerName(koda::SymbolId capability) const
{
  auto it = mTriggerNames.find(capability);
  return it == mTriggerNames.end() ? "trigger" : it->second;
}

std::string LoweringPass::lower(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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
