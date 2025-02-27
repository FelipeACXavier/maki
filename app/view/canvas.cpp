#include "canvas.h"

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include "app_configs.h"
#include "connection.h"
#include "node.h"

Canvas::Canvas(QObject* parent)
    : QGraphicsScene(parent)
{
  // setAcceptDrops(true);
  setProperty("class", QVariant(QStringLiteral("canvas")));
  setBackgroundBrush(QBrush(QColor("#212121")));
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
  const int gridSize = 20;  // Grid spacing

  QPen lightPen(Qt::gray, 0.5, Qt::DotLine);
  QPen darkPen(Qt::darkGray, 1, Qt::SolidLine);

  painter->setPen(lightPen);

  // Draw vertical grid lines
  for (qreal x = std::floor(rect.left() / gridSize) * gridSize; x < rect.right(); x += gridSize)
  {
    painter->drawLine(QLineF(x, rect.top(), x, rect.bottom()));
  }

  // Draw horizontal grid lines
  for (qreal y = std::floor(rect.top() / gridSize) * gridSize; y < rect.bottom(); y += gridSize)
  {
    painter->drawLine(QLineF(rect.left(), y, rect.right(), y));
  }

  // Draw thicker lines every 5 grid spaces (major grid)
  painter->setPen(darkPen);
  for (qreal x = std::floor(rect.left() / (gridSize * 5)) * (gridSize * 5); x < rect.right(); x += gridSize * 5)
  {
    painter->drawLine(QLineF(x, rect.top(), x, rect.bottom()));
  }
  for (qreal y = std::floor(rect.top() / (gridSize * 5)) * (gridSize * 5); y < rect.bottom(); y += gridSize * 5)
  {
    painter->drawLine(QLineF(rect.left(), y, rect.right(), y));
  }
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
    addItem(new NodeItem(event->scenePos()));
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
    if (event->button() == Qt::LeftButton)
    {
      QPointF startPoint;
      NodeItem* node = static_cast<NodeItem*>(item);

      // Check if the user clicked near the left or right connection point
      if (node->leftConnectionArea().contains(event->scenePos()))
        startPoint = node->leftConnectionPoint();
      else if (node->rightConnectionArea().contains(event->scenePos()))
        startPoint = node->rightConnectionPoint();

      if (!startPoint.isNull())
      {
        m_startNode = node;
        m_connection = node->startConnection(startPoint, event->scenePos());
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
    m_connection->move(Savant::Constants::TMP_CONNECTION_ID, event->scenePos());
  else
    QGraphicsScene::mouseMoveEvent(event);
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
        QPointF endPoint;

        // Check if we are inside another connection point
        if (node->leftConnectionArea().contains(event->scenePos()))
          endPoint = node->leftConnectionPoint();
        else if (node->rightConnectionArea().contains(event->scenePos()))
          endPoint = node->rightConnectionPoint();

        if (!endPoint.isNull())
        {
          m_connection->setEnd(node->Id(), endPoint);
          m_startNode->endConnection(m_connection);
          node->addConnection(m_connection);
        }
      }

      m_connection = nullptr;
      m_startNode = nullptr;
    }
  }
  else
  {
    QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
  }
}
