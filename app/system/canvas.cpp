#include "canvas.h"

#include <QBuffer>
#include <QClipboard>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>

#include "app_configs.h"
#include "canvas_view.h"
#include "config.h"
#include "config_table.h"
#include "elements/connection.h"
#include "elements/connector.h"
#include "elements/node.h"
#include "logging.h"

Canvas::Canvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : QGraphicsScene(parent)
    , mConfigTable(configTable)
{
  setProperty("class", QVariant(QStringLiteral("canvas")));
  setBackgroundBrush(Qt::transparent);
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
    NodeItem* parentNode = nullptr;
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->type() == NodeItem::Type)
    {
      parentNode = static_cast<NodeItem*>(item);

      // Add error message
      if (!parentNode->acceptDrops())
        return;
    }

    QByteArray data = event->mimeData()->data(Constants::TYPE_NODE);
    QDataStream stream(&data, QIODevice::ReadOnly);

    NodeSaveInfo info;
    stream >> info;

    if (createNode(info, event->scenePos(), parentNode))
      event->acceptProposedAction();
    else
      event->ignore();

    // Make sure we show that we are no longer dragging
    dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::ArrowCursor);
  }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // If the press is on the left or right connection point of a node, start
  // drawing
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
      mConnection = new ConnectionItem();

      auto shifts = connector->shift();
      mConnection->setStart(connector->id(), connector->center(), shifts.first);
      mConnection->setEnd(Constants::TMP_CONNECTION_ID, event->scenePos(), shifts.second);

      addItem(mConnection);
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
      mHoveredConnector->updateColor(true);
    }
    else if (mHoveredConnector)
    {
      mHoveredConnector->updateColor(false);
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
          mConnection->setEnd(connector->id(), connector->center(), connector->shift().first);
          connector->addConnection(mConnection);
          mConnector->addConnection(mConnection);

          mConnection->done(mConnector, connector);
        }
      }
      else
      {
        removeItem(mConnection);
      }

      mConnection = nullptr;
      mConnector = nullptr;
    }
  }

  QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Delete)
  {
    for (QGraphicsItem* item : selectedItems())
    {
      if (item->type() == NodeItem::Type)
        dynamic_cast<NodeItem*>(item)->onDelete();
      else
        delete item;
    }
  }
  else
  {
    QGraphicsScene::keyPressEvent(event);  // Forward event to base class
  }
}

void Canvas::copySelectedItems()
{
  copiedNodes.clear();

  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() != NodeItem::Type)
      continue;

    NodeItem* node = dynamic_cast<NodeItem*>(item);
    if (!node)
      continue;

    copiedNodes.append(node->saveInfo());

    // Make sure the item is not selected after copying
    item->setSelected(false);
  }
}

void Canvas::pasteCopiedItems()
{
  if (copiedNodes.isEmpty())
    return;

  QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  for (NodeSaveInfo& info : copiedNodes)
  {
    NodeItem* parentNode = nullptr;
    QGraphicsItem* item = itemAt(mousePosition, QTransform());
    if (item && item->type() == NodeItem::Type)
    {
      parentNode = static_cast<NodeItem*>(item);

      // Add error message
      if (!parentNode->acceptDrops())
        return;
    }

    auto node = createNode(info, mousePosition, parentNode);

    for (const auto& child : info.children)
    {
      auto childNode = dynamic_cast<NodeItem*>(child);
      if (!childNode)
        continue;

      // Children's positions take into account the relative prosition to the parent
      auto childInfo = childNode->saveInfo();
      (void)createNode(childInfo, mousePosition - (info.position - childInfo.position), node);
    }
  }
}

VoidResult Canvas::loadFromSave(const SaveInfo& info)
{
  // Clear the canvas before repopulating
  clear();

  for (const auto& node : info.structuralNodes)
  {
    LOG_DEBUG("Creating structural node %s with parent %s", qPrintable(node.id), qPrintable(node.parentId));
    (void)createNode(node, node.position, findNodeWithId(node.parentId));
  }

  for (const auto& node : info.behaviouralNodes)
  {
    LOG_DEBUG("Creating behavioral node %s with parent %s", qPrintable(node.id), qPrintable(node.parentId));
    (void)createNode(node, node.position, findNodeWithId(node.parentId));
  }

  for (const auto& conn : info.connections)
  {
    LOG_DEBUG("Creating connection with parents %s -> %s", qPrintable(conn.srcId), qPrintable(conn.dstId));

    auto srcConn = findConnectorWithId(conn.srcId);
    auto dstConn = findConnectorWithId(conn.dstId);

    if (!srcConn || !dstConn)
    {
      LOG_WARNING("Could not find connectors");
      continue;
    }

    auto connection = new ConnectionItem();

    connection->setStart(conn.srcId, conn.srcPoint, conn.srcShift);
    connection->setEnd(conn.dstId, conn.dstPoint, conn.dstShift);

    srcConn->addConnection(connection);
    dstConn->addConnection(connection);

    connection->done(srcConn, dstConn);

    addItem(connection);
  }

  return VoidResult();
}

CanvasView* Canvas::parentView() const
{
  return static_cast<CanvasView*>(parent());
}

NodeItem* Canvas::createNode(const NodeSaveInfo& info, const QPointF& position, NodeItem* parent)
{
  auto config = mConfigTable->get(info.nodeId);
  if (config == nullptr)
  {
    LOG_WARNING("Added node with no configuration");
    return nullptr;
  }

  if (config->libraryType != Types::LibraryTypes::STRUCTURAL && parent == nullptr)
  {
    LOG_WARNING("Node %d must be inside a structural element", (int)config->libraryType);
    return nullptr;
  }

  // On drop, there is no info.id yet so we must assign a unique id to this node.
  // When loading from a file, we need to retain the id
  QString id = QUuid::createUuid().toString();
  if (!info.id.isEmpty() && !info.id.isNull())
    id = info.id;

  NodeItem* node = new NodeItem(id, info, position, config, parentView()->getScale());

  node->nodeSeletected = [this](NodeItem* item) { emit nodeSelected(item); };
  node->nodeCopied = [this](NodeItem* /* item */) { copySelectedItems(); };
  node->nodeDeleted = [this](NodeItem* item) {
    removeItem(item);
    emit nodeRemoved(item);
  };

  node->start();

  // When loading from save, we need to enforce the position
  node->setPos(position);

  if (parent != nullptr)
  {
    node->setParent(parent);
    parent->addChild(node);
  }

  addItem(node);

  emit nodeAdded(node);

  return node;
}

NodeItem* Canvas::findNodeWithId(const QString& id) const
{
  for (const auto& item : items())
  {
    if (item->type() != NodeItem::Type)
      continue;

    auto node = static_cast<NodeItem*>(item);
    if (node->id() == id)
      return node;
  }

  return nullptr;
}

Connector* Canvas::findConnectorWithId(const QString& id) const
{
  for (const auto& item : items())
  {
    if (item->type() != Connector::Type)
      continue;

    auto connector = static_cast<Connector*>(item);
    if (connector->id() == id)
      return connector;
  }

  return nullptr;
}

qreal Canvas::getScale() const
{
  return parentView()->getScale();
}
