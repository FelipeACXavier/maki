#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

#include "app_configs.h"
#include "string_helpers.h"

ConnectorConfig::ConnectorConfig()
{
}

ConnectorConfig::ConnectorConfig(const QJsonObject& object)
{
  if (object.contains("id"))
    id = object["id"].toString();

  if (object.contains("position"))
  {
    const auto config = object["position"].toString();
    position = QString::fromStdString(ToLowerCase(config.toStdString(), 0, config.size()));
  }

  if (object.contains("type"))
    type = fromString(object["type"].toString());
}

QPointF ConnectorConfig::getPosition(const QRectF& bounds) const
{
  if (position == "north")
    return QPointF(bounds.width() / 2, 0);
  else if (position == "north west")
    return QPointF(bounds.width() / 4, 0);
  else if (position == "north east")
    return QPointF(3 * bounds.width() / 4, 0);

  else if (position == "south")
    return QPointF(bounds.width() / 2, bounds.height());
  else if (position == "south west")
    return QPointF(bounds.width() / 4, bounds.height());
  else if (position == "south east")
    return QPointF(3 * bounds.width() / 4, bounds.height());

  else if (position == "east")
    return QPointF(bounds.width(), bounds.height() / 2);
  else if (position == "east north")
    return QPointF(bounds.width(), bounds.height() / 4);
  else if (position == "east south")
    return QPointF(bounds.width(), 3 * bounds.height() / 4);

  else if (position == "west")
    return QPointF(0, bounds.height() / 2);
  else if (position == "west north")
    return QPointF(0, bounds.height() / 4);
  else if (position == "west south")
    return QPointF(0, 3 * bounds.height() / 4);

  return QPointF(0, 0);
}

QPointF ConnectorConfig::getShift(const QString& config) const
{
  auto pos = config.isEmpty() ? position : config;

  if (pos == "north" || pos == "north west" || pos == "north east")
    return QPointF(0, -Constants::CONTROL_POINT_SHIFT);

  else if (pos == "south" || pos == "south west" || pos == "south east")
    return QPointF(0, Constants::CONTROL_POINT_SHIFT);

  else if (pos == "east" || pos == "east north" || pos == "east south")
    return QPointF(Constants::CONTROL_POINT_SHIFT, 0);

  else if (pos == "west" || pos == "west north" || pos == "west north")
    return QPointF(-Constants::CONTROL_POINT_SHIFT, 0);

  return QPointF(0, 0);
}

QPointF ConnectorConfig::getMirrorShift() const
{
  if (position == "north" || position == "north west" ||
      position == "north east")
    return getShift("south");

  else if (position == "south" || position == "south west" ||
           position == "south east")
    return getShift("north");

  else if (position == "east" || position == "east north" ||
           position == "east south")
    return getShift("west");

  return getShift("east");
}

Types::ConnectorType ConnectorConfig::fromString(const QString& config) const
{
  const auto type = QString::fromStdString(ToLowerCase(config.toStdString(), 0, config.size()));
  if (config == "in")
    return Types::ConnectorType::IN;
  else if (config == "out")
    return Types::ConnectorType::OUT;
  else if (config == "inout")
    return Types::ConnectorType::IN_AND_OUT;

  return Types::ConnectorType::UNKNOWN;
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
  type = toType(object["type"].toString());

  if (object.contains("format"))
    format = object["format"].toString();
}

Types::ControlTypes ControlsConfig::toType(const QString& config) const
{
  const auto type = QString::fromStdString(ToLowerCase(config.toStdString(), 0, config.size()));
  if (type == "add control")
    return Types::ControlTypes::ADD_CONTROL;
  else if (type == "remove control")
    return Types::ControlTypes::REMOVE_CONTROL;
  else if (type == "add field")
    return Types::ControlTypes::ADD_FIELD;

  return Types::ControlTypes::UNKNOWN;
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

  if (object.contains("connectors"))
  {
    for (const auto& connector : object["connectors"].toArray())
    {
      auto conn = ConnectorConfig(connector.toObject());
      if (!conn.isValid())
        setInvalid(conn.errorMessage);

      connectors.push_back(conn);
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
  out << config.connectors;
  out << config.properties;
  out << config.libraryType;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeConfig& config)
{
  in >> config.type;
  in >> config.body;
  in >> config.help;
  in >> config.behaviour;
  in >> config.controls;
  in >> config.connectors;
  in >> config.properties;
  in >> config.libraryType;

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
// ConnectorConfig
QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config)
{
  out << config.id;
  out << config.position;
  out << config.type;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectorConfig& config)
{
  return in;
}
