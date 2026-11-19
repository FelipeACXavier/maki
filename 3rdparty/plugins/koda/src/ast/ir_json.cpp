#include "ir_json.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSaveFile>
#include <QtGlobal>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <variant>

#include "type_helpers.h"

namespace koda
{
namespace
{
using Object = QJsonObject;
using Array = QJsonArray;
using Value = QJsonValue;

constexpr int MaxDepth = 512;

[[noreturn]] void malformed(const QString& message)
{
  throw std::runtime_error(message.toStdString());
}

QString str(const std::string& s)
{
  return QString::fromStdString(s);
}

std::string native(const QString& s)
{
  return s.toStdString();
}

Value symbolToJson(SymbolId id)
{
  if (id == InvalidSymbol)
    return Value(QJsonValue::Null);

  return Value(static_cast<qint64>(id));
}

Value optionalStringToJson(const std::optional<std::string>& text)
{
  return text ? Value(str(*text)) : Value(QJsonValue::Null);
}

Object spanToJson(const Span& span)
{
  return {{"lineStart", span.lineStart}, {"colStart", span.colStart}, {"lineEnd", span.lineEnd}, {"colEnd", span.colEnd}};
}

template <typename Enum>
int enumToJson(Enum e)
{
  return static_cast<int>(e);
}

template <typename T, typename F>
Array mapArray(const std::vector<T>& items, F&& convert)
{
  Array result;
  for (const auto& item : items)
    result.append(convert(item));
  return result;
}

Array stringsToJson(const std::vector<std::string>& items)
{
  return mapArray(items, [](const auto& s) -> Value { return str(s); });
}

// The pointer graph is expected to be an owned tree/DAG, not cyclic.
// Nodes are written structurally; object addresses never enter JSON.
class Writer
{
public:
  Object program(const ir::Program& p)
  {
    return {{"components", mapArray(p.components, [this](const auto& c) { return component(c); })}};
  }

  Object symbols(const SymbolRegistry& registry)
  {
    Array all;
    // SymbolRegistry IDs are contiguous indices into mSymbols. Iterating get()
    // avoids adding a new public accessor or serializing its derived mScopes.
    for (SymbolId id = 0; id != InvalidSymbol; ++id)
    {
      const Symbol* s = registry.get(id);
      if (!s)
        break;
      all.append(Object{{"id", symbolToJson(s->id)},
                        {"kind", enumToJson(s->kind)},
                        {"name", str(s->name)},
                        {"type", maki::typeReferenceToJson(s->type)},
                        {"span", spanToJson(s->span)},
                        {"owner", symbolToJson(s->owner)},
                        {"actionId", symbolToJson(s->actionId)}});
    }
    return {{"entries", all}};
  }

private:
  // Writer's stack guard rejects actual pointer cycles. Shared subtrees that
  // appear in different branches are serialized twice (identity not retained).
  struct Guard
  {
    Writer& w;
    const void* address;

    Guard(Writer& writer, const void* pointer)
        : w(writer)
        , address(pointer)
    {
      if (++w.depth > MaxDepth)
        malformed("IR exceeds maximum JSON nesting depth");
      if (pointer && !w.active.insert(pointer).second)
        malformed("Cycle detected in IR shared_ptr graph");
    }

    ~Guard()
    {
      if (address)
        w.active.erase(address);
      --w.depth;
    }
  };

  int depth = 0;
  std::unordered_set<const void*> active;

  template <typename T, typename F>
  Value ptr(const std::shared_ptr<T>& p, F&& convert)
  {
    if (!p)
      return Value(QJsonValue::Null);
    Guard guard(*this, p.get());
    return convert(*p);
  }

  Value expressionPtr(const ir::PExpression& p)
  {
    return ptr(p, [this](const auto& x) -> Value { return expression(x); });
  }

  Value strategyPtr(const ir::PStrategy& p)
  {
    return ptr(p, [this](const auto& x) -> Value { return strategy(x); });
  }

  Value handlerPtr(const ir::PHandler& p)
  {
    return ptr(p, [this](const auto& x) -> Value { return handler(x); });
  }

  Value propertyPtr(const ir::PPropertyExpr& p)
  {
    return ptr(p, [this](const auto& x) -> Value { return propertyExpr(x); });
  }

  Object argument(const ir::Argument& a)
  {
    return {{"symbol", symbolToJson(a.symbol)},
            {"name", str(a.name)},
            {"type", maki::typeReferenceToJson(a.type)},
            {"mode", enumToJson(a.mode)},
            {"span", spanToJson(a.span)}};
  }

  Object call(const ir::Call& c)
  {
    return {{"kind", enumToJson(c.kind)},
            {"receiver", symbolToJson(c.receiver)},
            {"target", symbolToJson(c.target)},
            {"arguments", mapArray(c.arguments, [this](const auto& a) { return expressionPtr(a); })},
            {"inputSlots", mapArray(c.inputSlots, [](const auto& slot) { return optionalStringToJson(slot); })},
            {"outputSlots", stringsToJson(c.outputSlots)},
            {"span", spanToJson(c.span)}};
  }

  Object expression(const ir::Expression& e)
  {
    Object o{{"type", maki::typeReferenceToJson(e.type)}, {"span", spanToJson(e.span)}};
    std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, ir::Expression::Literal>)
        {
          o["node"] = "literal";
          o["text"] = str(value.text);
          o["literalType"] = maki::typeReferenceToJson(value.type);
        }
        else if constexpr (std::is_same_v<T, ir::Expression::Reference>)
        {
          o["node"] = "reference";
          o["symbol"] = symbolToJson(value.symbol);
        }
        else if constexpr (std::is_same_v<T, ir::Expression::CallExpr>)
        {
          o["node"] = "call";
          o["call"] = call(value.call);
        }
        else if constexpr (std::is_same_v<T, ir::Expression::Unary>)
        {
          o["node"] = "unary";
          o["op"] = str(value.op);
          o["value"] = expressionPtr(value.value);
        }
        else if constexpr (std::is_same_v<T, ir::Expression::Binary>)
        {
          o["node"] = "binary";
          o["op"] = str(value.op);
          o["lhs"] = expressionPtr(value.lhs);
          o["rhs"] = expressionPtr(value.rhs);
        }
        else if constexpr (std::is_same_v<T, ir::Expression::RecordLiteral>)
        {
          o["node"] = "record";
          o["fields"] = mapArray(value.fields, [this](const auto& f) { return Object{{"name", str(f.name)}, {"value", expressionPtr(f.value)}}; });
        }
        else if constexpr (std::is_same_v<T, ir::Expression::ListLiteral>)
        {
          o["node"] = "list";
          o["fields"] = mapArray(value.fields, [this](const auto& f) { return expressionPtr(f); });
        }
        else if constexpr (std::is_same_v<T, ir::Expression::MapLiteral>)
        {
          o["node"] = "map";
          o["fields"] = mapArray(value.fields, [this](const auto& f) { return Object{{"key", expressionPtr(f.key)}, {"value", expressionPtr(f.value)}}; });
        }
        else if constexpr (std::is_same_v<T, ir::Expression::DataExpr>)
        {
          o["node"] = "data";
          o["capability"] = str(value.capability);
          o["data"] = str(value.data);
        }
      },
      e.value);
    return o;
  }

  Object handler(const ir::Handler& h)
  {
    return {{"kind", enumToJson(h.kind)},
            {"emitter", h.emitter ? Value(call(*h.emitter)) : Value(QJsonValue::Null)},
            {"body", strategyPtr(h.body)},
            {"span", spanToJson(h.span)}};
  }

  Object strategy(const ir::Strategy& s)
  {
    Object o{{"id", str(s.id)}, {"name", str(s.name)}, {"owner", symbolToJson(s.owner)}, {"span", spanToJson(s.span)}};
    std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, ir::Strategy::Sequence> || std::is_same_v<T, ir::Strategy::Join> || std::is_same_v<T, ir::Strategy::Either>)
        {
          if constexpr (std::is_same_v<T, ir::Strategy::Sequence>)
            o["node"] = "sequence";
          if constexpr (std::is_same_v<T, ir::Strategy::Join>)
            o["node"] = "join";
          if constexpr (std::is_same_v<T, ir::Strategy::Either>)
            o["node"] = "either";
          o["items"] = mapArray(value.items, [this](const auto& p) { return strategyPtr(p); });
        }
        else if constexpr (std::is_same_v<T, ir::Strategy::Within>)
        {
          o["node"] = "within";
          o["seconds"] = value.seconds;
          o["body"] = strategyPtr(value.body);
          o["fallback"] = strategyPtr(value.fallback);
          o["handlers"] = mapArray(value.handlers, [this](const auto& p) { return handlerPtr(p); });
        }
        else if constexpr (std::is_same_v<T, ir::Strategy::Repeat>)
        {
          o["node"] = "repeat";
          o["seconds"] = value.seconds;
          o["iterations"] = value.iterations;
          o["body"] = strategyPtr(value.body);
          o["handlers"] = mapArray(value.handlers, [this](const auto& p) { return handlerPtr(p); });
        }
        else if constexpr (std::is_same_v<T, ir::Strategy::Retry>)
        {
          o["node"] = "retry";
          o["iterations"] = value.iterations;
          o["body"] = strategyPtr(value.body);
          o["handlers"] = mapArray(value.handlers, [this](const auto& p) { return handlerPtr(p); });
        }
        else if constexpr (std::is_same_v<T, ir::Strategy::End>)
          o["node"] = "end";
        else if constexpr (std::is_same_v<T, ir::Strategy::Failure>)
          o["node"] = "failure";
        else if constexpr (std::is_same_v<T, ir::Strategy::Continue>)
          o["node"] = "continue";
        else if constexpr (std::is_same_v<T, ir::Strategy::Call>)
        {
          o["node"] = "call";
          o["call"] = call(value.call);
          o["handlers"] = mapArray(value.handlers, [this](const auto& p) { return handlerPtr(p); });
        }
        else if constexpr (std::is_same_v<T, ir::Strategy::Choose>)
        {
          o["node"] = "choose";
          o["options"] = mapArray(value.options,
                                  [this](const auto& p) { return Object{{"condition", expressionPtr(p.condition)}, {"strategy", strategyPtr(p.strategy)}}; });
        }
      },
      s.value);
    return o;
  }

  Object variable(const ir::Variable& v)
  {
    return {{"symbol", symbolToJson(v.symbol)},    {"name", str(v.name)},        {"type", maki::typeReferenceToJson(v.type)},
            {"initial", expressionPtr(v.initial)}, {"span", spanToJson(v.span)}, {"slot", optionalStringToJson(v.slot)}};
  }

  Object event(const ir::Event& e)
  {
    return {{"symbol", symbolToJson(e.symbol)},
            {"kind", enumToJson(e.kind)},
            {"name", str(e.name)},
            {"type", maki::typeReferenceToJson(e.type)},
            {"arguments", mapArray(e.arguments, [this](const auto& a) { return argument(a); })},
            {"span", spanToJson(e.span)}};
  }

  Object flow(const ir::Flow& f)
  {
    return {{"symbol", symbolToJson(f.symbol)},
            {"name", str(f.name)},
            {"arguments", mapArray(f.arguments, [this](const auto& a) { return argument(a); })},
            {"strategy", strategyPtr(f.strategy)},
            {"span", spanToJson(f.span)}};
  }

  Object action(const ir::Action& a)
  {
    return {{"events", mapArray(a.events, [this](const auto& e) { return event(e); })}};
  }

  Object observation(const ir::Observation& o)
  {
    return {{"kind", enumToJson(o.kind)}, {"receiver", symbolToJson(o.receiver)}, {"target", symbolToJson(o.target)}, {"span", spanToJson(o.span)}};
  }

  Object propertyExpr(const ir::PropertyExpr& e)
  {
    Object o{{"span", spanToJson(e.span)}};
    std::visit(
      [&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, ir::Observation>)
        {
          o["node"] = "observation";
          o["observation"] = observation(value);
        }
        else if constexpr (std::is_same_v<T, ir::PropertyExpr::Not> || std::is_same_v<T, ir::PropertyExpr::Always> ||
                           std::is_same_v<T, ir::PropertyExpr::Eventually> || std::is_same_v<T, ir::PropertyExpr::Next>)
        {
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Not>)
            o["node"] = "not";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Always>)
            o["node"] = "always";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Eventually>)
            o["node"] = "eventually";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Next>)
            o["node"] = "next";
          o["operand"] = propertyPtr(value.operand);
        }
        else
        {
          if constexpr (std::is_same_v<T, ir::PropertyExpr::And>)
            o["node"] = "and";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Or>)
            o["node"] = "or";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Implies>)
            o["node"] = "implies";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::Until>)
            o["node"] = "until";
          if constexpr (std::is_same_v<T, ir::PropertyExpr::WeakUntil>)
            o["node"] = "weakUntil";
          o["lhs"] = propertyPtr(value.lhs);
          o["rhs"] = propertyPtr(value.rhs);
        }
      },
      e.value);
    return o;
  }

  Object property(const ir::Property& p)
  {
    return {{"name", str(p.name)}, {"expression", propertyPtr(p.expression)}, {"span", spanToJson(p.span)}};
  }

  Object component(const ir::Component& c)
  {
    Object metadata;
    for (const auto& [key, value] : c.metadata)
      metadata.insert(str(key), str(value));
    return {{"symbol", symbolToJson(c.symbol)},
            {"kind", enumToJson(c.kind)},
            {"capabilityKind", enumToJson(c.capabilityKind)},
            {"name", str(c.name)},
            {"arguments", mapArray(c.arguments, [this](const auto& a) { return argument(a); })},
            {"variables", mapArray(c.variables, [this](const auto& v) { return variable(v); })},
            {"actions", mapArray(c.actions, [this](const auto& a) { return action(a); })},
            {"flows", mapArray(c.flows, [this](const auto& f) { return flow(f); })},
            {"properties", mapArray(c.properties, [this](const auto& p) { return property(p); })},
            {"metadata", metadata},
            {"span", spanToJson(c.span)}};
  }
};

Value required(const Object& o, const char* key)
{
  const QString name = QString::fromLatin1(key);
  if (!o.contains(name))
    malformed("Missing required JSON field: " + name);
  return o.value(name);
}

Object asObject(const Value& v, const QString& field)
{
  if (!v.isObject())
    malformed("Expected object at " + field);
  return v.toObject();
}

Array asArray(const Value& v, const QString& field)
{
  if (!v.isArray())
    malformed("Expected array at " + field);
  return v.toArray();
}

std::string asString(const Value& v, const QString& field)
{
  if (!v.isString())
    malformed("Expected string at " + field);
  return native(v.toString());
}

int asInt(const Value& v, const QString& field)
{
  if (!v.isDouble())
    malformed("Expected integer at " + field);
  const double n = v.toDouble();
  if (!std::isfinite(n) || std::floor(n) != n || n < std::numeric_limits<int>::min() || n > std::numeric_limits<int>::max())
    malformed("Integer out of range at " + field);
  return static_cast<int>(n);
}

Object fieldObject(const Object& o, const char* name)
{
  return asObject(required(o, name), QString::fromLatin1(name));
}

Array fieldArray(const Object& o, const char* name)
{
  return asArray(required(o, name), QString::fromLatin1(name));
}

std::string fieldString(const Object& o, const char* name)
{
  return asString(required(o, name), QString::fromLatin1(name));
}

int fieldInt(const Object& o, const char* name)
{
  return asInt(required(o, name), QString::fromLatin1(name));
}

SymbolId readSymbol(const Value& v)
{
  if (v.isNull())
    return InvalidSymbol;
  if (!v.isDouble())
    malformed("SymbolId must be an integer or null");
  const double n = v.toDouble();
  if (!std::isfinite(n) || std::floor(n) != n || n < 0.0 || n >= static_cast<double>(InvalidSymbol))
    malformed("Invalid SymbolId");
  return static_cast<SymbolId>(n);
}

std::optional<std::string> readOptionalString(const Value& v)
{
  if (v.isNull())
    return std::nullopt;
  return asString(v, "optional string");
}

Span readSpan(const Object& o)
{
  return {fieldInt(o, "lineStart"), fieldInt(o, "colStart"), fieldInt(o, "lineEnd"), fieldInt(o, "colEnd")};
}

// Enums in this format have stable underlying integer codes. New enum cases
// should increment FormatVersion and update the corresponding permitted range.
template <typename E>
E readEnum(const Value& v, int first, int last)
{
  const int n = asInt(v, "enum");
  if (n < first || n > last)
    malformed("Unrecognized IR enum value");
  return static_cast<E>(n);
}

// Delegate type wire formats to MAKI's existing type_helpers API.
types::TypeReference readType(const Object& o)
{
  auto value = maki::typeReferenceFromJson(o);
  if (!value.IsSuccess())
    malformed("Invalid TypeReference: " + str(value.ErrorMessage()));
  return value.Value();
}

types::TypeDefinition readDefinition(const Object& o)
{
  auto value = maki::typeDefinitionFromJson(o);
  if (!value.IsSuccess())
    malformed("Invalid TypeDefinition: " + str(value.ErrorMessage()));
  return value.Value();
}

template <typename T, typename F>
std::vector<T> readArray(const Array& a, F&& decode)
{
  std::vector<T> result;
  result.reserve(static_cast<std::size_t>(a.size()));
  for (const auto& v : a)
    result.push_back(decode(v));
  return result;
}

class Reader
{
public:
  ir::Program program(const Object& o)
  {
    return {readArray<ir::Component>(fieldArray(o, "components"), [this](const Value& v) { return component(asObject(v, "component")); })};
  }

  SymbolRegistry symbols(const Object& o)
  {
    SymbolRegistry registry;
    for (const auto& item : fieldArray(o, "entries"))
    {
      const Object s = asObject(item, "symbol");
      const SymbolId expected = readSymbol(required(s, "id"));
      if (expected == InvalidSymbol)
        malformed("Symbol cannot have a null id");
      const SymbolKind kind = readEnum<SymbolKind>(required(s, "kind"), 0, 8);
      const std::string name = fieldString(s, "name");
      const auto type = readType(fieldObject(s, "type"));
      const auto span = readSpan(fieldObject(s, "span"));
      const SymbolId owner = readSymbol(required(s, "owner"));
      const SymbolId actionId = readSymbol(required(s, "actionId"));
      // Rebuild in original declaration order. declare() recreates all scope
      // lookup tables and guarantees that IDs match their original indices.
      if (owner != InvalidSymbol && !registry.get(owner))
        malformed("Symbol owner must be declared before child: " + str(name));
      auto declared = registry.declare(kind, name, type, span, owner, actionId);
      if (!declared.IsSuccess())
        malformed("SymbolRegistry: " + str(declared.ErrorMessage()));
      if (declared.Value() != expected)
        malformed("Symbol ID/order mismatch at " + str(name));
      // declare() may normalize unresolved named types. Preserve the precise
      // serialized type descriptor after it has rebuilt the scope indexes.
      registry.get(expected)->type = type;
    }
    return registry;
  }

private:
  int depth = 0;

  struct Guard
  {
    Reader& reader;

    explicit Guard(Reader& r)
        : reader(r)
    {
      if (++reader.depth > MaxDepth)
        malformed("JSON IR exceeds maximum nesting depth");
    }

    ~Guard()
    {
      --reader.depth;
    }
  };

  ir::PExpression expressionPtr(const Value& v)
  {
    if (v.isNull())
      return {};
    Guard guard(*this);
    return std::make_shared<ir::Expression>(expression(asObject(v, "expression")));
  }

  ir::PStrategy strategyPtr(const Value& v)
  {
    if (v.isNull())
      return {};
    Guard guard(*this);
    return std::make_shared<ir::Strategy>(strategy(asObject(v, "strategy")));
  }

  ir::PHandler handlerPtr(const Value& v)
  {
    if (v.isNull())
      return {};
    Guard guard(*this);
    return std::make_shared<ir::Handler>(handler(asObject(v, "handler")));
  }

  ir::PPropertyExpr propertyPtr(const Value& v)
  {
    if (v.isNull())
      return {};
    Guard guard(*this);
    return std::make_shared<ir::PropertyExpr>(propertyExpr(asObject(v, "property expression")));
  }

  std::vector<ir::PExpression> expressionPtrs(const Array& a)
  {
    return readArray<ir::PExpression>(a, [this](const Value& v) { return expressionPtr(v); });
  }

  std::vector<ir::PStrategy> strategyPtrs(const Array& a)
  {
    return readArray<ir::PStrategy>(a, [this](const Value& v) { return strategyPtr(v); });
  }

  std::vector<ir::PHandler> handlerPtrs(const Array& a)
  {
    return readArray<ir::PHandler>(a, [this](const Value& v) { return handlerPtr(v); });
  }

  std::vector<ir::Argument> arguments(const Array& a)
  {
    return readArray<ir::Argument>(a, [this](const Value& v) { return argument(asObject(v, "argument")); });
  }

  ir::Argument argument(const Object& o)
  {
    ir::Argument a;
    a.symbol = readSymbol(required(o, "symbol"));
    a.name = fieldString(o, "name");
    a.type = readType(fieldObject(o, "type"));
    a.mode = readEnum<ir::ArgumentMode>(required(o, "mode"), 0, 2);
    a.span = readSpan(fieldObject(o, "span"));
    return a;
  }

  ir::Call call(const Object& o)
  {
    ir::Call c;
    c.kind = readEnum<ir::CallKind>(required(o, "kind"), 0, 3);
    c.receiver = readSymbol(required(o, "receiver"));
    c.target = readSymbol(required(o, "target"));
    c.arguments = expressionPtrs(fieldArray(o, "arguments"));
    c.inputSlots = readArray<std::optional<std::string>>(fieldArray(o, "inputSlots"), [](const Value& v) { return readOptionalString(v); });
    c.outputSlots = readArray<std::string>(fieldArray(o, "outputSlots"), [](const Value& v) { return asString(v, "output slot"); });
    c.span = readSpan(fieldObject(o, "span"));
    return c;
  }

  ir::Expression expression(const Object& o)
  {
    ir::Expression e;
    e.type = readType(fieldObject(o, "type"));
    e.span = readSpan(fieldObject(o, "span"));
    const std::string node = fieldString(o, "node");
    if (node == "literal")
      e.value = ir::Expression::Literal{fieldString(o, "text"), readType(fieldObject(o, "literalType"))};
    else if (node == "reference")
      e.value = ir::Expression::Reference{readSymbol(required(o, "symbol"))};
    else if (node == "call")
      e.value = ir::Expression::CallExpr{call(fieldObject(o, "call"))};
    else if (node == "unary")
      e.value = ir::Expression::Unary{fieldString(o, "op"), expressionPtr(required(o, "value"))};
    else if (node == "binary")
      e.value = ir::Expression::Binary{fieldString(o, "op"), expressionPtr(required(o, "lhs")), expressionPtr(required(o, "rhs"))};
    else if (node == "record")
    {
      ir::Expression::RecordLiteral r;
      r.fields = readArray<ir::Expression::RecordLiteral::Field>(fieldArray(o, "fields"), [this](const Value& v) {
        const auto f = asObject(v, "record field");
        return ir::Expression::RecordLiteral::Field{fieldString(f, "name"), expressionPtr(required(f, "value"))};
      });
      e.value = std::move(r);
    }
    else if (node == "list")
      e.value = ir::Expression::ListLiteral{expressionPtrs(fieldArray(o, "fields"))};
    else if (node == "map")
    {
      ir::Expression::MapLiteral m;
      m.fields = readArray<ir::Expression::MapLiteral::Field>(fieldArray(o, "fields"), [this](const Value& v) {
        const auto f = asObject(v, "map field");
        return ir::Expression::MapLiteral::Field{expressionPtr(required(f, "key")), expressionPtr(required(f, "value"))};
      });
      e.value = std::move(m);
    }
    else if (node == "data")
      e.value = ir::Expression::DataExpr{fieldString(o, "capability"), fieldString(o, "data")};
    else
      malformed("Unknown Expression node: " + str(node));
    return e;
  }

  ir::Handler handler(const Object& o)
  {
    ir::Handler h;
    h.kind = readEnum<ir::HandlerKind>(required(o, "kind"), 0, 3);
    const auto emitter = required(o, "emitter");
    if (!emitter.isNull())
      h.emitter = call(asObject(emitter, "handler emitter"));
    h.body = strategyPtr(required(o, "body"));
    h.span = readSpan(fieldObject(o, "span"));
    return h;
  }

  ir::Strategy strategy(const Object& o)
  {
    ir::Strategy s;
    s.id = fieldString(o, "id");
    s.name = fieldString(o, "name");
    s.owner = readSymbol(required(o, "owner"));
    s.span = readSpan(fieldObject(o, "span"));
    const std::string node = fieldString(o, "node");
    if (node == "sequence")
      s.value = ir::Strategy::Sequence{strategyPtrs(fieldArray(o, "items"))};
    else if (node == "join")
      s.value = ir::Strategy::Join{strategyPtrs(fieldArray(o, "items"))};
    else if (node == "either")
      s.value = ir::Strategy::Either{strategyPtrs(fieldArray(o, "items"))};
    else if (node == "within")
      s.value = ir::Strategy::Within{fieldInt(o, "seconds"), strategyPtr(required(o, "body")), strategyPtr(required(o, "fallback")),
                                     handlerPtrs(fieldArray(o, "handlers"))};
    else if (node == "repeat")
      s.value = ir::Strategy::Repeat{fieldInt(o, "seconds"), fieldInt(o, "iterations"), strategyPtr(required(o, "body")),
                                     handlerPtrs(fieldArray(o, "handlers"))};
    else if (node == "retry")
      s.value = ir::Strategy::Retry{fieldInt(o, "iterations"), strategyPtr(required(o, "body")), handlerPtrs(fieldArray(o, "handlers"))};
    else if (node == "end")
      s.value = ir::Strategy::End{};
    else if (node == "failure")
      s.value = ir::Strategy::Failure{};
    else if (node == "continue")
      s.value = ir::Strategy::Continue{};
    else if (node == "call")
      s.value = ir::Strategy::Call{call(fieldObject(o, "call")), handlerPtrs(fieldArray(o, "handlers"))};
    else if (node == "choose")
    {
      ir::Strategy::Choose c;
      c.options = readArray<ir::Strategy::Choose::When>(fieldArray(o, "options"), [this](const Value& v) {
        const Object w = asObject(v, "when option");
        return ir::Strategy::Choose::When{expressionPtr(required(w, "condition")), strategyPtr(required(w, "strategy"))};
      });
      s.value = std::move(c);
    }
    else
      malformed("Unknown Strategy node: " + str(node));
    return s;
  }

  ir::Variable variable(const Object& o)
  {
    ir::Variable v;
    v.symbol = readSymbol(required(o, "symbol"));
    v.name = fieldString(o, "name");
    v.type = readType(fieldObject(o, "type"));
    v.initial = expressionPtr(required(o, "initial"));
    v.span = readSpan(fieldObject(o, "span"));
    v.slot = readOptionalString(required(o, "slot"));
    return v;
  }

  ir::Event event(const Object& o)
  {
    ir::Event e;
    e.symbol = readSymbol(required(o, "symbol"));
    e.kind = readEnum<ir::EventKind>(required(o, "kind"), 0, 5);
    e.name = fieldString(o, "name");
    e.type = readType(fieldObject(o, "type"));
    e.arguments = arguments(fieldArray(o, "arguments"));
    e.span = readSpan(fieldObject(o, "span"));
    return e;
  }

  ir::Flow flow(const Object& o)
  {
    ir::Flow f;
    f.symbol = readSymbol(required(o, "symbol"));
    f.name = fieldString(o, "name");
    f.arguments = arguments(fieldArray(o, "arguments"));
    f.strategy = strategyPtr(required(o, "strategy"));
    f.span = readSpan(fieldObject(o, "span"));
    return f;
  }

  ir::Action action(const Object& o)
  {
    ir::Action a;
    a.events = readArray<ir::Event>(fieldArray(o, "events"), [this](const Value& v) { return event(asObject(v, "event")); });
    return a;
  }

  ir::Observation observation(const Object& o)
  {
    ir::Observation v;
    v.kind = readEnum<ir::Observation::Kind>(required(o, "kind"), 0, 6);
    v.receiver = readSymbol(required(o, "receiver"));
    v.target = readSymbol(required(o, "target"));
    v.span = readSpan(fieldObject(o, "span"));
    return v;
  }

  ir::PropertyExpr propertyExpr(const Object& o)
  {
    ir::PropertyExpr e;
    e.span = readSpan(fieldObject(o, "span"));
    const auto node = fieldString(o, "node");
    if (node == "observation")
      e.value = observation(fieldObject(o, "observation"));
    else if (node == "not")
      e.value = ir::PropertyExpr::Not{propertyPtr(required(o, "operand"))};
    else if (node == "always")
      e.value = ir::PropertyExpr::Always{propertyPtr(required(o, "operand"))};
    else if (node == "eventually")
      e.value = ir::PropertyExpr::Eventually{propertyPtr(required(o, "operand"))};
    else if (node == "next")
      e.value = ir::PropertyExpr::Next{propertyPtr(required(o, "operand"))};
    else if (node == "and")
      e.value = ir::PropertyExpr::And{propertyPtr(required(o, "lhs")), propertyPtr(required(o, "rhs"))};
    else if (node == "or")
      e.value = ir::PropertyExpr::Or{propertyPtr(required(o, "lhs")), propertyPtr(required(o, "rhs"))};
    else if (node == "implies")
      e.value = ir::PropertyExpr::Implies{propertyPtr(required(o, "lhs")), propertyPtr(required(o, "rhs"))};
    else if (node == "until")
      e.value = ir::PropertyExpr::Until{propertyPtr(required(o, "lhs")), propertyPtr(required(o, "rhs"))};
    else if (node == "weakUntil")
      e.value = ir::PropertyExpr::WeakUntil{propertyPtr(required(o, "lhs")), propertyPtr(required(o, "rhs"))};
    else
      malformed("Unknown PropertyExpr node: " + str(node));
    return e;
  }

  ir::Property property(const Object& o)
  {
    ir::Property p;
    p.name = fieldString(o, "name");
    p.expression = propertyPtr(required(o, "expression"));
    p.span = readSpan(fieldObject(o, "span"));
    return p;
  }

  ir::Component component(const Object& o)
  {
    ir::Component c;
    c.symbol = readSymbol(required(o, "symbol"));
    c.kind = readEnum<ir::ComponentKind>(required(o, "kind"), 0, 1);
    c.capabilityKind = readEnum<ir::CapabilityKind>(required(o, "capabilityKind"), 0, 2);
    c.name = fieldString(o, "name");
    c.arguments = arguments(fieldArray(o, "arguments"));
    c.variables = readArray<ir::Variable>(fieldArray(o, "variables"), [this](const Value& v) { return variable(asObject(v, "variable")); });
    c.actions = readArray<ir::Action>(fieldArray(o, "actions"), [this](const Value& v) { return action(asObject(v, "action")); });
    c.flows = readArray<ir::Flow>(fieldArray(o, "flows"), [this](const Value& v) { return flow(asObject(v, "flow")); });
    c.properties = readArray<ir::Property>(fieldArray(o, "properties"), [this](const Value& v) { return property(asObject(v, "property")); });
    const Object metadata = fieldObject(o, "metadata");
    for (auto it = metadata.begin(); it != metadata.end(); ++it)
      c.metadata.emplace(native(it.key()), asString(it.value(), "component metadata"));
    c.span = readSpan(fieldObject(o, "span"));
    return c;
  }
};

// Check that every IR reference resolves against the reconstructed registry.
// Walk the wire representation so newly added IR fields can be checked by key
// (without building a second full IR tree visitor). Type/metadata objects are
// excluded because their identifiers do not belong to SymbolRegistry.
void validateReferences(const Value& value, const SymbolRegistry& symbols)
{
  if (value.isArray())
  {
    for (const auto& item : value.toArray())
      validateReferences(item, symbols);
    return;
  }
  if (!value.isObject())
    return;
  const Object o = value.toObject();
  for (auto it = o.begin(); it != o.end(); ++it)
  {
    const QString& key = it.key();
    if (key == "type" || key == "literalType" || key == "metadata")
      continue;
    if (key == "symbol" || key == "receiver" || key == "target" || key == "owner" || key == "actionId")
    {
      const SymbolId id = readSymbol(it.value());
      if (id != InvalidSymbol && !symbols.get(id))
        malformed("Unknown SymbolId " + QString::number(id) + " in " + key);
    }
    else
      validateReferences(it.value(), symbols);
  }
}

}  // namespace

QJsonObject IrJson::toJson(const ir::Program& program, const SymbolRegistry& symbols, const std::vector<types::TypeDefinition>& typeDefinitions)
{
  Writer writer;
  Array definitions;
  for (const auto& definition : typeDefinitions)
    definitions.append(maki::typeDefinitionToJson(definition));

  return {
    {"format", "koda.ir"}, {"version", FormatVersion}, {"program", writer.program(program)}, {"symbols", writer.symbols(symbols)}, {"types", definitions},
  };
}

Result<IrArchive> IrJson::fromJson(const QJsonObject& json)
{
  try
  {
    if (fieldString(json, "format") != "koda.ir")
      malformed("Not a KODA IR document");
    if (fieldInt(json, "version") != FormatVersion)
      malformed("Unsupported KODA IR schema version");

    Reader reader;
    IrArchive result;
    result.symbols = reader.symbols(fieldObject(json, "symbols"));
    const Object irObject = fieldObject(json, "program");
    result.program = reader.program(irObject);
    validateReferences(irObject, result.symbols);
    for (const auto& item : fieldArray(json, "types"))
      result.typeDefinitions.push_back(readDefinition(asObject(item, "type definition")));
    return result;
  } catch (const std::exception& e)
  {
    return Result<IrArchive>::Failed("{}", e.what());
  }
}

VoidResult IrJson::save(const QString& filename, const ir::Program& program, const SymbolRegistry& symbols,
                        const std::vector<types::TypeDefinition>& typeDefinitions)
{
  try
  {
    QSaveFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
      return VoidResult::Failed("Could not open {}: {}", native(filename), native(file.errorString()));
    const QByteArray data = QJsonDocument(toJson(program, symbols, typeDefinitions)).toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size())
      return VoidResult::Failed("Could not write {}: {}", native(filename), native(file.errorString()));
    if (!file.commit())
      return VoidResult::Failed("Could not commit {}: {}", native(filename), native(file.errorString()));
    return VoidResult{};
  } catch (const std::exception& e)
  {
    return VoidResult::Failed("{}", e.what());
  }
}

Result<IrArchive> IrJson::load(const QString& filename)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
    return Result<IrArchive>::Failed("Could not open {}: {}", native(filename), native(file.errorString()));
  QJsonParseError error;
  const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
  if (error.error != QJsonParseError::NoError || !document.isObject())
    return Result<IrArchive>::Failed("Invalid JSON in {}: {}", native(filename), native(error.errorString()));
  return fromJson(document.object());
}

}  // namespace koda
