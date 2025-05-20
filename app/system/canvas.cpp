#include "canvas.h"

#include <QBuffer>
#include <QClipboard>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>

#include "app_configs.h"
#include "canvas_view.h"
#include "config.h"
#include "config_table.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "elements/transition.h"
#include "logging.h"

Canvas::Canvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : QGraphicsScene(parent)
    , mConfigTable(configTable)
    , mStorage(storage)
    , mId(canvasId)
{
  setProperty("class", QVariant(QStringLiteral("canvas")));
  setBackgroundBrush(Qt::transparent);

  mHoverTimer = new QTimer(this);
  mHoverTimer->setSingleShot(true);
}

QString Canvas::id() const
{
  return mId;
}

Types::LibraryTypes Canvas::type() const
{
  return Types::LibraryTypes::UNKNOWN;
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

    auto node = createNode(NodeCreation::Dropping, info, event->scenePos(), parentNode);
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
    if (item && (item->type() == NodeItem::Type || item->type() == QGraphicsTextItem::Type))
    {
      NodeItem* node = static_cast<NodeItem*>(item->type() == QGraphicsTextItem::Type ? item->parentItem() : item);
      if (isModifierSet(event, Qt::AltModifier))
      {
        mNode = node;
        mTransition = new TransitionItem(std::make_shared<TransitionSaveInfo>());

        mTransition->setStart(node->id(), node->mapToScene(node->boundingRect().center()), {0, 0});
        mTransition->setEnd(Constants::TMP_CONNECTION_ID, event->scenePos(), {0, 0});

        addItem(mTransition);
        parentView()->setDragMode(QGraphicsView::NoDrag);
        event->accept();
        return;
      }
      else if (isModifierSet(event, Qt::ControlModifier))
      {
        selectNode(node, !node->isSelected());
        event->accept();
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
  if (mTransition)
  {
    mTransition->move(Constants::TMP_CONNECTION_ID, event->scenePos());
  }
  // else if (event->buttons() & Qt::LeftButton)
  // {
  //   QList<QGraphicsItem*> draggedItems = selectedItems();

  //   if (draggedItems.size() > 0)
  //   {
  //     // Check for collision or proximity with other nodes
  //     QList<QGraphicsItem*> itemsUnder = items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);

  //     if (itemsUnder.size() > 1)
  //     {
  //       for (QGraphicsItem* target : itemsUnder)
  //       {
  //         if (draggedItems.contains(target) || target->type() != NodeItem::Type)
  //           continue;

  //         NodeItem* targetNode = static_cast<NodeItem*>(target);

  //         if (mHoveredNode != targetNode)
  //         {
  //           mHoveredNode = targetNode;
  //           mHoverTimer->stop();
  //           mHoverTimer->disconnect();
  //           connect(mHoverTimer, &QTimer::timeout, this, [=]() {
  //             if (!mHoveredNode)
  //               return;

  //             if (!(QApplication::mouseButtons() & Qt::LeftButton))
  //               return;

  //             bool shouldAsk = false;
  //             for (const auto& item : draggedItems)
  //             {
  //               if (item->type() != NodeItem::Type)
  //                 continue;

  //               auto node = static_cast<NodeItem*>(item);

  //               if (node->parentNode() != mHoveredNode)
  //               {
  //                 shouldAsk = true;
  //                 break;
  //               }
  //             }

  //             if (!shouldAsk)
  //               return;

  //             if (QMessageBox::question(nullptr, "Make Child", "Make this node a child of the hovered node?") == QMessageBox::Yes)
  //             {
  //               for (const auto& item : draggedItems)
  //               {
  //                 if (item->type() != NodeItem::Type)
  //                   continue;

  //                 auto node = static_cast<NodeItem*>(item);
  //                 if (node->parentNode())
  //                   continue;

  //                 // If item is a NodeItem and it has no parent, make this node the parent
  //                 node->setParent(mHoveredNode);
  //                 mHoveredNode->addChild(node);
  //               }
  //             }
  //           });
  //           mHoverTimer->start(1000);
  //         }

  //         break;
  //       }
  //     }
  //     else if (mHoveredNode)
  //     {
  //       mHoverTimer->stop();
  //       mHoveredNode = nullptr;
  //     }
  //   }
  // }

  QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  parentView()->setDragMode(QGraphicsView::RubberBandDrag);

  if (mTransition)
  {
    if (event->button() == Qt::LeftButton)
    {
      QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
      if (item && (item->type() == NodeItem::Type || item->type() == QGraphicsTextItem::Type))
      {
        NodeItem* node = static_cast<NodeItem*>(item->type() == QGraphicsTextItem::Type ? item->parentItem() : item);

        mTransition->setEnd(node->id(), node->mapToScene(node->boundingRect().center()), {0, 0});
        mTransition->done(mNode, node);
      }
      else
      {
        removeItem(mTransition);
      }

      mTransition = nullptr;
      mNode = nullptr;
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

  QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
  NodeItem* node = nullptr;
  if (item && item->type() == NodeItem::Type)
    node = static_cast<NodeItem*>(item);

  QAction* newFlowAction = menu.addAction(tr("New flow"));
  newFlowAction->setEnabled(node != nullptr && items.size() == 1);
  QObject::connect(newFlowAction, &QAction::triggered, [this, node]() {
    emit openFlow(nullptr, node);
  });
  menu.addAction(newFlowAction);

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
    else if (item->type() == TransitionItem::Type)
    {
      TransitionItem* transition = static_cast<TransitionItem*>(item);
      if (!(transition->source() && transition->source()->isSelected()) && !(transition->destination() && transition->destination()->isSelected()))
        connectionsToDelete.append(item);
    }
  }

  // First delete the connections
  for (QGraphicsItem* connection : connectionsToDelete)
  {
    removeItem(connection);
    delete connection;
  }

  // Then delete the nodes
  for (NodeItem* node : nodesToDelete)
  {
    updateParent(node, nullptr, false);
    node->deleteNode();
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

    // Do not copy the children
    if (node->parentNode() != nullptr && static_cast<NodeItem*>(node->parentNode())->isSelected())
      continue;

    // Save relative position
    auto info = node->saveInfo();

    // Must clean since the pasted nodes must have generated id
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

  for (NodeSaveInfo info : copiedNodes)
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

    auto node = createNode(NodeCreation::Pasting, info, mousePosition - info.position, parentNode);

    for (const auto& childInfo : info.children)
    {
      // Children's positions take into account the relative prosition to the parent
      auto child = createNode(NodeCreation::Pasting, *childInfo, childInfo->position, node);
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
    LOG_DEBUG("Creating structural node %s with parent %s", qPrintable(node->id), qPrintable(node->parentId));
    auto createdNode = createNode(NodeCreation::Pasting, *node, node->position, nullptr);

    for (const auto& childInfo : node->children)
    {
      // Children's positions take into account the relative prosition to the parent
      auto child = createNode(NodeCreation::Pasting, *childInfo, childInfo->position, createdNode);
      selectNode(child, false);
    }

    for (const auto& flow : node->flows)
    {
      Flow* createdFlow = createdNode->createFlow(flow->name, flow);
      emit flowAdded(createdFlow, createdNode);
    }

    selectNode(createdNode, false);
  }

  // for (const auto& node : info.behaviouralNodes)
  // {
  //   LOG_DEBUG("Creating behavioral node %s with parent %s", qPrintable(node.id), qPrintable(node.parentId));
  //   (void)createNode(node, node.position, findNodeWithId(node.parentId));
  // }

  // for (const auto& node : info.behaviouralNodes)
  // {
  //   auto srcConn = findNodeWithId(node.id);
  //   if (!srcConn)
  //     continue;

  //   for (const auto& transition : node.transitions)
  //   {
  //     auto dstConn = findNodeWithId(transition.dstId);
  //     if (!dstConn)
  //     {
  //       LOG_WARNING("Could not find destination node");
  //       continue;
  //     }

  //     LOG_DEBUG("Creating transitions with parents %s -> %s", qPrintable(node.id), qPrintable(transition.dstId));

  //     auto connection = new TransitionItem();

  //     connection->setStart(node.id, transition.srcPoint, transition.srcShift);
  //     connection->setEnd(transition.dstId, transition.dstPoint, transition.dstShift);

  //     srcConn->addTransition(connection);
  //     dstConn->addTransition(connection);

  //     connection->done(srcConn, dstConn);

  //     addItem(connection);
  //   }
  // }

  return VoidResult();
}

CanvasView* Canvas::parentView() const
{
  return static_cast<CanvasView*>(parent());
}

NodeItem* Canvas::createNode(NodeCreation creation, const NodeSaveInfo& info, const QPointF& position, NodeItem* parent)
{
  auto config = mConfigTable->get(info.nodeId);
  if (config == nullptr)
  {
    LOG_WARNING("Added node with no configuration");
    return nullptr;
  }

  if (config->libraryType != type())
  {
    LOG_WARNING("Node of type \"%s\" cannot be placed in a \"%s\" canvas", qPrintable(Types::LibraryTypeToString(config->libraryType)), qPrintable(Types::LibraryTypeToString(type())));
    return nullptr;
  }

  // If no parent is defined, we must create a "base node" in the canvas
  NodeItem* node = nullptr;
  auto nodeInfo = std::make_shared<NodeSaveInfo>(info);
  if (parent == nullptr)
  {
    if (type() == Types::LibraryTypes::STRUCTURAL)
    {
      mStorage->structuralNodes.append(nodeInfo);
      node = new NodeItem("", nodeInfo, position, config);
    }
    else
    {
      // In a behavioral flow, the info is stored in the flow itself
      // mStorage->behaviouralNodes.append(nodeInfo);
      node = new NodeItem("", nodeInfo, position, config);
    }
  }
  // If it is defined, we simply add a child node to the parent
  else
  {
    QPointF pos = creation == NodeCreation::Dropping ? parent->mapFromScene(position) : position;
    node = new NodeItem("", nodeInfo, pos, config, parent);

    parent->addChild(node, nodeInfo);
  }

  // TODO(felaze): Move these to a function or so
  node->nodeModified = [this](NodeItem* item) { emit nodeModified(item); };
  node->nodeDeleted = [this](NodeItem* item) {
    removeItem(item);
    emit nodeRemoved(item);
  };

  node->start();

  // TODO(felaze): When loading from save, we need to enforce the position
  if (creation == NodeCreation::Pasting)
    node->setPos(position);

  // Do not add child nodes to the scene
  if (parent == nullptr)
    addItem(node);

  if (creation != NodeCreation::Populating)
    updateParent(node, nodeInfo, true);

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

void Canvas::onSelectNode(const QList<QString>& nodeIds)
{
  clearSelectedNodes();

  for (const auto& nodeId : nodeIds)
  {
    auto node = findNodeWithId(nodeId);
    if (node)
      node->setSelected(true);

    emit nodeSelected(nodeIds.size() == 1 ? node : nullptr, nodeIds.size() == 1);
  }
}

void Canvas::onRenameNode(const QString& nodeId, const QString& name)
{
  auto node = findNodeWithId(nodeId);
  if (node)
    node->setProperty("name", name);
}

// ==========================================================================================
// Flow
void Canvas::populate(Flow* flow)
{
  // First create all the nodes
  LOG_DEBUG("Nodes before %d", flow->getNodes().size());
  for (const auto& node : flow->getNodes())
  {
    LOG_DEBUG("Creating behavioral node %s with parent %s", qPrintable(node->id), qPrintable(node->parentId));
    (void)createNode(NodeCreation::Populating, *node, node->position, findNodeWithId(node->parentId));
  }

  // Then create the transitions between the nodes
  for (const auto& node : flow->getNodes())
  {
    auto srcConn = findNodeWithId(node->id);
    if (!srcConn)
    {
      LOG_WARNING("Could not find source node");
      continue;
    }

    qDebug() << node->toJson();
    for (const auto& transition : node->transitions)
    {
      auto dstConn = findNodeWithId(transition->dstId);
      if (!dstConn)
      {
        LOG_WARNING("Could not find destination node");
        continue;
      }

      LOG_DEBUG("Creating transitions %s -> %s", qPrintable(node->id), qPrintable(transition->dstId));

      auto connection = new TransitionItem(transition);

      connection->setStart(node->id, transition->srcPoint, transition->srcShift);
      connection->setEnd(transition->dstId, transition->dstPoint, transition->dstShift);

      connection->done(srcConn, dstConn);

      addItem(connection);
    }
  }
}

void Canvas::onFlowSelected(const QString& flowId, const QString& nodeId)
{
  LOG_INFO("Selected flow %s of node %s", qPrintable(flowId), qPrintable(nodeId));
  auto node = findNodeWithId(nodeId);
  if (!node)
  {
    LOG_WARNING("Flow is not tied to any nodes");
    return;
  }

  auto flow = node->getFlow(flowId);
  emit openFlow(flow, node);
}

void Canvas::onFlowRemoved(const QString& flowId, const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
  {
    LOG_WARNING("Flow is not tied to any nodes");
    return;
  }

  node->deleteFlow(flowId);
  emit flowRemoved(flowId, node);
}

void Canvas::updateParent(NodeItem* /* node */, std::shared_ptr<NodeSaveInfo> /* storage */, bool /* adding */)
{
}
