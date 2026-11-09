#include "tracer_serializer.h"

#include <format>
#include <set>
#include <type_traits>
#include <utility>

namespace koda::tracer
{
namespace
{
// Extend this list when adding reference-engine support for a new IR strategy.
template <typename T>
constexpr const char* strategyName()
{
  if constexpr (std::is_same_v<T, ir::Strategy::Either>)
    return "either";
  else if constexpr (std::is_same_v<T, ir::Strategy::Within>)
    return "within";
  else if constexpr (std::is_same_v<T, ir::Strategy::Repeat>)
    return "repeat";
  else if constexpr (std::is_same_v<T, ir::Strategy::Retry>)
    return "retry";
  else if constexpr (std::is_same_v<T, ir::Strategy::Choose>)
    return "choose";
  else if constexpr (std::is_same_v<T, ir::Strategy::End>)
    return "success/end";
  else if constexpr (std::is_same_v<T, ir::Strategy::Failure>)
    return "failure";
  else if constexpr (std::is_same_v<T, ir::Strategy::Continue>)
    return "continue";
  else
    return "unknown";
}
}  // namespace

Serializer::Serializer(const ir::Program& program, const SymbolRegistry& symbols)
    : mSymbols(symbols)
{
  for (const auto& component : program.components)
    if (component.kind == ir::ComponentKind::Capability)
      mCapabilities.emplace(component.name, &component);
}

std::string Serializer::location(const ir::Strategy& node)
{
  return node.span.toString();
}

const ir::Argument* Serializer::capabilityInstance(const ir::Component& task, SymbolId symbol) const
{
  for (const auto& arg : task.arguments)
    if (arg.symbol == symbol && mCapabilities.contains(arg.type.toString()))
      return &arg;
  return nullptr;
}

Result<Json> Serializer::serializeChildren(const std::vector<ir::PStrategy>& children, const ir::Component& task, const std::string& key,
                                           const ir::Strategy& parent) const
{
  if (children.empty())
    return Result<Json>::Failed(std::format("tracer: empty {} at {}", key, location(parent)));

  Json::Array array;
  array.reserve(children.size());
  for (const auto& child : children)
  {
    auto serialized = serializeNode(child, task);
    if (!serialized.IsSuccess())
      return Result<Json>::Failed(serialized.ErrorMessage());
    array.emplace_back(std::move(serialized.Value()));
  }
  return Json::object({{key, Json::array(std::move(array))}});
}

Result<Json> Serializer::serializeHandlers(Json action, const std::vector<ir::PHandler>& handlers, const ir::Component& task, const ir::Strategy& parent) const
{
  // Handlers wrap the preceding strategy, in source order.  Thus A on abort(H)
  // on error(E) becomes on_error(on_abort(A,H),E), matching Dezyne lowering.
  for (const auto& handler : handlers)
  {
    if (!handler || !handler->body)
      return Result<Json>::Failed(std::format("tracer: missing handler body at {}", location(parent)));
    auto body = serializeNode(handler->body, task);
    if (!body.IsSuccess())
      return body;
    const char* kind = nullptr;
    if (handler->kind == ir::HandlerKind::Abort)
      kind = "on_abort";
    else if (handler->kind == ir::HandlerKind::Error)
      kind = "on_error";
    else
      return Result<Json>::Failed(std::format("tracer: unsupported signal/emitter handler at {}", location(parent)));
    action = Json::object({{kind, Json::object({{"action", std::move(action)}, {"handler", std::move(body.Value())}})}});
  }
  return action;
}

Result<Json> Serializer::serializeExpression(const ir::PExpression& expr) const
{
  if (!expr)
    return Result<Json>::Failed("tracer: null expression");
  return std::visit(
    [&](const auto& x) -> Result<Json> {
      using E = std::decay_t<decltype(x)>;
      if constexpr (std::is_same_v<E, ir::Expression::Literal>)
        return Json::object({{"literal", x.text}});
      else if constexpr (std::is_same_v<E, ir::Expression::Reference>)
      {
        const auto* symbol = mSymbols.get(x.symbol);
        return symbol ? Json::object({{"reference", symbol->name}}) : Result<Json>::Failed("tracer: unresolved guard symbol");
      }
      else if constexpr (std::is_same_v<E, ir::Expression::DataExpr>)
        return Json::object({{"data", x.capability + "." + x.data}});
      else if constexpr (std::is_same_v<E, ir::Expression::Unary>)
      {
        auto v = serializeExpression(x.value);
        if (!v.IsSuccess())
          return v;
        return Json::object({{"unary", Json::object({{"op", x.op}, {"value", std::move(v.Value())}})}});
      }
      else if constexpr (std::is_same_v<E, ir::Expression::Binary>)
      {
        auto l = serializeExpression(x.lhs);
        if (!l.IsSuccess())
          return l;
        auto r = serializeExpression(x.rhs);
        if (!r.IsSuccess())
          return r;
        return Json::object({{"binary", Json::object({{"op", x.op}, {"lhs", std::move(l.Value())}, {"rhs", std::move(r.Value())}})}});
      }
      else
        return Result<Json>::Failed("tracer: unsupported guard expression (only literal, reference, data, unary, binary)");
    },
    expr->value);
}

Result<Json> Serializer::serializeCall(const ir::Strategy::Call& nodeCall, const ir::Component& task, const ir::Strategy& node) const
{
  const auto& call = nodeCall.call;
  const auto withHandler = [&](Json action) -> Result<Json> { return serializeHandlers(std::move(action), nodeCall.handlers, task, node); };

  if (call.kind == ir::CallKind::Flow)
  {
    // Flow arguments are value-level data, abstracted by this control-flow tracer.
    const auto* target = mSymbols.get(call.target);
    if (!target || target->kind != SymbolKind::Flow || target->owner != task.symbol)
      return Result<Json>::Failed(std::format("tracer: invalid flow call at {}", location(node)));
    return withHandler(Json::object({{"flow_call", Json::object({{"name", target->name}})}}));
  }

  // Explicit calls (e.g. camera.process()) are CallKind::Event in current IR.
  // CapabilityTrigger also occurs in the IR for implicit trigger resolution.
  if (call.kind != ir::CallKind::CapabilityTrigger && call.kind != ir::CallKind::Event)
    return Result<Json>::Failed(std::format("tracer: UnsupportedCall(kind={}) at {}", static_cast<int>(call.kind), location(node)));

  const auto* receiver = mSymbols.get(call.receiver);
  const auto* target = mSymbols.get(call.target);
  const auto* instance = capabilityInstance(task, call.receiver);
  if (!receiver || !target || !instance || receiver->kind != SymbolKind::Argument || target->kind != SymbolKind::Event)
    return Result<Json>::Failed(std::format("tracer: invalid capability-event call at {}", location(node)));

  const auto def = mCapabilities.find(instance->type.toString());
  if (def == mCapabilities.end())
    return Result<Json>::Failed(std::format("tracer: unknown capability type '{}' at {}", instance->type.toString(), location(node)));

  const ir::Event* resolved = nullptr;
  for (const auto& action : def->second->actions)
    for (const auto& event : action.events)
      if (event.symbol == call.target)
        resolved = &event;

  if (!resolved)
    return Result<Json>::Failed(std::format("tracer: undeclared capability event '{}.{}' at {}", receiver->name, target->name, location(node)));

  if (resolved->kind == ir::EventKind::Trigger)
  {
    // Values are deliberately abstracted; the number of actual arguments is
    // retained so the tracer does not silently misrepresent call arity.
    Json::Object entry{{"instance", instance->name}, {"event", target->name}};
    if (!call.arguments.empty())
      entry.emplace("argument_count", static_cast<int>(call.arguments.size()));
    return withHandler(Json::object({{"capability_event", Json::object(std::move(entry))}}));
  }

  if (resolved->kind == ir::EventKind::Abort)
  {
    // Direct KODA 'drive.abort()' is a synchronous cancellation *request*,
    // not a new async capability trigger.  It is represented distinctly.
    return withHandler(Json::object({{"abort_call", Json::object({{"instance", instance->name}, {"event", target->name}})}}));
  }
  if (resolved->kind == ir::EventKind::In)
  {
    // Services are synchronous, and argument *values* are deliberately abstracted.
    // No data-consuming strategy nodes exist in this supported subset. Keep the
    // number of supplied arguments visible rather than silently dropping them.
    // inputSlots is a parallel annotation of arguments, not additional arguments.
    if (!call.inputSlots.empty() && call.inputSlots.size() != call.arguments.size())
      return Result<Json>::Failed(std::format("tracer: inconsistent service argument slots at {}", location(node)));
    return withHandler(Json::object(
      {{"service_call", Json::object({{"instance", instance->name}, {"event", target->name}, {"argument_count", static_cast<int>(call.arguments.size())}})}}));
  }
  return Result<Json>::Failed(std::format("tracer: unsupported capability event kind for '{}.{}' at {}", receiver->name, target->name, location(node)));
}

Result<Json> Serializer::serializeNode(const ir::PStrategy& node, const ir::Component& task) const
{
  if (!node)
    return Result<Json>::Failed("tracer: null strategy node");

  return std::visit(
    [&](const auto& value) -> Result<Json> {
      using T = std::decay_t<decltype(value)>;
      if constexpr (std::is_same_v<T, ir::Strategy::Sequence>)
        return serializeChildren(value.items, task, "sequence", *node);
      else if constexpr (std::is_same_v<T, ir::Strategy::Join>)
        return serializeChildren(value.items, task, "parallel", *node);
      else if constexpr (std::is_same_v<T, ir::Strategy::Call>)
        return serializeCall(value, task, *node);
      else if constexpr (std::is_same_v<T, ir::Strategy::Either>)
        return serializeChildren(value.items, task, "either", *node);
      else if constexpr (std::is_same_v<T, ir::Strategy::Within>)
      {
        auto body = serializeNode(value.body, task);
        if (!body.IsSuccess())
          return body;
        auto fallback = serializeNode(value.fallback, task);
        if (!fallback.IsSuccess())
          return fallback;
        return serializeHandlers(
          Json::object({{"within", Json::object({{"timeout_ms", value.seconds}, {"body", std::move(body.Value())}, {"else", std::move(fallback.Value())}})}}),
          value.handlers, task, *node);
      }
      else if constexpr (std::is_same_v<T, ir::Strategy::Repeat>)
      {
        auto body = serializeNode(value.body, task);
        if (!body.IsSuccess())
          return body;
        auto wrapped = serializeHandlers(std::move(body.Value()), value.handlers, task, *node);
        if (!wrapped.IsSuccess())
          return wrapped;
        return Json::object(
          {{"repeat", Json::object({{"interval_ms", value.seconds}, {"iterations", value.iterations}, {"body", std::move(wrapped.Value())}})}});
      }
      else if constexpr (std::is_same_v<T, ir::Strategy::Retry>)
      {
        auto body = serializeNode(value.body, task);
        if (!body.IsSuccess())
          return body;
        auto wrapped = serializeHandlers(std::move(body.Value()), value.handlers, task, *node);
        if (!wrapped.IsSuccess())
          return wrapped;
        return Json::object({{"retry", Json::object({{"attempts", value.iterations}, {"body", std::move(wrapped.Value())}})}});
      }
      else if constexpr (std::is_same_v<T, ir::Strategy::Choose>)
      {
        Json::Array branches;
        for (const auto& option : value.options)
        {
          auto body = serializeNode(option.strategy, task);
          if (!body.IsSuccess())
            return body;
          if (option.condition)
          {
            auto guard = serializeExpression(option.condition);
            if (!guard.IsSuccess())
              return guard;
            branches.emplace_back(Json::object({{"when", std::move(guard.Value())}, {"body", std::move(body.Value())}}));
          }
          else
            branches.emplace_back(Json::object({{"default", std::move(body.Value())}}));
        }
        return Json::object({{"choose", Json::array(std::move(branches))}});
      }
      else if constexpr (std::is_same_v<T, ir::Strategy::End>)
        return Json::object({{"success", Json::object({})}});
      else if constexpr (std::is_same_v<T, ir::Strategy::Failure>)
        return Json::object({{"failure", Json::object({})}});
      else if constexpr (std::is_same_v<T, ir::Strategy::Continue>)
        return Json::object({{"continue", Json::object({})}});
      else
        return Result<Json>::Failed(std::format("tracer: UnsupportedNode({}) at {}", strategyName<T>(), location(*node)));
    },
    node->value);
}

namespace
{
const char* actionKindName(ir::CapabilityKind kind)
{
  switch (kind)
  {
    case ir::CapabilityKind::Async:
      return "async_action";
    case ir::CapabilityKind::Sync:
      return "sync_action";
    case ir::CapabilityKind::Unknown:
      break;
  }
  return "unknown";
}

Json resultChoice(const char* reply, const char* event = nullptr)
{
  Json::Object result{{"reply", reply}};
  if (event)
    result.emplace("event", event);
  return Json::object(std::move(result));
}

Json defaultActionMock(const std::string& kind)
{
  // These are possible outcomes, NOT probabilities or an expected execution.
  // An async trigger can be immediately rejected, or accepted then succeed/fail.
  // A sync trigger can finish or fail within the calling macrostep.
  Json::Array outcomes;
  if (kind == "async_action")
  {
    outcomes.emplace_back(resultChoice("ACCEPTED", "SUCCESS"));
    outcomes.emplace_back(resultChoice("ACCEPTED", "FAILURE"));
    outcomes.emplace_back(resultChoice("FAILURE"));
  }
  else if (kind == "sync_action")
  {
    outcomes.emplace_back(resultChoice("DONE"));
    outcomes.emplace_back(resultChoice("FAILURE"));
  }
  else if (kind == "sync_service")
  {
    // Synchronous service calls return within the invoking macrostep:
    // successful completion (DONE) or immediate FAILURE. Never ACCEPTED.
    outcomes.emplace_back(resultChoice("DONE"));
    outcomes.emplace_back(resultChoice("FAILURE"));
  }
  Json::Object mock{{"outcomes", Json::array(std::move(outcomes))}};
  if (kind != "sync_service")
    mock.emplace("reset", Json::array(Json::Array{"SUCCESS", "FAILURE"}));
  if (kind == "async_action")
    mock.emplace("abort", Json::array(Json::Array{"SUCCESS", "FAILURE"}));
  // Sync operations have no Running state to abort, so omit abort outcomes.
  return Json::object(std::move(mock));
}
}  // namespace

Result<std::map<std::string, std::string>> Serializer::callableActions(const ir::Component& capability) const
{
  std::map<std::string, std::string> calls;
  for (const auto& action : capability.actions)
    for (const auto& event : action.events)
    {
      if (event.kind != ir::EventKind::Trigger && event.kind != ir::EventKind::In)
        continue;
      if (event.name.empty())
        return Result<std::map<std::string, std::string>>::Failed(std::format("tracer: empty trigger event in capability '{}'", capability.name));
      if (event.kind == ir::EventKind::Trigger && capability.capabilityKind == ir::CapabilityKind::Unknown)
        return Result<std::map<std::string, std::string>>::Failed(std::format("tracer: capability '{}' has unknown sync/async kind", capability.name));
      const std::string kind = event.kind == ir::EventKind::In ? "sync_service" : actionKindName(capability.capabilityKind);
      if (!calls.emplace(event.name, kind).second)
        return Result<std::map<std::string, std::string>>::Failed(
          std::format("tracer: duplicate callable event '{}.{}' (cannot key mocks by overload)", capability.name, event.name));
    }
  return calls;
}

Result<Json> Serializer::serializeCapabilities(const ir::Component& task) const
{
  Json::Object instances;
  for (const auto& argument : task.arguments)
  {
    const auto def = mCapabilities.find(argument.type.toString());
    if (def == mCapabilities.end())
      continue;
    if (argument.symbol == InvalidSymbol || argument.name.empty())
      return Result<Json>::Failed(std::format("tracer: invalid capability argument in task '{}'", task.name));
    auto actions = callableActions(*def->second);
    if (!actions.IsSuccess())
      return Result<Json>::Failed(actions.ErrorMessage());

    Json::Array events;
    Json::Object kinds;
    for (const auto& [name, kind] : actions.Value())
    {
      events.emplace_back(name);
      kinds.emplace(name, kind);
    }
    // A capability's explicit abort requests are NOT async action triggers.
    // Preserve their names separately to validate direct KODA abort calls.
    std::set<std::string> abortNames;
    for (const auto& action : def->second->actions)
      for (const auto& event : action.events)
        if (event.kind == ir::EventKind::Abort)
          abortNames.insert(event.name);
    Json::Array abortEvents;
    for (const auto& name : abortNames)
      abortEvents.emplace_back(name);
    instances.emplace(argument.name, Json::object({{"type", def->second->name},
                                                   {"events", Json::array(std::move(events))},
                                                   {"abort_events", Json::array(std::move(abortEvents))},
                                                   {"event_kinds", Json::object(std::move(kinds))}}));
  }
  return Json::object(std::move(instances));
}

Result<Json> Serializer::serializeDefaultMocks(const ir::Component& task) const
{
  if (task.kind != ir::ComponentKind::Task)
    return Result<Json>::Failed(std::format("tracer: '{}' is not a task", task.name));
  Json::Object instances;
  for (const auto& argument : task.arguments)
  {
    const auto def = mCapabilities.find(argument.type.toString());
    if (def == mCapabilities.end())
      continue;
    auto actions = callableActions(*def->second);
    if (!actions.IsSuccess())
      return Result<Json>::Failed(actions.ErrorMessage());

    Json::Object mockEvents;
    for (const auto& [name, kind] : actions.Value())
      mockEvents.emplace(name, defaultActionMock(kind));
    instances.emplace(argument.name, Json::object(std::move(mockEvents)));
  }
  return Json::object({{"schema", "koda.reference.mocks.v1"}, {"mocks", Json::object(std::move(instances))}});
}

Result<Json> Serializer::serializeTask(const ir::Component& task) const
{
  if (task.kind != ir::ComponentKind::Task)
    return Result<Json>::Failed(std::format("tracer: '{}' is not a task", task.name));

  Json::Object flows;
  bool foundMain = false;
  for (const auto& flow : task.flows)
  {
    if (!flow.arguments.empty())
      return Result<Json>::Failed(std::format("tracer: UnsupportedNode(flow parameters) in '{}.{}'", task.name, flow.name));
    if (flow.name.empty())
      return Result<Json>::Failed(std::format("tracer: unnamed flow in task '{}'", task.name));
    if (flow.name == "main")
      foundMain = true;

    auto result = serializeNode(flow.strategy, task);
    if (!result.IsSuccess())
      return Result<Json>::Failed(std::format("tracer: task '{}', flow '{}': {}", task.name, flow.name, result.ErrorMessage()));
    if (!flows.emplace(flow.name, std::move(result.Value())).second)
      return Result<Json>::Failed(std::format("tracer: duplicate flow '{}.{}'", task.name, flow.name));
  }
  if (!foundMain)
    return Result<Json>::Failed(std::format("tracer: task '{}' has no main flow", task.name));

  auto caps = serializeCapabilities(task);
  if (!caps.IsSuccess())
    return Result<Json>::Failed(caps.ErrorMessage());

  return Json::object({{"schema", "koda.reference.model.v1"},
                       {"program", Json::object({{"flow_call", Json::object({{"name", "main"}})}})},
                       {"flows", Json::object(std::move(flows))},
                       {"capabilities", std::move(caps.Value())}});
}
}  // namespace koda::tracer
