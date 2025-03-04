#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

#include "app_configs.h"
#include "logging.h"
#include "string_helpers.h"

ConfigBase::ConfigBase()
{
}

ConfigBase::ConfigBase(ConfigBase& copy)
    : mIsValid(copy.mIsValid)
{
}

ConfigBase::ConfigBase(const ConfigBase& copy)
    : mIsValid(copy.mIsValid)
{
}

bool ConfigBase::isValid() const
{
  return mIsValid.IsSuccess();
}

void ConfigBase::setInvalid(const QString& message)
{
  mIsValid = VoidResult::Failed(message.toStdString());
}

VoidResult ConfigBase::result() const
{
  return mIsValid;
}

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

QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config)
{
  out << config.position;
  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectorConfig& config)
{
  in >> config.position;
  return in;
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

PropertiesConfig::PropertiesConfig()
{
}

PropertiesConfig::PropertiesConfig(const QJsonObject& object)
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
  if (type == Types::PropertyTypes::UNKNOWN)
  {
    setInvalid("Invalid property type: " + object["type"].toString() + " for " + id);
    return;
  }

  // Set default later for easier comparison
  defaultValue = toDefault(object, type);
  if (!defaultValue.isValid())
    setInvalid("Invalid default value for " + id);
}

QString PropertiesConfig::typeToString() const
{
  if (type == Types::PropertyTypes::STRING)
    return "string";
  else if (type == Types::PropertyTypes::INTEGER)
    return "integer";
  else if (type == Types::PropertyTypes::REAL)
    return "real";
  else if (type == Types::PropertyTypes::BOOLEAN)
    return "boolean";
  else if (type == Types::PropertyTypes::LIST)
    return "list";

  return "unknown";
}

QVariant PropertiesConfig::toDefault(const QJsonObject& object, Types::PropertyTypes objectType)
{
  LOG_INFO("Setting default of type: %d", (int)type);

  if (objectType == Types::PropertyTypes::STRING)
    return object.contains("default") ? object["default"].toString() : QVariant(QString(""));
  else if (objectType == Types::PropertyTypes::INTEGER)
    return object.contains("default") ? object["default"].toInt() : QVariant(qint32(0));
  else if (objectType == Types::PropertyTypes::REAL)
    return object.contains("default") ? object["default"].toDouble() : QVariant(qreal(0));
  else if (objectType == Types::PropertyTypes::BOOLEAN)
    return object.contains("default") ? object["default"].toBool() : QVariant(false);
  else if (objectType == Types::PropertyTypes::LIST)
  {
    qDebug() << object;

    if (object.contains("default"))
    {
      for (auto obj : object["default"].toArray())
        LOG_INFO("Default: %s", qPrintable(obj.toString()));
    }
    else
    {
      LOG_WARNING("No default");
    }

    return object.contains("default") ? object["default"].toArray().toVariantList() : QVariantList();
  }
  else if (objectType == Types::PropertyTypes::SELECT)
  {
    if (!object.contains("options"))
    {
      setInvalid("Select type requires and \"options\" attribute");
      return QVariant();
    }

    for (const auto& option : object["options"].toArray())
    {
      auto prop = PropertiesConfig(option.toObject());
      options.push_back(PropertiesConfig(option.toObject()));
    }

    return toDefault(object, Types::PropertyTypes::STRING);
  }

  return QVariant();
}

Types::PropertyTypes PropertiesConfig::toType(const QString& config)
{
  const auto type = QString::fromStdString(ToLowerCase(config.toStdString(), 0, config.size()));
  if (type == "string")
    return Types::PropertyTypes::STRING;
  else if (type == "integer")
    return Types::PropertyTypes::INTEGER;
  else if (type == "real")
    return Types::PropertyTypes::REAL;
  else if (type == "boolean")
    return Types::PropertyTypes::BOOLEAN;
  else if (type == "select")
    return Types::PropertyTypes::SELECT;
  else if (type == "list")
    return Types::PropertyTypes::LIST;

  return Types::PropertyTypes::UNKNOWN;
}

bool PropertiesConfig::isValid() const
{
  for (const auto& option : options)
    if (!option.isValid())
      return false;

  return ConfigBase::isValid();
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

QDataStream& operator<<(QDataStream& out, const BodyConfig& config)
{
  out << config.shape;
  out << config.textColor;
  out << config.backgroundColor;
  out << config.borderColor;
  out << config.width;
  out << config.height;
  return out;
}

QDataStream& operator>>(QDataStream& in, BodyConfig& config)
{
  in >> config.shape;
  in >> config.textColor;
  in >> config.backgroundColor;
  in >> config.borderColor;
  in >> config.width;
  in >> config.height;
  return in;
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

NodeConfig::NodeConfig(const QJsonObject& object)
{
  if (!object.contains("name"))
  {
    setInvalid("Object must contain a name");
    return;
  }

  name = object["name"].toString();

  if (object.contains("body"))
  {
    body = BodyConfig(object["body"].toObject());
    if (!body.isValid())
      setInvalid(QString::fromStdString(body.result().ErrorMessage()));
  }

  if (object.contains("behaviour"))
  {
    behaviour = BehaviourConfig(object["behaviour"].toObject());
    if (!behaviour.isValid())
      setInvalid(QString::fromStdString(behaviour.result().ErrorMessage()));
  }

  if (object.contains("help"))
  {
    help = HelpConfig(object["help"].toObject());
    if (!help.isValid())
      setInvalid(QString::fromStdString(help.result().ErrorMessage()));
  }

  if (object.contains("properties"))
  {
    for (const auto& property : object["properties"].toArray())
    {
      auto prop = PropertiesConfig(property.toObject());
      if (!prop.isValid())
        setInvalid(QString::fromStdString(prop.result().ErrorMessage()));

      properties.push_back(prop);
    }
  }

  if (object.contains("connectors"))
  {
    for (const auto& connector : object["connectors"].toArray())
    {
      auto conn = ConnectorConfig(connector.toObject());
      if (!conn.isValid())
        setInvalid(QString::fromStdString(conn.result().ErrorMessage()));

      connectors.push_back(conn);
    }
  }

  if (object.contains("controls"))
  {
    for (const auto& control : object["controls"].toArray())
    {
      auto ctrl = ControlsConfig(control.toObject());
      if (!ctrl.isValid())
        setInvalid(QString::fromStdString(ctrl.result().ErrorMessage()));

      controls.push_back(ctrl);
    }
  }
}

QDataStream& operator<<(QDataStream& out, const NodeConfig& config)
{
  out << config.name;
  out << config.body;
  out << config.connectors;
  return out;
}

QDataStream& operator>>(QDataStream& in, NodeConfig& config)
{
  in >> config.name;
  in >> config.body;
  in >> config.connectors;
  return in;
}

QDataStream& operator<<(QDataStream& out,
                        const QVector<ConnectorConfig>& connectors)
{
  out << connectors.size();
  for (const auto& connector : connectors)
    out << connector;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<ConnectorConfig>& connectors)
{
  int size;
  in >> size;
  connectors.clear();
  for (int i = 0; i < size; ++i)
  {
    ConnectorConfig connector;
    in >> connector;
    connectors.append(connector);
  }
  return in;
}
