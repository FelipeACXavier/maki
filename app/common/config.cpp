#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

#include "app_configs.h"
#include "keys.h"
#include "string_helpers.h"

FlowConfig::FlowConfig()
{
}

FlowConfig::FlowConfig(const QJsonObject& object)
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

  name = object[ConfigKeys::ID].toString();
  type = Types::StringToConnectorType(object[ConfigKeys::TYPE].toString());
  if (type == Types::ConnectorType::UNKNOWN)
  {
    setInvalid("Invalid type: " + object[ConfigKeys::TYPE].toString() + " for " + name);
    return;
  }

  returnType = Types::StringToPropertyTypes(object[ConfigKeys::RETURN_TYPE].toString());
  if (returnType == Types::PropertyTypes::UNKNOWN)
  {
    setInvalid("Invalid property type: " + object[ConfigKeys::RETURN_TYPE].toString() + " for " + name);
    return;
  }

  for (const auto& arg : object[ConfigKeys::ARGUMENTS].toArray())
    arguments.push_back(PropertiesConfig(arg.toObject()));
}

ControlsConfig::ControlsConfig()
{
}

ControlsConfig::ControlsConfig(const QJsonObject& object)
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
  type = Types::StringToControlTypes(object["type"].toString());

  if (object.contains("format"))
    format = object["format"].toString();
}

BehaviourConfig::BehaviourConfig()
{
}

BehaviourConfig::BehaviourConfig(const QJsonObject& object)
{
  if (object.contains("code"))
    code = object["code"].toString();
}

HelpConfig::HelpConfig()
{
}

HelpConfig::HelpConfig(const QJsonObject& object)
{
  if (object.contains("message"))
    message = object["message"].toString();
}

BodyConfig::BodyConfig()
{
}

BodyConfig::BodyConfig(const QJsonObject& object)
{
  if (object.contains("width"))
    width = object["width"].toInt();

  if (object.contains("height"))
    height = object["height"].toInt();

  if (object.contains("backgroundColor"))
    backgroundColor = QColor(object["backgroundColor"].toString());

  if (object.contains("textColor"))
    textColor = QColor(object["textColor"].toString());

  if (object.contains("borderColor"))
    borderColor = QColor(object["borderColor"].toString());

  if (object.contains("borderRadius"))
    borderRadius = object["borderRadius"].toInt();

  if (object.contains("shape"))
    shape = toShape(object["shape"].toString());

  if (object.contains("icon"))
    iconPath = object["icon"].toString();

  if (object.contains("scale"))
    iconScale = object["scale"].toDouble();

  if (object.contains("z-index"))
    zIndex = object["z-index"].toInt();
}

Types::Shape BodyConfig::toShape(const QString& config) const
{
  // Make configuration a bit easier by making it case independent
  const auto shape = QString::fromStdString(ToLowerCase(config.toStdString(), 0, config.size()));

  if (shape == "rectangle")
    return Types::Shape::RECTANGLE;
  else if (shape == "rounded rectangle")
    return Types::Shape::ROUNDED_RECTANGLE;
  else if (shape == "ellipse")
    return Types::Shape::ELLIPSE;
  else if (shape == "diamond")
    return Types::Shape::DIAMOND;

  return Types::Shape::ROUNDED_RECTANGLE;
}

NodeConfig::NodeConfig()
{
}

NodeConfig::NodeConfig(const QJsonObject& object)
{
  if (!object.contains("type"))
  {
    setInvalid("Object must contain a type");
    return;
  }

  type = object["type"].toString();

  if (object.contains("body"))
  {
    body = BodyConfig(object["body"].toObject());
    if (!body.isValid())
      setInvalid(body.errorMessage);
  }

  if (object.contains("behaviour"))
  {
    behaviour = BehaviourConfig(object["behaviour"].toObject());
    if (!behaviour.isValid())
      setInvalid(behaviour.errorMessage);
  }

  if (object.contains("help"))
  {
    help = HelpConfig(object["help"].toObject());
    if (!help.isValid())
      setInvalid(help.errorMessage);
  }

  if (object.contains("properties"))
  {
    for (const auto& property : object["properties"].toArray())
    {
      auto prop = PropertiesConfig(property.toObject());
      if (!prop.isValid())
        setInvalid(prop.errorMessage);

      properties.push_back(prop);
    }
  }

  if (object.contains("controls"))
  {
    for (const auto& control : object["controls"].toArray())
    {
      auto ctrl = ControlsConfig(control.toObject());
      if (!ctrl.isValid())
        setInvalid(ctrl.errorMessage);

      controls.push_back(ctrl);
    }
  }

  if (object.contains("events"))
  {
    for (const auto& control : object["events"].toArray())
    {
      auto ctrl = FlowConfig(control.toObject());
      if (!ctrl.isValid())
        setInvalid(ctrl.errorMessage);

      events.push_back(ctrl);
    }
  }
}

// ===========================================================================================================
// NodeConfig
QDataStream& operator<<(QDataStream& out, const NodeConfig& config)
{
  out << config.type;
  out << config.body;
  out << config.help;
  out << config.behaviour;
  out << config.controls;
  out << config.properties;
  out << config.libraryType;
  out << config.events;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeConfig& config)
{
  in >> config.type;
  in >> config.body;
  in >> config.help;
  in >> config.behaviour;
  in >> config.controls;
  in >> config.properties;
  in >> config.libraryType;
  in >> config.events;

  return in;
}

// ===========================================================================================================
// BodyConfig
QDataStream& operator<<(QDataStream& out, const BodyConfig& config)
{
  out << config.shape;
  out << config.textColor;
  out << config.backgroundColor;
  out << config.borderColor;
  out << config.width;
  out << config.height;
  out << config.zIndex;
  out << config.borderRadius;
  out << config.iconPath;
  out << config.iconScale;

  return out;
}

QDataStream& operator>>(QDataStream& in, BodyConfig& config)
{
  return in;
}

// ===========================================================================================================
// HelpConfig
QDataStream& operator<<(QDataStream& out, const HelpConfig& config)
{
  out << config.message;

  return out;
}

QDataStream& operator>>(QDataStream& in, HelpConfig& config)
{
  return in;
}

// ===========================================================================================================
// BehaviourConfig
QDataStream& operator<<(QDataStream& out, const BehaviourConfig& config)
{
  out << config.code;

  return out;
}

QDataStream& operator>>(QDataStream& in, BehaviourConfig& config)
{
  return in;
}

// ===========================================================================================================
// ControlsConfig
QDataStream& operator<<(QDataStream& out, const ControlsConfig& config)
{
  out << config.id;
  out << config.type;
  out << config.format;

  return out;
}

QDataStream& operator>>(QDataStream& in, ControlsConfig& config)
{
  return in;
}

// ===========================================================================================================
// FlowConfig
QDataStream& operator<<(QDataStream& out, const FlowConfig& config)
{
  out << config.name;
  out << config.type;
  out << config.returnType;
  out << config.arguments;

  return out;
}

QDataStream& operator>>(QDataStream& in, FlowConfig& config)
{
  return in;
}
