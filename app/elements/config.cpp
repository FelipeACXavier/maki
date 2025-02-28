#include "config.h"

#include <QJsonArray>
#include <QPoint>
#include <QRect>

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
  else if (position == "south")
    return QPointF(bounds.width() / 2, bounds.height());
  else if (position == "east")
    return QPointF(bounds.width(), bounds.height() / 2);
  else if (position == "west")
    return QPointF(0, bounds.height() / 2);

  return QPointF(0, 0);
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
