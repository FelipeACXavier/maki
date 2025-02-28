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

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& map, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(QUuid::createUuid().toString())
    , mPixmap(map)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  // Create connection points as ellipses
  mConnectors.append(std::make_shared<Connector>(mLeftPoint, mRadius, this));
  mConnectors.append(std::make_shared<Connector>(mRightPoint, mRadius, this));

  mPixmapItem = std::make_shared<QGraphicsPixmapItem>(mPixmap, this);

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
  // The rectangle itself is limited by its left point and width, but because of the connection points we
  // shift to the left and, therefore, need to double the radius for the "shift" to the right
  //
  // mLeft                 | ------------ |       mWidth
  // mLeft - mRadius     | ------------ |         mWidth
  // mLeft - mRadius     | -------------- |       mWidth + mRadius
  // mLeft - mRadius     | ---------------- |     mWidth + 2 * mRadius
  // return QRectF(mLeft - mRadius, mTop, mWidth + 2 * mRadius, mHeight);
  return mPixmapItem->boundingRect();
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  painter->drawPixmap(0, 0, mPixmap);
}

QPointF NodeItem::leftConnectionPoint() const
{
  return scenePos() + mLeftPoint;
}

QPointF NodeItem::rightConnectionPoint() const
{
  return scenePos() + mRightPoint;
}

QRectF NodeItem::leftConnectionArea() const
{
  return QRectF(scenePos() + mLeftPoint - QPointF(mRadius, mRadius), pos() + mLeftPoint + QPointF(mRadius, mRadius));
}

QRectF NodeItem::rightConnectionArea() const
{
  return QRectF(scenePos() + mRightPoint - QPointF(mRadius, mRadius), pos() + mRightPoint + QPointF(mRadius, mRadius));
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
