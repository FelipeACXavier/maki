#include "canvas.h"

#include <QBuffer>
#include <QClipboard>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
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

    // Make sure that no other nodes are selected before dropping
    clearSelection();

    QByteArray data = event->mimeData()->data(Constants::TYPE_NODE);
    QDataStream stream(&data, QIODevice::ReadOnly);

    NodeSaveInfo info;
    stream >> info;
    info.scale = parentView()->getScale();

    auto node = createNode(info, event->scenePos(), parentNode);
    if (node)
    {
      selectNode(node, true);
      event->acceptProposedAction();
    }
    else
    {
      event->ignore();
    }

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
    else if (item && item->type() == NodeItem::Type)
    {
      NodeItem* node = static_cast<NodeItem*>(item);
      selectNode(node, true);
    }
    else if (!item)
    {
      selectNode(nullptr, false);
      // emit nodeSelected(nullptr);
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

void Canvas::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  QList<QGraphicsItem*> items = selectedItems();

  // If right-clicking directly on a node and that is the only node, let it handle the event
  // QGraphicsItem* clickedItem = itemAt(event->scenePos(), QTransform());
  // if (items.size() < 2 && clickedItem && clickedItem->type() == NodeItem::Type)
  // {
  //   QGraphicsScene::contextMenuEvent(event);
  //   return;
  // }

  // Create the menu
  QMenu menu;

  // Add actions to the menu
  QAction* copyAction = menu.addAction("Copy");
  copyAction->setEnabled(items.size() > 0);
  QObject::connect(copyAction, &QAction::triggered, [this]() {
    copySelectedItems();
  });

  QAction* pasteAction = menu.addAction("Paste");
  pasteAction->setEnabled(copiedNodes.size() > 0);
  QObject::connect(pasteAction, &QAction::triggered, [this]() {
    pasteCopiedItems();
  });
  menu.addSeparator();

  QAction* deleteAction = menu.addAction("Delete");
  deleteAction->setEnabled(items.size() > 0);
  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    deleteSelectedItems();
  });

  menu.addSeparator();

  QAction* toggleLabelAction = menu.addAction("Toggle label");
  toggleLabelAction->setEnabled(items.size() > 0);
  QObject::connect(toggleLabelAction, &QAction::triggered, [this]() {
    for (QGraphicsItem* item : selectedItems())
    {
      if (item->type() == NodeItem::Type)
        dynamic_cast<NodeItem*>(item)->toggleLableVisibility();
    }
  });

  QMenu* alignMenu = new QMenu("Align");
  QAction* alignHCenter = alignMenu->addAction("Align H center");
  QAction* alignLeft = alignMenu->addAction("Align left");
  QAction* alignRight = alignMenu->addAction("Align right");
  QAction* alignVCenter = alignMenu->addAction("Align V center");
  QAction* alignTop = alignMenu->addAction("Align top");
  QAction* alignBottom = alignMenu->addAction("Align bottom");
  alignMenu->setEnabled(items.size() > 1);

  menu.addMenu(alignMenu);

  // Execute the menu at the mouse cursor's position
  menu.exec(event->screenPos());
}

void Canvas::deleteSelectedItems()
{
  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() == NodeItem::Type)
      dynamic_cast<NodeItem*>(item)->deleteNode();
    else
      removeItem(item);
  }
}

void Canvas::copySelectedItems()
{
  copiedNodes.clear();

  QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() != NodeItem::Type)
      continue;

    NodeItem* node = dynamic_cast<NodeItem*>(item);
    if (!node)
      continue;

    // Save relative position
    auto info = node->saveInfo();
    info.position = mousePosition - info.position;

    copiedNodes.append(info);

    // Make sure the item is not selected after copying
    selectNode(node, false);
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

    auto node = createNode(info, mousePosition - info.position, parentNode);

    for (const auto& childInfo : info.children)
    {
      // Children's positions take into account the relative prosition to the parent
      auto child = createNode(childInfo, node->scenePos() - childInfo.position, node);
      selectNode(child, false);
    }

    selectNode(node, false);
  }
}

void Canvas::clearCanvas()
{
  QList<QGraphicsItem*> itemsList = items();
  for (QGraphicsItem* item : itemsList)
  {
    if (item->type() == NodeItem::Type)
      static_cast<NodeItem*>(item)->deleteNode();
    else
      removeItem(item);
  }
}

void Canvas::selectNode(NodeItem* node, bool select)
{
  if (node)
    node->setSelected(select);

  emit nodeSelected(select ? node : nullptr);
}

VoidResult Canvas::loadFromSave(const SaveInfo& info)
{
  // Clear the canvas before repopulating
  clearCanvas();

  // Reset canvas
  // TODO(felaze): This should be moved to the CanvasView, something like parentView()->loadFromSave(info.canvasInfo);
  parentView()->zoom(info.canvasInfo.scale / parentView()->getScale());
  parentView()->setScale(info.canvasInfo.scale);
  parentView()->centerOn(info.canvasInfo.center);

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

  NodeItem* node = new NodeItem(id, info, position, config);

  node->nodeModified = [this](NodeItem* item) { emit nodeModified(item); };
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

QPointF Canvas::getCenter() const
{
  return parentView()->getCenter();
}

void Canvas::onFocusNode(const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
    return;

  parentView()->zoom(2 * node->baseScale() / parentView()->getScale());

  // Center the node in the view
  parentView()->centerOn(node);
}

void Canvas::onRemoveNode(const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  if (node)
    node->deleteNode();
}

void Canvas::onSelectNode(const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  selectNode(node, true);
}

void Canvas::onRenameNode(const QString& nodeId, const QString& name)
{
  auto node = findNodeWithId(nodeId);
  if (node)
    node->setProperty("name", name);
}
