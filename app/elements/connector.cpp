#include "connector.h"

#include <QBrush>
#include <QGraphicsSceneHoverEvent>

#include "app_configs.h"
#include "logging.h"

Connector::Connector(const ConnectorConfig& config, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(std::make_shared<ConnectorConfig>(config))
{
  initialize();
}

Connector::Connector(const ConnectorConfig& config, const QString& id, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , mId(id)
    , mConfig(std::make_shared<ConnectorConfig>(config))
{
  initialize();
}

void Connector::initialize()
{
  setZValue(2);
  setBrush(typeToColor(mConfig->type));
  setAcceptHoverEvents(true);

  const QPointF center = mConfig->getPosition(parentItem()->boundingRect());
  const QPointF shift = QPointF(Config::CONNECTOR_RADIUS, Config::CONNECTOR_RADIUS);

  setRect(QRectF(center - shift, center + shift));
}

Connector::~Connector()
{
  auto copy = mConnections;
  for (auto& item : copy)
    delete item;
}

QString Connector::id() const
{
  return mId;
}

int Connector::type() const
{
  return Type;
}

QString Connector::connectorName() const
{
  return mConfig->id;
}

Types::ConnectorType Connector::connectorType() const
{
  return mConfig->type;
}

QVector<IConnection*> Connector::connections() const
{
  return mConnections;
}

QVector<IConnection*> Connector::connectionsFromThis() const
{
  QVector<IConnection*> fromThis;
  for (const auto& conn : connections())
  {
    if (conn->source()->id() == id())
      fromThis.push_back(conn);
  }

  return fromThis;
}

QVector<IConnection*> Connector::connectionsToThis() const
{
  QVector<IConnection*> toThis;
  for (const auto& conn : connections())
  {
    if (conn->destination()->id() == id())
      toThis.push_back(conn);
  }

  return toThis;
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
  const QPointF centerPoint = mConfig->getPosition(parentItem()->boundingRect());
  const QPointF shift = QPointF(Config::CONNECTOR_RADIUS, Config::CONNECTOR_RADIUS);

  setRect(QRectF(centerPoint - shift, centerPoint + shift));

  for (auto& conn : connections())
    dynamic_cast<ConnectionItem*>(conn)->move(id(), center());
}

void Connector::addConnection(ConnectionItem* connection)
{
  mConnections.push_back(connection);
  updateConnections();
}

void Connector::removeConnection(ConnectionItem* connection)
{
  mConnections.removeIf([connection](IConnection* item) {
    return item->id() == connection->id();
  });
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

ConnectorSaveInfo Connector::saveInfo() const
{
  ConnectorSaveInfo info;
  info.connectorId = id();
  info.configId = mConfig->id;

  return info;
}

void Connector::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  for (auto& connection : connections())
    static_cast<ConnectionItem*>(connection)->setOpacity(opacity());

  QGraphicsEllipseItem::paint(painter, style, widget);
}
