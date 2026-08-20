#include "mission_parameter.h"

#include <format>

#include "type_registry.h"
#include "typing/helpers.h"

namespace maki
{
bool Value::isValid() const
{
  return !std::holds_alternative<std::monostate>(data);
}

bool Value::toBool() const
{
  if (std::holds_alternative<bool>(data))
    return std::get<bool>(data);
  else
    return false;
}

int Value::toInt() const
{
  if (std::holds_alternative<int>(data))
    return std::get<int>(data);
  else if (std::holds_alternative<QString>(data))
  {
    bool ok = false;
    int val = std::get<QString>(data).toInt(&ok);
    if (ok)
      return val;
  }
  else if (std::holds_alternative<std::string>(data))
  {
    auto val = std::get<std::string>(data);
    try
    {
      return std::stoi(val);
    } catch (const std::exception& e)
    {
      // Do nothing really
    }
  }

  return 0;
}

double Value::toDouble() const
{
  if (std::holds_alternative<double>(data))
    return std::get<double>(data);
  else if (std::holds_alternative<QString>(data))
  {
    bool ok = false;
    double val = std::get<QString>(data).toDouble(&ok);
    if (ok)
      return val;
  }
  else if (std::holds_alternative<std::string>(data))
  {
    auto val = std::get<std::string>(data);
    try
    {
      return std::stod(val);
    } catch (const std::exception& e)
    {
      // Do nothing really
    }
  }

  return 0;
}

QString Value::toString() const
{
  if (std::holds_alternative<std::string>(data))
    return QString::fromStdString(std::get<std::string>(data));
  else if (std::holds_alternative<QString>(data))
    return std::get<QString>(data);
  else
    return QString();
}

RecordValue Value::toRecord() const
{
  if (std::holds_alternative<RecordValue>(data))
    return std::get<RecordValue>(data);
  else
    return RecordValue{};
}

ListValue Value::toList() const
{
  if (std::holds_alternative<ListValue>(data))
    return std::get<ListValue>(data);
  else
    return ListValue{};
}

MapValue Value::toMap() const
{
  if (std::holds_alternative<MapValue>(data))
    return std::get<MapValue>(data);
  else
    return MapValue{};
}

QString Value::toReadable() const
{
  if (std::holds_alternative<bool>(data))
  {
    return std::get<bool>(data) ? "true" : "false";
  }
  else if (std::holds_alternative<int>(data))
  {
    return QString::number(std::get<int>(data));
  }
  else if (std::holds_alternative<double>(data))
  {
    return QString::number(std::get<double>(data));
  }
  else if (std::holds_alternative<std::string>(data))
  {
    return QString::fromStdString(std::get<std::string>(data));
  }
  else if (std::holds_alternative<QString>(data))
  {
    return std::get<QString>(data);
  }
  else if (std::holds_alternative<RecordValue>(data))
  {
    bool first = true;
    std::string out = "{";
    for (const auto& [key, value] : std::get<RecordValue>(data))
    {
      if (!first)
        out += ", ";

      out += std::format("{}: {}", key, value.toReadable().toStdString());
      first = false;
    }

    return QString::fromStdString(out + "}");
  }
  else if (std::holds_alternative<ListValue>(data))
  {
    bool first = true;
    std::string out = "{";
    for (const auto& value : std::get<ListValue>(data))
    {
      if (!first)
        out += ", ";

      out += std::format("{}", value.toReadable().toStdString());
      first = false;
    }

    return QString::fromStdString(out + "}");
  }
  else if (std::holds_alternative<MapValue>(data))
  {
    bool first = true;
    std::string out = "{";
    for (const auto& [key, value] : std::get<MapValue>(data))
    {
      if (!first)
        out += ", ";

      out += std::format("<{}, {}>", key.toReadable().toStdString(), value.toReadable().toStdString());
      first = false;
    }

    return QString::fromStdString(out + "}");
  }
  else if (std::holds_alternative<QColor>(data))
  {
    return std::get<QColor>(data).name();
  }

  return QString("None");
}

Value Value::defaultValue(const koda::types::TypeReference& ref)
{
  Value out;
  if (ref.isList())
  {
    out.data = ListValue{};
  }
  else if (ref.isMap())
  {
    out.data = MapValue{};
  }
  else if (ref.isPrimitive())
  {
    auto kind = ref.primitiveKind();
    if (koda::types::isInteger(kind))
      out.data = 0;
    else if (koda::types::isFloatingPoint(kind))
      out.data = 0.0;
    else if (kind == koda::types::PrimitiveKind::String)
      out.data = QString();
    else if (kind == koda::types::PrimitiveKind::Bool)
      out.data = false;
    else if (kind == koda::types::PrimitiveKind::Bytes)
      out.data = QString();
  }
  else
  {
    const auto* definition = TypeRegistry::instance().resolve(ref);
    if (!definition)
      return out;

    if (definition->isRecord())
      out.data = RecordValue{};
    else if (definition->isAlias())
      out = Value::defaultValue(definition->alias().target);
    else if (definition->isEnum())
      out.data = QString::fromStdString(definition->enumeration().values.front().name);
  }

  return out;
}

bool Value::operator<(const Value& other) const
{
  // Different types are ordered according to their position in the variant.
  if (data.index() != other.data.index())
    return data.index() < other.data.index();

  switch (data.index())
  {
    case 0:  // monostate
      return false;

    case 1:  // bool
      return std::get<bool>(data) < std::get<bool>(other.data);

    case 2:  // int
      return std::get<int>(data) < std::get<int>(other.data);

    case 3:  // double
    {
      const double lhs = std::get<double>(data);
      const double rhs = std::get<double>(other.data);

      // NaN needs special treatment to preserve strict weak ordering.
      const bool lhsNan = std::isnan(lhs);
      const bool rhsNan = std::isnan(rhs);

      if (lhsNan != rhsNan)
        return lhsNan;

      if (lhsNan)
        return false;

      return lhs < rhs;
    }

    case 4:  // std::string
      return std::get<std::string>(data) < std::get<std::string>(other.data);

    case 5:  // RecordValue
      return std::get<RecordValue>(data) < std::get<RecordValue>(other.data);

    case 6:  // ListValue
      return std::get<ListValue>(data) < std::get<ListValue>(other.data);

    case 7:  // MapValue
      return std::get<MapValue>(data) < std::get<MapValue>(other.data);

    case 8:  // QColor
      return std::get<QColor>(data).rgba() < std::get<QColor>(other.data).rgba();

    case 9:  // QString
      return QString::compare(std::get<QString>(data), std::get<QString>(other.data)) < 0;

    default:
      return false;
  }
}

}  // namespace maki