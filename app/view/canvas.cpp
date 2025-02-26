#include "canvas.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include "logging.h"

Canvas::Canvas(QObject* parent)
    : QGraphicsScene(parent)
{
  // setAcceptDrops(true);
  setProperty("class", QVariant(QStringLiteral("canvas")));

  setSceneRect(0, 0, 800, 600);

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
        m_connection = new QGraphicsLineItem();
        m_connection->setLine(QLineF(node->leftConnectionPoint(), event->scenePos()));
        m_connection->setPen(QPen(Qt::white, 2));
        addItem(m_connection);

        return;  // Return early to avoid blocking default move behavior
      }
      else if (node->rightConnectionArea().contains(event->scenePos()))
      {
        m_startNode = node;
        m_connection = new QGraphicsLineItem();
        m_connection->setLine(QLineF(node->rightConnectionPoint(), event->scenePos()));
        m_connection->setPen(QPen(Qt::white, 2));
        addItem(m_connection);

        return;  // Return early to avoid blocking default move behavior
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
    QPointF endPos = event->scenePos();
    QPointF startPos = m_connection->line().p1();

    m_connection->setLine(QLineF(startPos, endPos));
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
      qInfo() << item->type();

      if (item && qgraphicsitem_cast<NodeItem*>(item) && item != m_startNode)
      {
        NodeItem* node = static_cast<NodeItem*>(item);

        // Check if we are inside another connection point
        if (node->leftConnectionArea().contains(event->scenePos()))
        {
          ConnectionItem* connection = new ConnectionItem(m_startNode, node);
          addItem(connection);
        }
        else if (node->rightConnectionArea().contains(event->scenePos()))
        {
          ConnectionItem* connection = new ConnectionItem(m_startNode, node);
          addItem(connection);
        }
      }

      // Always delete the connection once mouse is released
      removeItem(m_connection);
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
{
  setFlags(ItemIsMovable | ItemIsSelectable);
  setCacheMode(DeviceCoordinateCache);
  setAcceptHoverEvents(true);
  setZValue(1);
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

ConnectionItem::ConnectionItem(NodeItem* startNode, NodeItem* endNode)
    : QGraphicsLineItem()
    , m_startNode(startNode)
    , m_endNode(endNode)
{
  setPen(QPen(Qt::white, 2));  // Set line color and width
  updateLine();
}

void ConnectionItem::updateLine()
{
  // Update the line to connect the two nodes
  QPointF startPos = m_startNode->rightConnectionPoint();     // Right side of the start node
  QPointF endPos = m_endNode->leftConnectionPoint();         // Left side of the end node
  setLine(QLineF(startPos, endPos));  // Set the line between the two points
}
