#include "mission_parameter.h"

#include <format>

#include "type_helpers.h"
#include "type_registry.h"
#include "typing/helpers.h"

namespace maki
{
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
  else if (std::holds_alternative<int>(data))
    return QString::number(std::get<int>(data));
  else if (std::holds_alternative<double>(data))
    return QString::number(std::get<double>(data));
  else if (std::holds_alternative<bool>(data))
    return std::get<bool>(data) ? "true" : "false";
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
    {
      out.data = RecordValue{};
    }
    else if (definition->isAlias())
    {
      out = Value::defaultValue(definition->alias().target);
    }
    else if (definition->isEnum())
    {
      if (!definition->enumeration().values.empty())
        out.data = QString::fromStdString(definition->enumeration().values.front().name);

      auto primitiveKind = koda::types::enumKindFromEnumKind(definition->enumeration().underlyingType);
      out = Value::defaultValue(koda::types::TypeReference::primitive(primitiveKind));
    }
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

// ===================================================================
// All the stuff for the interface
IValue::Kind Value::kind() const
{
  return static_cast<IValue::Kind>(data.index());
}

bool Value::isValid() const
{
  return !std::holds_alternative<std::monostate>(data);
}

bool Value::toBoolValue() const
{
  return toBool();
}

int Value::toIntValue() const
{
  return toInt();
}

double Value::toDoubleValue() const
{
  return toDouble();
}

QString Value::toStringValue() const
{
  return toString();
}

IValue::IRecordValue Value::toRecordValue() const
{
  IRecordValue out;
  for (const auto& [key, value] : toRecord())
    out.emplace(key, std::make_shared<Value>(value));

  return out;
}

IValue::IListValue Value::toListValue() const
{
  IListValue out;
  for (const auto& value : toList())
    out.push_back(std::make_shared<Value>(value));

  return out;
}

IValue::IMapValue Value::toMapValue() const
{
  IMapValue out;
  for (const auto& [key, value] : toMap())
    out.emplace(std::make_shared<Value>(key), std::make_shared<Value>(value));

  return out;
}

QJsonObject Value::toJson() const
{
  QJsonObject json;
  switch (kind())
  {
    case IValue::Kind::Bool:
      json["kind"] = "bool";
      json["value"] = toBool();
      break;

    case IValue::Kind::Int:
      json["kind"] = "int";
      json["value"] = toInt();
      break;

    case IValue::Kind::Double:
      json["kind"] = "double";
      json["value"] = toDouble();
      break;

    case IValue::Kind::StdString:
      json["kind"] = "std_string";
      json["value"] = QString::fromStdString(std::get<std::string>(data));
      break;

    case IValue::Kind::QString:
      json["kind"] = "qstring";
      json["value"] = std::get<QString>(data);
      break;

    case IValue::Kind::Color:
    {
      json["kind"] = "color";
      json["value"] = std::get<QColor>(data).name(QColor::HexArgb);
      break;
    }

    case IValue::Kind::Record:
    {
      json["kind"] = "record";

      QJsonObject fields;
      for (const auto& [name, fieldValue] : toRecord())
        fields[QString::fromStdString(name)] = fieldValue.toJson();

      json["value"] = fields;
      break;
    }

    case IValue::Kind::List:
    {
      json["kind"] = "list";

      QJsonArray values;
      for (const auto& item : toList())
        values.append(item.toJson());

      json["value"] = values;
      break;
    }

    case IValue::Kind::Map:
    {
      json["kind"] = "map";

      QJsonArray entries;
      for (const auto& [key, value] : toMap())
      {
        QJsonObject entry;
        entry["key"] = key.toJson();
        entry["value"] = value.toJson();

        entries.append(entry);
      }

      json["value"] = entries;
      break;
    }

    case IValue::Kind::Invalid:
    default:
      json["kind"] = "invalid";
      json["value"] = QJsonValue::Null;
      break;
  }

  return json;
}

Value Value::fromJson(const QJsonObject& json)
{
  Value result;

  const auto kind = json["kind"].toString();
  const auto data = json["value"];

  if (kind == "bool")
  {
    result.data = data.toBool();
  }
  else if (kind == "int")
  {
    result.data = data.toInt();
  }
  else if (kind == "double")
  {
    result.data = data.toDouble();
  }
  else if (kind == "std_string")
  {
    result.data = data.toString().toStdString();
  }
  else if (kind == "qstring")
  {
    result.data = data.toString();
  }
  else if (kind == "color")
  {
    result.data = QColor(data.toString());
  }
  else if (kind == "record")
  {
    RecordValue record;

    const auto fields = data.toObject();

    for (auto it = fields.begin(); it != fields.end(); ++it)
      record[it.key().toStdString()] = Value::fromJson(it.value().toObject());

    result.data = std::move(record);
  }
  else if (kind == "list")
  {
    ListValue list;

    const auto array = data.toArray();

    list.reserve(array.size());

    for (const auto& item : array)
      list.push_back(Value::fromJson(item.toObject()));

    result.data = std::move(list);
  }
  else if (kind == "map")
  {
    MapValue map;

    const auto entries = data.toArray();

    for (const auto& item : entries)
    {
      const auto entry = item.toObject();

      auto key = Value::fromJson(entry["key"].toObject());
      auto mappedValue = Value::fromJson(entry["value"].toObject());

      map.emplace(std::move(key), std::move(mappedValue));
    }

    result.data = std::move(map);
  }
  else
  {
    result.data = std::monostate{};
  }

  return result;
}

// ===================================================================
// MissionPaarameter
std::string MissionParameter::getid() const
{
  return id;
}

std::string MissionParameter::getname() const
{
  return name;
}

koda::types::TypeReference MissionParameter::gettype() const
{
  return type;
}

const std::shared_ptr<IValue> MissionParameter::getvalue() const
{
  return std::make_shared<Value>(value);
}

QJsonObject MissionParameter::toJson() const
{
  QJsonObject json;

  json["id"] = QString::fromStdString(id);
  json["name"] = QString::fromStdString(name);
  json["type"] = maki::typeReferenceToJson(type);
  json["value"] = value.toJson();

  return json;
}

Result<MissionParameter> MissionParameter::fromJson(const QJsonObject& json)
{
  MissionParameter parameter;

  parameter.id = json["id"].toString().toStdString();
  parameter.name = json["name"].toString().toStdString();
  ASSIGN_OR_RETURN_ON_FAILURE_AS(parameter.type, maki::typeReferenceFromJson(json["type"].toObject()), MissionParameter);
  parameter.value = Value::fromJson(json["value"].toObject());

  return parameter;
}

}  // namespace maki