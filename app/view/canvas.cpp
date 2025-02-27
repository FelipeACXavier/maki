#include "canvas.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QTimer>
#include <QUuid>

#include "logging.h"

static const QString TMP_CONNECTION_ID = QStringLiteral("tmp_id");

QString printPoint(const QPointF& point)
{
  return QStringLiteral("(%1, %2)").arg(point.x(), point.y());
}

Canvas::Canvas(QObject* parent)
    : QGraphicsScene(parent)
{
  // setAcceptDrops(true);
  setProperty("class", QVariant(QStringLiteral("canvas")));
  setBackgroundBrush(QBrush(QColor("#212121")));
}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat("application/x-node"))
    event->acceptProposedAction();
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat("application/x-node"))
    event->acceptProposedAction();
}

void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat("application/x-node"))
  {
    // Create and add a new node at the drop position
    NodeItem* node = new NodeItem();
    node->setPos(event->scenePos() - node->topCorner());
    addItem(node);

    event->acceptProposedAction();
  }
}

// Handle connecting two nodes
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

  // If the press is on the left or right connection point of a node, start drawing
  if (item && qgraphicsitem_cast<NodeItem*>(item))
  {
    NodeItem* node = static_cast<NodeItem*>(item);
    if (event->button() == Qt::LeftButton)
    {
      // Check if the user clicked near the left or right connection point
      if (node->leftConnectionArea().contains(event->scenePos()))
      {
        m_startNode = node;
        m_connection = node->startConnection(node->leftConnectionPoint(), event->scenePos());
        addItem(m_connection.get());

        return;  // Return early to avoid blocking default move behavior
      }
      else if (node->rightConnectionArea().contains(event->scenePos()))
      {
        m_startNode = node;
        m_connection = node->startConnection(node->rightConnectionPoint(), event->scenePos());
        addItem(m_connection.get());

        return;
      }
    }
  }
  // Otherwise, let the item move as usual
  QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (m_connection)
  {
    m_connection->move(TMP_CONNECTION_ID, event->scenePos());
  }
  else
  {
    QGraphicsScene::mouseMoveEvent(event);
  }
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (m_connection)
  {
    if (event->button() == Qt::LeftButton)
    {
      QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
      if (item && qgraphicsitem_cast<NodeItem*>(item) && item != m_startNode)
      {
        NodeItem* node = static_cast<NodeItem*>(item);

        // Check if we are inside another connection point
        if (node->leftConnectionArea().contains(event->scenePos()))
        {
          m_connection->setEnd(node->Id(), node->leftConnectionPoint());
          m_startNode->endConnection(m_connection);
          node->addConnection(m_connection);
        }
        else if (node->rightConnectionArea().contains(event->scenePos()))
        {
          m_connection->setEnd(node->Id(), node->rightConnectionPoint());
          m_startNode->endConnection(m_connection);
          node->addConnection(m_connection);
        }
      }

      m_connection = nullptr;
    }
  }
  else
  {
    QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
  }
}

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
  connection->setEnd(TMP_CONNECTION_ID, endPoint);
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

ConnectionItem::ConnectionItem()
    : QGraphicsLineItem()
{
  setPen(QPen(Qt::white, 2));  // Set line color and width
}

void ConnectionItem::setStart(QString id, QPointF point)
{
  mSrcId = id;
  mSrcPoint = point;
}

void ConnectionItem::setEnd(QString id, QPointF point)
{
  mDstId = id;
  mDstPoint = point;
}

void ConnectionItem::move(QString id, QPointF pos)
{
  if (id == mSrcId)
  {
    mSrcPoint = pos;
    // LOG_INFO("Moving start to: (%f %f), (%f, %f)", mStartPoint.x(), mStartPoint.y(), mEndPoint.x(), mEndPoint.y());
    setLine(QLineF(mSrcPoint, mDstPoint));
  }
  else if (id == mDstId)
  {
    mDstPoint = pos;
    // LOG_INFO("Moving end to: (%f %f) (%f, %f)", mStartPoint.x(), mStartPoint.y(), mEndPoint.x(), mEndPoint.y());
    setLine(QLineF(mSrcPoint, mDstPoint));
  }
}
