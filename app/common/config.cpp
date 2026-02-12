#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

#include "app_configs.h"
#include "keys.h"
#include "string_helpers.h"

PropertyConfig::PropertyConfig()
{
}

PropertyConfig::PropertyConfig(const QJsonObject& object)
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

  if (object.contains(ConfigKeys::OPTIONS))
  {
    // if (type == Types::PropertyTypes::COMPONENT_SELECT && object["options"].toArray().size() != 1)
    // {
    //   setInvalid("We only support one option for now");
    //   return;
    // }

    for (const auto& option : object[ConfigKeys::OPTIONS].toArray())
      options.push_back(PropertyConfig(option.toObject()));
  }
  else if (type == Types::PropertyTypes::SELECT)
  {
    setInvalid("Invalid config for " + id + ". " + object["type"].toString() + " type must have options");
    return;
  }

  if (object.contains(ConfigKeys::INFO))
    info = object[ConfigKeys::INFO].toString();

  // Set default later for easier comparison
  defaultValue = toDefault(object, type);
  if (!defaultValue.isValid())
    setInvalid("Invalid default value for " + id);
}

QVariant PropertyConfig::toDefault(const QJsonObject& object, Types::PropertyTypes objectType)
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
    if (!object.contains(ConfigKeys::OPTIONS))
      return toDefault(object, Types::PropertyTypes::STRING);

    QJsonObject defaultObject;
    QJsonArray array;
    for (const auto& option : options)
    {
      // A component select with options must be able to hold its on data as well as the data option
      QJsonObject propOption;
      propOption[ConfigKeys::DATA] = "";
      propOption["data_id"] = "";

      propOption[ConfigKeys::ID] = option.id;
      propOption[ConfigKeys::TYPE] = Types::PropertyTypesToString(option.type);
      propOption[ConfigKeys::OPTION_DATA] = option.defaultValue.toString();
      propOption["option_data_id"] = "";

      array.append(propOption);
    }

    defaultObject[ConfigKeys::OPTIONS] = array;

    return defaultObject;
  }
  else if (objectType == Types::PropertyTypes::EVENT_SELECT)
    return toDefault(object, Types::PropertyTypes::STRING);
  else if (objectType == Types::PropertyTypes::SELECT)
    return toDefault(object, Types::PropertyTypes::STRING);
  else if (objectType == Types::PropertyTypes::ENUM)
    return toDefault(object, Types::PropertyTypes::STRING);

  return QVariant();
}

TransitionConfig::TransitionConfig()
{
}

TransitionConfig::TransitionConfig(const QJsonObject& object)
{
  if (!object.contains(ConfigKeys::ID))
  {
    setInvalid("Missing event id");
    return;
  }

  id = object[ConfigKeys::ID].toString();
  if (object.contains(ConfigKeys::LABEL))
    label = object[ConfigKeys::LABEL].toString();

  if (object.contains(ConfigKeys::MODIFIABLE))
    modifiable = object[ConfigKeys::MODIFIABLE].toBool();
}

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
    arguments.push_back(PropertyConfig(arg.toObject()));

  if (object.contains(ConfigKeys::MODIFIABLE))
    modifiable = object[ConfigKeys::MODIFIABLE].toBool();

  if (object.contains(ConfigKeys::INFO))
    info = object[ConfigKeys::INFO].toString();
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
      auto prop = PropertyConfig(property.toObject());
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

  if (object.contains(ConfigKeys::TRANSITIONS))
  {
    for (const auto& control : object[ConfigKeys::TRANSITIONS].toArray())
    {
      auto ctrl = TransitionConfig(control.toObject());
      if (!ctrl.isValid())
        setInvalid(ctrl.errorMessage);

      transitions.push_back(ctrl);
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
  out << config.transitions;

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
  in >> config.transitions;

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
  out << config.info;

  return out;
}

QDataStream& operator>>(QDataStream& in, FlowConfig& config)
{
  return in;
}

// ===========================================================================================================
// TransitionConfig
QDataStream& operator<<(QDataStream& out, const TransitionConfig& config)
{
  out << config.id;
  out << config.label;
  out << config.modifiable;

  return out;
}

QDataStream& operator>>(QDataStream& in, TransitionConfig& config)
{
  return in;
}

// ===========================================================================================================
// PropertyConfig
QDataStream& operator<<(QDataStream& out, const PropertyConfig& config)
{
  out << config.id;
  out << config.defaultValue;
  out << config.type;
  out << config.options;
  out << config.info;

  return out;
}

QDataStream& operator>>(QDataStream& in, PropertyConfig& config)
{
  in >> config.id;
  in >> config.defaultValue;
  in >> config.type;
  in >> config.options;
  in >> config.info;

  return in;
}