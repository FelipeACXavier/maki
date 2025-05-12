#include "event_config.h"

#include <QJsonArray>

#include "keys.h"

EventConfig::EventConfig()
    : isConfigValid(true)
    , errorMessage("")
{
}

EventConfig::EventConfig(const QJsonObject& object)
    : isConfigValid(true)
    , errorMessage("")
{
  if (!object.contains(ConfigKeys::ID))
  {
    setInvalid("Missing event id");
    return;
  }

  if (!object.contains(ConfigKeys::TYPE))
  {
    setInvalid("Missing event type");
    return;
  }

  id = object[ConfigKeys::ID].toString();
  type = Types::StringToConnectorType(object[ConfigKeys::TYPE].toString());
  if (type == Types::ConnectorType::UNKNOWN)
  {
    setInvalid("Invalid type: " + object[ConfigKeys::TYPE].toString() + " for " + id);
    return;
  }

  returnType = Types::StringToPropertyTypes(object[ConfigKeys::RETURN_TYPE].toString());
  if (returnType == Types::PropertyTypes::UNKNOWN)
  {
    setInvalid("Invalid property type: " + object[ConfigKeys::RETURN_TYPE].toString() + " for " + id);
    return;
  }

  for (const auto& arg : object[ConfigKeys::ARGUMENTS].toArray())
    arguments.push_back(PropertiesConfig(arg.toObject()));
}

bool EventConfig::isValid() const
{
  return isConfigValid;
}

void EventConfig::setInvalid(const QString& message)
{
  isConfigValid = false;
  errorMessage = message;
}

QJsonObject EventConfig::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = id;
  data[ConfigKeys::TYPE] = Types::ConnectorTypeToString(type);

  QJsonArray optionArray;
  for (const auto& arg : arguments)
    optionArray.append(arg.toJson());

  data[ConfigKeys::ARGUMENTS] = optionArray;
  data[ConfigKeys::RETURN_TYPE] = Types::PropertyTypesToString(returnType);

  return data;
}

EventConfig EventConfig::fromJson(const QJsonObject& data)
{
  return EventConfig(data);
}

QDataStream& operator<<(QDataStream& out, const EventConfig& config)
{
  out << config.id;
  out << config.returnType;
  out << config.arguments;
  out << config.errorMessage;
  out << config.isConfigValid;

  return out;
}

QDataStream& operator>>(QDataStream& in, EventConfig& config)
{
  in >> config.id;
  in >> config.returnType;
  in >> config.arguments;
  in >> config.errorMessage;
  in >> config.isConfigValid;

  return in;
}
