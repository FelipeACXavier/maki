#include "canvas.h"

#include <QBuffer>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include "app_configs.h"
#include "config_table.h"
#include "connection.h"
#include "elements/config.h"
#include "logging.h"
#include "node.h"

Canvas::Canvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : QGraphicsScene(parent)
    , mConfigTable(configTable)
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
  if (event->mimeData()->hasFormat(Constants::TYPE_NODE))
    event->acceptProposedAction();
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat(Constants::TYPE_NODE))
    event->acceptProposedAction();
}

void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat(Constants::TYPE_NODE))
  {
    QPixmap pixmap;
    QByteArray pixmapData = event->mimeData()->data(Constants::TYPE_PIXMAP);
    pixmap.loadFromData(pixmapData, "PNG");

    QByteArray idData = event->mimeData()->data(Constants::TYPE_NODE_ID);
    QString nodeId = QString(idData);
    auto data = mConfigTable->get(nodeId);
    if (data == nullptr)
    {
      LOG_ERROR("Added node with no configuration");
      return;
    }

    addItem(new NodeItem(event->scenePos(), pixmap, data));
    event->acceptProposedAction();

    // Make sure we show that we are no longer dragging
    dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::ArrowCursor);
  }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // If the press is on the left or right connection point of a node, start drawing
  if (event->button() == Qt::LeftButton)
  {
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->type() == Connector::Type)
    {
      Connector* connector = static_cast<Connector*>(item);

      // Add error message
      if (connector->connectorType() == Types::ConnectorType::IN)
        return;

      mConnector = connector;
      mConnection = std::make_shared<ConnectionItem>();

      auto shifts = connector->shift();
      mConnection->setStart(connector->Id(), connector->center(), shifts.first);
      mConnection->setEnd(Constants::TMP_CONNECTION_ID, event->scenePos(), shifts.second);

      // connector->startConnection(event->scenePos());
      addItem(mConnection.get());
      return;
    }
  }

  QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mConnection)
  {
    // Find the nearest Connector while dragging
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->type() == Connector::Type)
    {
      mHoveredConnector = dynamic_cast<Connector*>(item);
      mHoveredConnector->setBrush(Qt::green);
    }
    else if (mHoveredConnector)
    {
      mHoveredConnector->setBrush(Qt::blue);
      mHoveredConnector = nullptr;
    }

    mConnection->move(Constants::TMP_CONNECTION_ID, event->scenePos());
  }

  QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mConnection)
  {
    if (event->button() == Qt::LeftButton)
    {
      QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
      if (item && item->type() == Connector::Type)
      {
        Connector* connector = static_cast<Connector*>(item);
        // Add error message
        if (connector->connectorType() == Types::ConnectorType::OUT)
          return;

        if (connector != mConnector)
        {
          mConnection->setEnd(connector->Id(), connector->center(), connector->shift().first);
          connector->addConnection(mConnection);
          mConnector->addConnection(mConnection);
          mConnection->done();
        }
      }

      mConnection = nullptr;
      mConnector = nullptr;
    }
  }

  QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
}
