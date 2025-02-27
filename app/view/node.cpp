#include "node.h"

#include <QPainter>
#include <QUuid>

#include "app_configs.h"
#include "connection.h"
#include "style_helpers.h"

NodeItem::NodeItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(QUuid::createUuid().toString())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptHoverEvents(true);
  setZValue(1);
}

QString NodeItem::Id() const
{
  return mId;
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
  return QRectF(mLeft - mRadius, mTop, mWidth + 2 * mRadius, mHeight);
}

QPointF NodeItem::topCorner() const
{
  return QPointF(mWidth / 2, mHeight / 2);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  painter->setBrush(m_hovered ? Qt::red : Qt::lightGray);
  painter->drawRect(mLeft, mTop, mWidth, mHeight);

  // Left connection circle
  painter->setBrush(Qt::red);
  painter->drawEllipse(mLeftPoint, mRadius, mRadius);

  // Right connection circle
  painter->setBrush(Qt::green);
  painter->drawEllipse(mRightPoint, mRadius, mRadius);
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

void NodeItem::addConnection(std::shared_ptr<ConnectionItem> connection)
{
  mInConnections.push_back(connection);
}

std::shared_ptr<ConnectionItem> NodeItem::startConnection(QPointF startPoint, QPointF endPoint)
{
  auto connection = std::make_shared<ConnectionItem>();
  connection->setStart(Id(), startPoint);
  connection->setEnd(Savant::Constants::TMP_CONNECTION_ID, endPoint);
  return connection;
}

void NodeItem::endConnection(std::shared_ptr<ConnectionItem> connection)
{
  mOutConnections.push_back(connection);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);

  for (auto& conn : mOutConnections)
    conn->move(Id(), mRightPoint + scenePos());

  for (auto& conn : mInConnections)
    conn->move(Id(), mLeftPoint + scenePos());
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  setPos(snapToGrid(scenePos(), Savant::Config::GRID_SIZE));  // Snap to the grid

  QGraphicsItem::mouseReleaseEvent(event);
}
