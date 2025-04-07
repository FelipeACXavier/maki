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

QList<NodeItem*> Canvas::availableNodes()
{
  QList<NodeItem*> nodes;
  for (auto& item : items())
  {
    if (item->type() == NodeItem::Type)
      nodes.push_back(static_cast<NodeItem*>(item));
  }

  return nodes;
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
    clearSelectedNodes();

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

bool Canvas::isModifierSet(QGraphicsSceneMouseEvent* event, Qt::KeyboardModifier modifier)
{
  return (event->modifiers() & modifier) > 0;
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // If the press is on the left or right connection point of a node, start
  // drawing
  if (event->button() == Qt::LeftButton)
  {
    mStartDragPosition = event->scenePos();

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
      parentView()->setDragMode(QGraphicsView::NoDrag);
      return;
    }
    else if (item && item->type() == NodeItem::Type)
    {
      NodeItem* node = static_cast<NodeItem*>(item);
      if (isModifierSet(event, Qt::ControlModifier))
      {
        selectNode(node, !node->isSelected());
        return;
      }
      else
      {
        // We cannot clear if there are multiple nodes selected
        if (selectedItems().size() < 2)
          clearSelectedNodes();

        selectNode(node, true);
      }
    }
    else if (!item)
    {
      clearSelectedNodes();
    }
  }
  else if (event->button() == Qt::MiddleButton)
  {
    parentView()->setDragMode(QGraphicsView::NoDrag);
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
  parentView()->setDragMode(QGraphicsView::RubberBandDrag);

  if (mConnection)
  {
    if (event->button() == Qt::LeftButton)
    {
      QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
      if (item && item->type() == Connector::Type)
      {
        Connector* connector = static_cast<Connector*>(item);

        if (connector->connectorType() != Types::ConnectorType::OUT && connector != mConnector)
        {
          mConnection->setEnd(connector->id(), connector->center(), connector->shift().first);
          connector->addConnection(mConnection);
          mConnector->addConnection(mConnection);

          mConnection->done(mConnector, connector);
        }
        else
        {
          LOG_WARNING("To create the connection, drop the connection on a valid connector");
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
  else
  {
    if (parentView()->dragMode() == QGraphicsView::RubberBandDrag && !isModifierSet(event, Qt::ControlModifier))
    {
      bool draggingFromRight = mStartDragPosition.x() > event->scenePos().x();

      QPainterPath selectionPath = selectionArea();
      QList<QGraphicsItem*> allSelectedItems = selectedItems();

      for (QGraphicsItem* item : selectedItems())
      {
        QRectF itemBounds = item->sceneBoundingRect();
        if ((draggingFromRight && !selectionPath.intersects(itemBounds)) ||
            (!draggingFromRight && !selectionPath.contains(itemBounds)))
        {
          item->setSelected(false);
        }
      }
    }
  }

  QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
}

QMenu* Canvas::createConnectionMenu(const QList<QGraphicsItem*>& items)
{
  QMenu* connectMenu = new QMenu("Connect");
  connectMenu->setEnabled(false);

  if (items.size() != 2)
    return connectMenu;

  auto item1 = selectedItems().at(0);
  auto item2 = selectedItems().at(1);

  if (item1->type() != NodeItem::Type || item2->type() != NodeItem::Type)
    return connectMenu;

  auto node1 = static_cast<NodeItem*>(item1);
  auto node2 = static_cast<NodeItem*>(item2);

  for (auto& con1 : node1->connectors())
  {
    for (auto& con2 : node2->connectors())
    {
      Connector* start = nullptr;
      Connector* end = nullptr;

      if (con1->connectorType() == Types::ConnectorType::OUT &&
          (con2->connectorType() == Types::ConnectorType::IN || con2->connectorType() == Types::ConnectorType::IN_AND_OUT))
      {
        start = static_cast<Connector*>(con1.get());
        end = static_cast<Connector*>(con2.get());
      }
      else if (con1->connectorType() == Types::ConnectorType::IN &&
               (con2->connectorType() == Types::ConnectorType::OUT || con2->connectorType() == Types::ConnectorType::IN_AND_OUT))
      {
        start = static_cast<Connector*>(con2.get());
        end = static_cast<Connector*>(con1.get());
      }
      else
      {
        continue;
      }

      QAction* connectAction = connectMenu->addAction(QString("%1 -> %2").arg(start->connectorName(), end->connectorName()));
      QObject::connect(connectAction, &QAction::triggered, [this, start, end]() {
        assert(start && end);
        auto connection = new ConnectionItem();

        LOG_DEBUG("Connecting %s to %s", qPrintable(start->connectorName()), qPrintable(end->connectorName()));

        connection->setStart(start->id(), start->center(), start->shift().first);
        connection->setEnd(end->id(), end->center(), end->shift().first);

        start->addConnection(connection);
        end->addConnection(connection);

        connection->done(start, end);

        addItem(connection);
      });
    }
  }

  connectMenu->setEnabled(connectMenu->actions().size() > 0);

  return connectMenu;
}

QMenu* Canvas::createAlignMenu(const QList<QGraphicsItem*>& items)
{
  QMenu* alignMenu = new QMenu("Align");

  QAction* alignHCenter = alignMenu->addAction("Align H center");
  QAction* alignLeft = alignMenu->addAction("Align left");
  QAction* alignRight = alignMenu->addAction("Align right");
  QAction* alignVCenter = alignMenu->addAction("Align V center");
  QAction* alignTop = alignMenu->addAction("Align top");
  QAction* alignBottom = alignMenu->addAction("Align bottom");

  alignMenu->setEnabled(items.size() > 1);

  return alignMenu;
}

void Canvas::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  QList<QGraphicsItem*> items = selectedItems();

  QMenu menu;

  // Define menu actions
  // =============================================
  menu.addSection("Creation");

  menu.addMenu(createConnectionMenu(items));

  // =============================================
  menu.addSection("Edit");

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

  QAction* deleteAction = menu.addAction("Delete");
  deleteAction->setEnabled(items.size() > 0);
  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    deleteSelectedItems();
  });

  // =============================================
  menu.addSection("Visual");

  QAction* forwardAction = menu.addAction("To front");
  forwardAction->setEnabled(items.size() > 0);
  QObject::connect(forwardAction, &QAction::triggered, [this]() {
    for (QGraphicsItem* item : selectedItems())
      item->setZValue(mFrontZValue);

    ++mFrontZValue;
  });

  QAction* backwardAction = menu.addAction("To back");
  backwardAction->setEnabled(items.size() > 0);
  QObject::connect(backwardAction, &QAction::triggered, [this]() {
    for (QGraphicsItem* item : selectedItems())
      item->setZValue(mBackZValue);

    --mBackZValue;
  });

  QAction* toggleLabelAction = menu.addAction("Toggle label");
  toggleLabelAction->setEnabled(items.size() > 0);
  QObject::connect(toggleLabelAction, &QAction::triggered, [this]() {
    for (QGraphicsItem* item : selectedItems())
    {
      if (item->type() == NodeItem::Type)
        dynamic_cast<NodeItem*>(item)->toggleLableVisibility();
    }
  });

  menu.addMenu(createAlignMenu(items));

  // Execute the menu at the mouse cursor's position
  menu.exec(event->screenPos());
}

void Canvas::deleteSelectedItems()
{
  QList<QGraphicsItem*> items = selectedItems();
  QList<NodeItem*> nodesToDelete;
  QList<QGraphicsItem*> connectionsToDelete;

  for (QGraphicsItem* item : items)
  {
    if (!item)
      continue;

    if (item->type() == NodeItem::Type)
    {
      NodeItem* node = static_cast<NodeItem*>(item);
      NodeItem* parent = static_cast<NodeItem*>(node->parentNode());

      // Only delete if no parent OR parent is not selected
      if (!parent || !parent->isSelected())
        nodesToDelete.append(node);
    }
    else if (item->type() == ConnectionItem::Type)
    {
      connectionsToDelete.append(item);
    }
  }

  // First delete nodes
  for (NodeItem* node : nodesToDelete)
    node->deleteNode();

  // Then delete connections
  for (QGraphicsItem* item : connectionsToDelete)
  {
    removeItem(item);
    delete item;
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
    info.id = QString();
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
    if (!item)
      continue;

    if (item->type() != NodeItem::Type)
      continue;

    NodeItem* node = static_cast<NodeItem*>(item);
    if (node->parentNode())
      continue;

    node->deleteNode();
  }
}

void Canvas::selectNode(NodeItem* node, bool select)
{
  if (node)
    node->setSelected(select);

  emit nodeSelected(node, select);
}

void Canvas::clearSelectedNodes()
{
  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() == NodeItem::Type)
    {
      auto node = static_cast<NodeItem*>(item);
      selectNode(node, false);
    }
  }

  clearSelection();

  selectNode(nullptr, false);
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
