#include "connector.h"

#include <QBrush>
#include <QGraphicsSceneHoverEvent>

#include "app_configs.h"

Connector::Connector(const ConnectorConfig& config, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(std::make_shared<ConnectorConfig>(config))
{
  setZValue(2);
  setBrush(typeToColor(mConfig->type));
  setAcceptHoverEvents(true);

  const QPointF center = config.getPosition(parent->boundingRect());
  const QPointF shift = QPointF(Config::CONNECTOR_RADIUS, Config::CONNECTOR_RADIUS);

  setRect(QRectF(center - shift, center + shift));
}

Connector::~Connector()
{
  auto copy = mConnections;
  for (auto& item : copy)
    delete item;
}

QString Connector::Id() const
{
  return mId;
}

int Connector::type() const
{
  return Type;
}

QString Connector::connectorId() const
{
  return mConfig->id;
}

Types::ConnectorType Connector::connectorType() const
{
  return mConfig->type;
}

QVector<ConnectionItem*> Connector::connections() const
{
  return mConnections;
}

QPointF Connector::center() const
{
  return scenePos() + mConfig->getPosition(parentItem()->boundingRect());
}

QPair<QPointF, QPointF> Connector::shift() const
{
  return QPair<QPointF, QPointF>(mConfig->getShift(), mConfig->getMirrorShift());
}

void Connector::updateConnections()
{
  for (auto& conn : mConnections)
    conn->move(Id(), center());
}

void Connector::addConnection(ConnectionItem* connection)
{
  mConnections.push_back(connection);
  updateConnections();
}

void Connector::removeConnection(ConnectionItem* connection)
{
  mConnections.removeIf([connection](ConnectionItem* item)
                        { return item->id() == connection->id(); });
}

void Connector::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  // Check if hovering over a connection point
  if (contains(event->pos()))
    updateColor(true);

  QGraphicsEllipseItem::hoverEnterEvent(event);
}

void Connector::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  // Reset color when leaving the connection point
  updateColor(false);

  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void Connector::updateColor(bool accent)
{
  setBrush(accent ? Config::Colours::ACCENT : typeToColor(mConfig->type));
}

Qt::GlobalColor Connector::typeToColor(Types::ConnectorType type) const
{
  switch (type)
  {
    case Types::ConnectorType::IN:
      return Qt::blue;
    case Types::ConnectorType::OUT:
      return Qt::yellow;
    case Types::ConnectorType::IN_AND_OUT:
      return Qt::green;
    default:
    case Types::ConnectorType::UNKNOWN:
      return Qt::red;
  }
}
