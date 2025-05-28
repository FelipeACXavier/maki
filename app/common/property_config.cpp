#include "property_config.h"

#include <QJsonArray>

#include "keys.h"

PropertiesConfig::PropertiesConfig()
    : isConfigValid(true)
    , errorMessage("")
{
}

PropertiesConfig::PropertiesConfig(const QJsonObject& object)
    : isConfigValid(true)
    , errorMessage("")
{
  if (!object.contains("id"))
  {
    setInvalid("Missing id attribute in property");
    return;
  }

  if (!object.contains("type"))
  {
    setInvalid("Missing type attribute in property");
    return;
  }

  id = object["id"].toString();
  type = Types::StringToPropertyTypes(object["type"].toString());
  if (type == Types::PropertyTypes::UNKNOWN)
  {
    setInvalid("Invalid property type: " + object["type"].toString() + " for " + id);
    return;
  }

  if (object.contains("options"))
  {
    if (type == Types::PropertyTypes::COMPONENT_SELECT && object["options"].toArray().size() != 1)
    {
      setInvalid("We only support one option for now");
      return;
    }

    for (const auto& option : object["options"].toArray())
      options.push_back(PropertiesConfig(option.toObject()));
  }
  else if (type == Types::PropertyTypes::SELECT)
  {
    setInvalid("Invalid config for " + id + ". " + object["type"].toString() + " type must have options");
    return;
  }

  // Set default later for easier comparison
  defaultValue = toDefault(object, type);
  if (!defaultValue.isValid())
    setInvalid("Invalid default value for " + id);
}

QVariant PropertiesConfig::toDefault(const QJsonObject& object, Types::PropertyTypes objectType)
{
  if (objectType == Types::PropertyTypes::STRING)
    return object.contains("default") ? object["default"].toString() : QVariant(QString(""));
  else if (objectType == Types::PropertyTypes::INTEGER)
    return object.contains("default") ? object["default"].toInt() : QVariant(qint32(0));
  else if (objectType == Types::PropertyTypes::REAL)
    return object.contains("default") ? object["default"].toDouble() : QVariant(qreal(0));
  else if (objectType == Types::PropertyTypes::BOOLEAN)
    return object.contains("default") ? object["default"].toBool() : QVariant(false);
  else if (objectType == Types::PropertyTypes::LIST)
    return object.contains("default") ? object["default"].toArray().toVariantList() : QVariantList();
  else if (objectType == Types::PropertyTypes::COLOR)
    return object.contains("default") ? object["default"].toString() : QVariant(QString("#050505"));
  else if (objectType == Types::PropertyTypes::COMPONENT_SELECT)
  {
    if (!object.contains("options"))
      return toDefault(object, Types::PropertyTypes::STRING);

    // A component select with options must be able to hold its on data as well as the data option
    QJsonObject propOption;
    propOption[ConfigKeys::DATA] = "";
    propOption["data_id"] = "";

    propOption[ConfigKeys::ID] = options.at(0).id;
    propOption[ConfigKeys::TYPE] = Types::PropertyTypesToString(options.at(0).type);
    propOption[ConfigKeys::OPTION_DATA] = options.at(0).defaultValue.toString();
    propOption["option_data_id"] = "";

    return propOption;
  }
  else if (objectType == Types::PropertyTypes::EVENT_SELECT)
    return toDefault(object, Types::PropertyTypes::STRING);
  else if (objectType == Types::PropertyTypes::SELECT)
    return toDefault(object, Types::PropertyTypes::STRING);

  return QVariant();
}

bool PropertiesConfig::isValid() const
{
  for (const auto& option : options)
    if (!option.isValid())
      return false;

  return isConfigValid;
}

void PropertiesConfig::setInvalid(const QString& message)
{
  isConfigValid = false;
  errorMessage = message;
}

QJsonObject PropertiesConfig::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = id;
  data[ConfigKeys::DEFAULT] = defaultValue.toJsonObject();

  QJsonArray optionArray;
  for (const auto& opt : options)
    optionArray.append(opt.toJson());

  data[ConfigKeys::OPTIONS] = optionArray;
  data[ConfigKeys::TYPE] = (int)type;

  return data;
}

PropertiesConfig PropertiesConfig::fromJson(const QJsonObject& data)
{
  PropertiesConfig config;
  config.id = data[ConfigKeys::ID].toString();
  config.defaultValue = QJsonValue(data[ConfigKeys::DEFAULT]).toVariant();

  for (const auto& value : data[ConfigKeys::OPTIONS].toArray())
    config.options.append(fromJson(value.toObject()));

  config.type = (Types::PropertyTypes)data[ConfigKeys::TYPE].toInt();

  return config;
}

QDataStream& operator<<(QDataStream& out, const PropertiesConfig& config)
{
  out << config.id;
  out << config.type;
  out << config.options;
  out << config.defaultValue;
  out << config.errorMessage;
  out << config.isConfigValid;

  return out;
}

QDataStream& operator>>(QDataStream& in, PropertiesConfig& config)
{
  in >> config.id;
  in >> config.type;
  in >> config.options;
  in >> config.defaultValue;
  in >> config.errorMessage;
  in >> config.isConfigValid;

  return in;
}
