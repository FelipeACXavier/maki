#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

#include "app_configs.h"
#include "logging.h"

ConnectorConfig::ConnectorConfig()
{
}

ConnectorConfig::ConnectorConfig(const QJsonObject& object)
{
  if (object.contains("position"))
    position = object["position"].toString();
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

  LOG_DEBUG("Using pos: %s", qPrintable(pos));

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
  if (position == "north" || position == "north west" || position == "north east")
    return getShift("south");

  else if (position == "south" || position == "south west" || position == "south east")
    return getShift("north");

  else if (position == "east" || position == "east north" || position == "east south")
    return getShift("west");

  return getShift("east");
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

  if (object.contains("shape"))
    shape = toShape(object["shape"].toString());
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

Type::Shape BodyConfig::toShape(const QString& config) const
{
  if (config == "Rectangle")
    return Type::Shape::RECTANGLE;
  else if (config == "Rounded rectangle")
    return Type::Shape::ROUNDED_RECTANGLE;
  else if (config == "Ellipse")
    return Type::Shape::ELLIPSE;
  else if (config == "Diamond")
    return Type::Shape::DIAMOND;

  return Type::Shape::ROUNDED_RECTANGLE;
}

NodeConfig::NodeConfig(const QJsonObject& object)
{
  assert(object.contains("name"));

  name = object["name"].toString();
  if (object.contains("body"))
    body = BodyConfig(object["body"].toObject());

  for (const auto& connector : object["connectors"].toArray())
    connectors.push_back(ConnectorConfig(connector.toObject()));
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

QDataStream& operator<<(QDataStream& out, const QVector<ConnectorConfig>& connectors)
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
