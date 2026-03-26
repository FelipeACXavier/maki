#include "canvas.h"

#include <QBuffer>
#include <QClipboard>
#include <QGraphicsSceneDragDropEvent>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QUndoStack>
#include <limits>
#include <memory>

#include "app_configs.h"
#include "canvas_view.h"
#include "common/style_helpers.h"
#include "config.h"
#include "config_table.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "elements/transition.h"
#include "logging.h"
#include "result.h"
#include "save_info.h"
#include "undo_commands/add_node.h"
#include "undo_commands/align.h"
#include "undo_commands/remove_node.h"

Canvas::Canvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : QGraphicsScene(parent)
    , mId(canvasId)
    , mCopiedNodes({})
    , mConfigTable(configTable)
    , mStorage(storage)
{
  setBackgroundBrush(Qt::transparent);

  mHoverTimer = new QTimer(this);
  mHoverTimer->setSingleShot(true);

  mUndoStack = new QUndoStack(this);
  mUndoStack->setUndoLimit(20);
}

QString Canvas::id() const
{
  return mId;
}

Types::LibraryTypes Canvas::type() const
{
  return Types::LibraryTypes::UNKNOWN;
}

QUndoStack* Canvas::undoStack() const
{
  return mUndoStack;
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
      {
        LOG_WARNING("Tried to drop node on parent that does not accept drops");
        return;
      }
    }

    // Make sure that no other nodes are selected before dropping
    clearSelectedNodes();

    QByteArray data = event->mimeData()->data(Constants::TYPE_NODE);
    QDataStream stream(&data, QIODevice::ReadOnly);

    auto info = std::make_shared<NodeSaveInfo>();
    stream >> *info;
    info->setScale(parentView()->getScale());

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
    mMouseDown = true;

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->type() == NodeItem::Type)
    {
      if (!nodeClickHandler(event, item))
        return;
    }
    else if (item && (item->type() == TransitionItem::Type))
    {
      if (!transitionClickHandler(event, item))
        return;
    }
    else if (item && (item->type() == QGraphicsTextItem::Type || item->type() == QGraphicsSvgItem::Type))
    {
      auto parent = item->parentItem();
      if (parent && parent->type() == NodeItem::Type)
      {
        if (!nodeClickHandler(event, parent))
          return;
      }
      else if (parent && parent->type() == TransitionItem::Type)
      {
        if (!transitionClickHandler(event, parent))
          return;
      }
    }
    else if (!item)
    {
      LOG_DEBUG("Clearing selected nodes");
      mSelectedNodes.clear();
      clearSelectedNodes();
    }
  }
  else if (event->button() == Qt::MiddleButton)
  {
    parentView()->setDragMode(QGraphicsView::NoDrag);
  }

  QGraphicsScene::mousePressEvent(event);
}

bool Canvas::nodeClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item)
{
  NodeItem* node = static_cast<NodeItem*>(item);
  if (isModifierSet(event, Qt::AltModifier))
  {
    if (!node->canAddTransition())
    {
      LOG_WARNING("Node already has the maximum number of transitions");
      return false;
    }

    mNode = node;
    auto info = std::make_shared<TransitionSaveInfo>();
    mTransition = new TransitionItem(std::make_shared<TransitionSaveInfo>());
    mTransition->setZValue(node->zValue() - 1);
    LOG_INFO("Node: %s ZValue: %f %f", qPrintable(node->nodeId()), node->zValue(), mTransition->zValue());

    auto config = node->nextTransition();
    mTransition->setName(config.label);

    mTransition->setStart(node->id(), node->mapToScene(node->boundingRect().center()), {0, 0});
    mTransition->setEnd(Constants::TMP_CONNECTION_ID, event->scenePos(), {0, 0});

    addItem(mTransition);
    parentView()->setDragMode(QGraphicsView::NoDrag);
    event->accept();
    return false;
  }
  else if (isModifierSet(event, Qt::ControlModifier))
  {
    nodeClicked(node);
    selectNode(node, !node->isSelected());
    event->accept();
    return false;
  }
  // else
  // {
  //   // We cannot clear if there are multiple nodes selected
  //   if (selectedItems().size() < 2)
  //     clearSelectedNodes();

  //   nodeClicked(node);
  //   selectNode(node, true);
  // }

  return true;
}

bool Canvas::transitionClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item)
{
  TransitionItem* transition = static_cast<TransitionItem*>(item);
  transition->setSelected(true);

  emit transitionSelected(transition);

  return true;
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mTransition)
  {
    mTransition->move(Constants::TMP_CONNECTION_ID, event->scenePos());
  }
  else if (mMouseDown)
  {
    const int dist = (event->scenePos() - event->buttonDownScreenPos(Qt::LeftButton)).manhattanLength();
    if (!mDragging && dist >= 400)
      mDragging = true;
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
      if (item)
      {
        NodeItem* node = nullptr;

        if (item->type() == NodeItem::Type)
          node = static_cast<NodeItem*>(item);
        else if (item->type() == QGraphicsTextItem::Type || item->type() == QGraphicsSvgItem::Type)
          node = static_cast<NodeItem*>(item->parentItem());

        if (node)
        {
          mTransition->setEnd(node->id(), node->mapToScene(node->boundingRect().center()), {0, 0});
          mTransition->done(mNode, node);
        }
      }
      else
      {
        removeItem(mTransition);
      }

      mTransition = nullptr;
      mNode = nullptr;
    }
  }
  else if (QGraphicsItem* item = itemAt(event->scenePos(), QTransform()))
  {
    if (item && item->type() == NodeItem::Type && !mDragging)
    {
      NodeItem* node = static_cast<NodeItem*>(item);

      // We cannot clear if there are multiple nodes selected
      if (selectedItems().size() < 2)
        clearSelectedNodes();

      nodeClicked(node);
      selectNode(node, true);
    }
    else
    {
      clearSelection();
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

  mMouseDown = false;
  mDragging = false;

  QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
}

QMenu* Canvas::createAlignMenu(const QList<Types::AlignmentNode>& items)
{
  QMenu* alignMenu = new QMenu("Align");

  // QAction* distribute = alignMenu->addAction("Distribute");
  QAction* alignHCenter = alignMenu->addAction("Align H center");
  connect(alignHCenter, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::CENTER); });

  QAction* alignLeft = alignMenu->addAction("Align left");
  connect(alignLeft, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::START); });

  QAction* alignRight = alignMenu->addAction("Align right");
  connect(alignRight, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::END); });

  QAction* alignVCenter = alignMenu->addAction("Align V center");
  connect(alignVCenter, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::CENTER); });

  QAction* alignTop = alignMenu->addAction("Align top");
  connect(alignTop, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::START); });

  QAction* alignBottom = alignMenu->addAction("Align bottom");
  connect(alignBottom, &QAction::triggered, [this, &items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::END); });

  alignMenu->setEnabled(items.size() > 1);

  return alignMenu;
}

void Canvas::nodeClicked(NodeItem* node)
{
  if (!node)
    return;

  LOG_DEBUG("Selecting node: %s", qPrintable(node->nodeId()));

  // Remove if it was already in the list
  mSelectedNodes.removeAll(node);

  // Append as "last clicked"
  mSelectedNodes.append(node);
}

QList<NodeItem*> Canvas::selectedNodes() const
{
  return mSelectedNodes;
}

void Canvas::requestAlignNodes(const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction)
{
  mUndoStack->push(new AlignCommand(this, items, mode, direction));
}

void Canvas::alignNodes(const QList<Types::AlignmentNode>& nodes, Types::AlignmentMode mode, Types::AlignmentDirection direction, bool useGiven)
{
  if (nodes.size() < 2)
    return;

  if (useGiven)
  {
    for (Types::AlignmentNode item : nodes)
    {
      auto node = findNodeWithId(item.id);
      if (node == nullptr)
        continue;

      if (!item.pos.isNull())
        node->updatePosition(item.pos);
    }
  }
  else if (mode == Types::AlignmentMode::HORIZONTAL)
  {
    alignNodesHorizontally(nodes, direction);
  }
  else
  {
    alignNodesVertically(nodes, direction);
  }
}

void Canvas::alignNodesHorizontally(const QList<Types::AlignmentNode>& nodes, Types::AlignmentDirection direction)
{
  auto ref = nodes.first();
  auto refNode = findNodeWithId(ref.id);
  if (!refNode)
    return;

  qreal refX = 0;
  if (direction == Types::AlignmentDirection::START)
    refX = refNode->pos().x();
  else if (direction == Types::AlignmentDirection::END)
    refX = refNode->pos().x() + refNode->boundingRect().width();
  else
    refX = refNode->pos().x() + refNode->boundingRect().width() / 2;

  for (Types::AlignmentNode item : nodes)
  {
    auto node = findNodeWithId(item.id);
    if (!node || node == refNode)
      continue;

    QPointF p = node->pos();

    if (direction == Types::AlignmentDirection::START)
      node->updatePosition(QPointF(refX, p.y()));
    else if (direction == Types::AlignmentDirection::END)
      node->updatePosition(QPointF(refX - node->boundingRect().width(), p.y()));
    else
      node->updatePosition(QPointF(refX - node->boundingRect().width() / 2, p.y()));
  }
}

void Canvas::alignNodesVertically(const QList<Types::AlignmentNode>& nodes, Types::AlignmentDirection direction)
{
  auto ref = nodes.first();
  auto refNode = findNodeWithId(ref.id);
  if (!refNode)
    return;

  qreal refY = 0;
  if (direction == Types::AlignmentDirection::START)
    refY = refNode->pos().y();
  else if (direction == Types::AlignmentDirection::END)
    refY = refNode->pos().y() + refNode->boundingRect().height();
  else
    refY = refNode->pos().y() + refNode->boundingRect().height() / 2;

  for (auto item : nodes)
  {
    auto node = findNodeWithId(item.id);
    if (!node || node == refNode)
      continue;

    QPointF p = node->pos();
    if (direction == Types::AlignmentDirection::START)
      node->updatePosition(QPointF(p.x(), refY));
    else if (direction == Types::AlignmentDirection::END)
      node->updatePosition(QPointF(p.x(), refY - node->boundingRect().height()));
    else
      node->updatePosition(QPointF(p.x(), refY - node->boundingRect().height() / 2));
  }
}

void Canvas::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

  // Define menu actions
  QMenu menu;
  QList<NodeItem*> items = selectedNodes();
  QList<Types::AlignmentNode> itemIds = {};
  for (const auto node : items)
  {
    if (node != nullptr)
      itemIds.append(Types::AlignmentNode{node->id(), node->pos()});
  }

  if (!item)
  {
    // =============================================
    addSectionLabel(&menu, "Edit");

    QAction* copyAction = menu.addAction("Copy");
    copyAction->setEnabled(items.size() > 0);
    QObject::connect(copyAction, &QAction::triggered, [this]() {
      copySelectedItems(nullptr);
    });

    QAction* pasteAction = menu.addAction("Paste");
    pasteAction->setEnabled(!mCopiedNodes.isEmpty());
    QObject::connect(pasteAction, &QAction::triggered, [this]() {
      pasteCopiedItems();
    });

    QAction* deleteAction = menu.addAction("Delete");
    deleteAction->setEnabled(items.size() > 0);
    QObject::connect(deleteAction, &QAction::triggered, [this]() {
      deleteSelectedItems();
    });

    // =============================================
    addSectionLabel(&menu, "Visual");

    QAction* forwardAction = menu.addAction("To front");
    forwardAction->setEnabled(items.size() > 0);
    QObject::connect(forwardAction, &QAction::triggered, [this, items]() {
      qreal topZLevel = 0;
      for (QGraphicsItem* item : this->items())
        topZLevel = qMax(item->zValue(), topZLevel);

      for (NodeItem* node : items)
        node->setZValue(topZLevel);
    });

    QAction* backwardAction = menu.addAction("To back");
    backwardAction->setEnabled(items.size() > 0);
    QObject::connect(backwardAction, &QAction::triggered, [this, items]() {
      qreal topZLevel = 0;
      for (QGraphicsItem* item : this->items())
        topZLevel = qMin(item->zValue(), topZLevel);

      for (NodeItem* node : items)
        node->setZValue(topZLevel);
    });

    QAction* toggleLabelAction = menu.addAction("Toggle label");
    toggleLabelAction->setEnabled(items.size() > 0);
    QObject::connect(toggleLabelAction, &QAction::triggered, [items]() {
      for (QGraphicsItem* item : items)
      {
        if (item->type() == NodeItem::Type)
          dynamic_cast<NodeItem*>(item)->toggleLabelVisibility();
      }
    });

    menu.addMenu(createAlignMenu(itemIds));
  }
  else if (item->type() == NodeItem::Type)
  {
    NodeItem* node = static_cast<NodeItem*>(item);

    // =============================================
    addSectionLabel(&menu, "Creation");

    QAction* newEventAction = menu.addAction(tr("New flow"));
    newEventAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(newEventAction, &QAction::triggered, [this, node]() {
      emit createEvent(node);
    });
    menu.addAction(newEventAction);

    // =============================================
    addSectionLabel(&menu, "Edit");

    QAction* copyAction = menu.addAction("Copy");
    copyAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(copyAction, &QAction::triggered, [this, node]() {
      copySelectedItems(node);
    });

    QAction* pasteAction = menu.addAction("Paste");
    pasteAction->setEnabled(!mCopiedNodes.isEmpty());
    QObject::connect(pasteAction, &QAction::triggered, [this]() {
      pasteCopiedItems();
    });

    QAction* deleteAction = menu.addAction("Delete");
    deleteAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(deleteAction, &QAction::triggered, [this]() {
      deleteSelectedItems();
    });

    // =============================================
    addSectionLabel(&menu, "Visual");

    QAction* forwardAction = menu.addAction("To front");
    forwardAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(forwardAction, &QAction::triggered, [this, node]() {
      if (!node)
        return;

      qreal topZLevel = std::numeric_limits<double>::min();
      for (QGraphicsItem* item : this->items())
        topZLevel = qMax(item->zValue(), topZLevel);

      LOG_DEBUG("Moving front: %.2lf", topZLevel);
      node->setZValue(++topZLevel);
    });

    QAction* backwardAction = menu.addAction("To back");
    backwardAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(backwardAction, &QAction::triggered, [this, node]() {
      if (!node)
        return;

      qreal topZLevel = std::numeric_limits<double>::max();
      for (QGraphicsItem* item : this->items())
        topZLevel = qMin(item->zValue(), topZLevel);

      LOG_DEBUG("Moving back: %.2lf", topZLevel);
      node->setZValue(--topZLevel);
    });

    QAction* toggleLabelAction = menu.addAction("Toggle label");
    toggleLabelAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(toggleLabelAction, &QAction::triggered, [items]() {
      for (QGraphicsItem* item : items)
      {
        if (item->type() == NodeItem::Type)
          dynamic_cast<NodeItem*>(item)->toggleLabelVisibility();
      }
    });

    menu.addMenu(createAlignMenu(itemIds));
  }
  else if (item->type() == TransitionItem::Type)
  {
    // =============================================
    addSectionLabel(&menu, "Visual");

    // TransitionItem* transition = static_cast<TransitionItem*>(item);

    QAction* toggleLabelAction = menu.addAction("Toggle label");
    toggleLabelAction->setEnabled(items.size() > 0);
    QObject::connect(toggleLabelAction, &QAction::triggered, [items]() {
      // for (QGraphicsItem* item : items)
      // {
      //   if (item->type() == TransitionItem::Type)
      //     dynamic_cast<TransitionItem*>(item)->toggleLabelVisibility();
      // }
    });
  }

  // Execute the menu at the mouse cursor's position
  menu.exec(event->screenPos());
}

void Canvas::createNodeContextMenu(QMenu& menu)
{
}

void Canvas::createTransitionContextMenu(QMenu& menu)
{
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
      {
        transition->detach();
        connectionsToDelete.append(item);
      }
    }
  }

  // First delete the connections
  for (QGraphicsItem* connection : connectionsToDelete)
  {
    removeItem(connection);
    delete connection;
  }

  // Then delete the nodes
  mUndoStack->beginMacro("Remove nodes");

  for (NodeItem* node : nodesToDelete)
    triggerNodeRemoval(node);

  mUndoStack->endMacro();
}

void Canvas::triggerNodeRemoval(const NodeItem* node)
{
  // We have to remember that QT QUndoCommands trigger the 'redo' method after creation. Thus, the node
  // removal is not explicit (as I would like) but happens through the RemoveNodeCommand below
  mUndoStack->push(new RemoveNodeCommand(this, node->saveInfo()));
}

void Canvas::removeNode(const NodeSaveInfo info)
{
  auto node = findNodeWithId(info.getid());
  if (!node)
    return;

  QTimer::singleShot(0, this, [this, node]() {
    auto toRemove = removeNode(node);

    // Delete children before parents (important if parent owns child QGraphicsItems)
    for (int i = toRemove.size() - 1; i >= 0; --i)
    {
      if (auto node = static_cast<NodeItem*>(toRemove[i]))
        LOG_INFO("Deleting: %s, has parent: %d", qPrintable(node->id()), node->parentNode() != nullptr);

      delete toRemove[i];
    }
  });
}

QVector<QGraphicsItem*> Canvas::removeNode(NodeItem* node)
{
  if (!node)
    return {};

  removeItem(node);

  // Clear any potential callback
  node->nodeModified = nullptr;
  node->flowAdded = nullptr;

  LOG_TRACE("Removing node: %s", qPrintable(node->id()));

  QVector<QGraphicsItem*> itemsToRemove = {node};
  updateParent(node, nullptr, false);

  auto flows = node->flows();
  for (Flow* flow : flows)
  {
    node->deleteFlow(flow->id());
    emit flowRemoved(flow->id(), node->id());
  }

  if (node->parentNode())
    node->parentNode()->childRemoved(node);

  auto transtionsToDelete = node->transitions();
  for (TransitionItem* transition : transtionsToDelete)
  {
    transition->detach();
    removeItem(transition);
    itemsToRemove.append(transition);
  }

  auto toDelete = node->children();
  for (NodeItem* child : toDelete)
    itemsToRemove += removeNode(child);  // Do not create undo for children

  mSelectedNodes.removeAll(node);

  emit nodeRemoved(node);

  // Since this function can be called in loops or recusively, we do not perform the deletion of the pointer.
  // Deletion is the responsibility of the outer caller
  return itemsToRemove;
}

bool Canvas::isParentSelected(NodeItem* node)
{
  if (node->parentNode() == nullptr)
    return false;

  auto parent = static_cast<NodeItem*>(node->parentNode());
  return parent->isSelected() || isParentSelected(parent);
}

void Canvas::copySelectedItems(NodeItem* clickedNode)
{
  mCopiedNodes.clear();

  QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  if (clickedNode != nullptr)
  {
    auto info = clickedNode->saveInfo();
    LOG_DEBUG("Selected %s (%.2f %.2f)", qPrintable(clickedNode->id()), info.getposition().x(), info.getposition().y());

    mCopiedNodes.append({info, mousePosition - info.getposition()});
  }

  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() != NodeItem::Type)
      continue;

    NodeItem* node = dynamic_cast<NodeItem*>(item);
    if (!node)
      continue;

    // Do not copy the children
    if (isParentSelected(node))
      continue;

    // Save relative position
    auto info = node->saveInfo();
    LOG_DEBUG("Selected %s (%.2f %.2f)", qPrintable(node->id()), info.getposition().x(), info.getposition().y());

    mCopiedNodes.append({info, mousePosition - info.getposition()});

    // Make sure the item is not selected after copying
    selectNode(node, false);
  }
}

void Canvas::pasteCopiedItems(const QPointF& mousePosition, NodeItem* parentNode, QList<CopiedNode> copiedNodes, bool absolute)
{
  for (const auto& copy : copiedNodes)
  {
    auto infoPtr = std::make_shared<NodeSaveInfo>(copy.info);

    QPointF newParentPosition = {0.0, 0.0};
    if (parentNode)
      newParentPosition = parentNode->saveInfo().getposition();

    auto node = createNode(NodeCreation::Pasting,
                           infoPtr,
                           absolute ? mousePosition - copy.posRelativeToMouse : (newParentPosition + (copy.info.getposition() - copy.posRelativeToMouse)),
                           parentNode);

    QList<CopiedNode> children;
    for (const auto& child : copy.info.getchildren())
      children.push_back({*std::dynamic_pointer_cast<NodeSaveInfo>(child), copy.info.getposition()});

    pasteCopiedItems(mousePosition, node, children, false);
    selectNode(node, false);
  }
}

void Canvas::pasteCopiedItems()
{
  if (mCopiedNodes.isEmpty())
    return;

  QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  NodeItem* parentNode = nullptr;
  QGraphicsItem* item = itemAt(mousePosition, QTransform());
  if (item && item->type() == NodeItem::Type)
  {
    parentNode = static_cast<NodeItem*>(item);

    // Add error message
    if (!parentNode->acceptDrops())
      return;
  }

  pasteCopiedItems(mousePosition, parentNode, mCopiedNodes, true);
}

void Canvas::clearCanvas()
{
  QVector<QGraphicsItem*> toRemove = {};
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

    toRemove = removeNode(node);
  }

  QTimer::singleShot(0, this, [toRemove]() {
    for (QGraphicsItem* item : toRemove)
      delete item;
  });
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

VoidResult Canvas::loadFromSave(const QVector<std::shared_ptr<INode>>& nodes, NodeItem* parent)
{
  for (std::shared_ptr<INode> inodeInfo : nodes)
  {
    // TODO(felaze): This is necessary because the save info is using shared ptr when it shouldn't...
    // I need to make a proper distinction between save and run-time store structures.
    std::shared_ptr<NodeSaveInfo> nodeInfo = std::dynamic_pointer_cast<NodeSaveInfo>(inodeInfo);

    auto node = std::make_shared<NodeSaveInfo>(*nodeInfo);

    LOG_DEBUG("Creating node %s with parent %s", qPrintable(node->getid()), qPrintable(node->getparentId()));
    auto createdNode = createNode(NodeCreation::Loading, node, node->getposition(), parent);

    auto ret = loadFromSave(nodeInfo->getchildren(), createdNode);
    if (!ret.IsSuccess())
    {
      LOG_ERROR("%s", ret.ErrorMessage().c_str());
      return VoidResult::Failed(ret.ErrorMessage());
    }

    for (const auto& flow : node->getflows())
      (void)createdNode->createFlow(flow->getname(), std::dynamic_pointer_cast<FlowSaveInfo>(flow));

    selectNode(createdNode, false);
  }

  return VoidResult();
}

VoidResult Canvas::loadFromSave(const SaveInfo& info)
{
  // Clear the canvas before repopulating
  clearCanvas();

  // Reset canvas
  // TODO(felaze): This should be moved to the CanvasView, something like parentView()->loadFromSave(info.canvasInfo);
  auto canvasInfo = info.canvasInfo();
  parentView()->zoom(canvasInfo.scale() / parentView()->getScale());
  parentView()->setScale(canvasInfo.scale());
  parentView()->centerOn(canvasInfo.center());

  return loadFromSave(info.getnodes(), nullptr);
}

CanvasView* Canvas::parentView() const
{
  return static_cast<CanvasView*>(parent());
}

void Canvas::moveNodeTo(const QString& nodeId, const QPointF& position)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
    return;

  node->updatePosition(position);
}

void Canvas::setNodeSize(const QString& nodeId, const QSizeF& size)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
    return;

  node->applySize(size);
}

void Canvas::createTransition(const TransitionSaveInfo& info)
{
}

void Canvas::removeTransition(const TransitionSaveInfo& info)
{
}

// TODO: Properly integrate the creation with the undo command
void Canvas::createNode(const NodeSaveInfo info)
{
  auto exists = findNodeWithId(info.getid());
  if (exists)
    return;

  auto parent = findNodeWithId(info.getparentId());

  auto infoPtr = std::make_shared<NodeSaveInfo>(info);
  (void)createNode(NodeCreation::Populating, infoPtr, info.getposition(), parent);

  // We also need to create the children of the node
  for (const auto& child : info.getchildren())
    (void)createNode(*std::dynamic_pointer_cast<NodeSaveInfo>(child));
}

NodeItem* Canvas::createNode(NodeCreation creation, std::shared_ptr<NodeSaveInfo> info, const QPointF& position, NodeItem* parent)
{
  auto config = mConfigTable->get(info->getnodeId());
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
  auto nodeId = creation == NodeCreation::Pasting ? "" : info->getid();
  NodeItem* node = new NodeItem(nodeId, info, position, config);

  if (parent == nullptr)
  {
    if (type() == Types::LibraryTypes::STRUCTURAL)
      mStorage->addNode(info);
  }
  else
  {
    node->addParent(parent);
    parent->addChild(node, info);
  }

  // TODO(felaze): Move these to a function or so
  node->nodeModified = [this](NodeItem* item) {
    emit nodeModified(item);
  };
  node->flowAdded = [this](Flow* flow, NodeItem* node) {
    emit flowAdded(flow, node);
  };

  node->start();

  // All nodes are children of the canvas
  addItem(node);

  if (creation != NodeCreation::Populating)
    updateParent(node, info, true);

  emit nodeAdded(node);

  if (creation != NodeCreation::Populating)
    mUndoStack->push(new AddNodeCommand(this, node->saveInfo()));

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
  if (!node)
  {
    LOG_WARNING("No node with id: %s", qPrintable(nodeId));
    return;
  }

  triggerNodeRemoval(node);
}

// ==========================================================================================
// Flow
void Canvas::populate(Flow* flow)
{
  // First create all the nodes
  for (std::shared_ptr<NodeSaveInfo> node : flow->getNodes())
  {
    LOG_DEBUG("Creating behavioral node %s with parent %s", qPrintable(node->getid()), qPrintable(node->getparentId()));
    auto created = createNode(NodeCreation::Populating, node, node->getposition(), findNodeWithId(node->getparentId()));
    LOG_DEBUG("Created node %s", qPrintable(created->id()));
  }

  // Then create the transitions between the nodes
  for (std::shared_ptr<NodeSaveInfo> node : flow->getNodes())
  {
    auto srcConn = findNodeWithId(node->getid());
    if (!srcConn)
    {
      LOG_WARNING("Could not find source node");
      continue;
    }

    for (std::shared_ptr<ITransition> itransition : node->gettransitions())
    {
      auto transition = std::dynamic_pointer_cast<TransitionSaveInfo>(itransition);
      auto dstConn = findNodeWithId(transition->getdstId());
      if (!dstConn)
      {
        LOG_WARNING("Could not find destination node");
        continue;
      }

      LOG_DEBUG("Creating transitions %s -> %s", qPrintable(node->getid()), qPrintable(transition->getdstId()));

      auto connection = new TransitionItem(transition);

      connection->setStart(node->getid(), transition->srcPoint(), transition->srcShift());
      connection->setEnd(transition->getdstId(), transition->dstPoint(), transition->dstShift());

      connection->done(srcConn, dstConn);

      addItem(connection);
    }
  }
}

void Canvas::onFlowSelected(const QString& flowId, const QString& nodeId)
{
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
  emit flowRemoved(flowId, nodeId);
}

void Canvas::updateParent(NodeItem* /* node */, std::shared_ptr<NodeSaveInfo> /* storage */, bool /* adding */)
{
}

void Canvas::themeChanged()
{
  for (QGraphicsItem* item : items())
  {
    item->update();
  }
}
