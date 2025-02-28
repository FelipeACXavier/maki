#include "node.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUuid>

#include "app_configs.h"
#include "connection.h"
#include "style_helpers.h"

Connector::Connector(const QPointF& center, int radius, QGraphicsItem* parent)
    : QGraphicsEllipseItem(QRectF(center - QPointF(radius, radius), center + QPointF(radius, radius)), parent)
    , mId(QUuid::createUuid().toString())
    , mCenter(center)
{
  setZValue(1);
  setBrush(Qt::blue);
  setAcceptHoverEvents(true);
}

QString Connector::Id() const
{
  return mId;
}

int Connector::type() const
{
  return Type;
}

QPointF Connector::center() const
{
  return scenePos() + mCenter;
}

void Connector::updateConnections()
{
  for (auto& conn : mConnections)
    conn->move(Id(), center());
}

void Connector::addConnection(std::shared_ptr<ConnectionItem> connection)
{
  mConnections.push_back(connection);
  updateConnections();
}

void Connector::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  // Check if hovering over a connection point
  if (contains(event->pos()))
    setBrush(Qt::green);
  else if (contains(event->pos()))
    setBrush(Qt::green);

  QGraphicsEllipseItem::hoverEnterEvent(event);
}

void Connector::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  // Reset color when leaving the connection point
  setBrush(Qt::blue);
  setBrush(Qt::blue);

  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& pixmap, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(config)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  mPixmapItem = std::make_shared<QGraphicsPixmapItem>(pixmap, this);

  for (const auto& connector : mConfig->connectors)
    mConnectors.append(std::make_shared<Connector>(connector.getPosition(boundingRect()), Config::CONNECTOR_RADIUS, this));

  // Create connection points as ellipses
  // mConnectors.append(std::make_shared<Connector>(mRightPoint, Config::CONNECTOR_RADIUS, this));

  setPos(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
}

QString NodeItem::Id() const
{
  return mId;
}

int NodeItem::type() const
{
  return Type;
}

QRectF NodeItem::boundingRect() const
{
  return mPixmapItem->boundingRect();
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  painter->drawPixmap(0, 0, mPixmapItem->pixmap());
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
  updateConnectors();
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  setPos(snapToGrid(scenePos(), Config::GRID_SIZE));
  updateConnectors();
  QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::updateConnectors()
{
  for (auto& connector : mConnectors)
    connector->updateConnections();
}
