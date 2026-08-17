#include "canvas.h"

#include <QBuffer>
#include <QClipboard>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QFrame>
#include <QGraphicsSceneDragDropEvent>
#include <QGridLayout>
#include <QIcon>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QCoreApplication>
#include <QScrollArea>
#include <QSet>
#include <QTimer>
#include <QToolButton>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <algorithm>
#include <limits>
#include <map>
#include <memory>
#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "app_configs.h"
#include "app_paths.h"
#include "canvas_view.h"
#include "common/style_helpers.h"
#include "config.h"
#include "config_table.h"
#include "edge_router.h"
#include "elements/behaviour/call_node.h"
#include "elements/behaviour/component_overlay.h"
#include "elements/behaviour/subflow_block.h"
#include "elements/behaviour/wait_node.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "elements/node_factory.h"
#include "elements/port.h"
#include "elements/transition.h"
#include "flow_info.h"
#include "keys.h"
#include "logging.h"
#include "node_info.h"
#include "result.h"
#include "save_info.h"
#include "types.h"
#include "undo_commands/add_node.h"
#include "undo_commands/align.h"
#include "undo_commands/remove_node.h"
#include "widgets/capability_icon_menu.h"
#include "widgets/structure/node_action_menu.h"
#include "widgets/structure/call_config_popup.h"
#include "widgets/widget_factory.h"

namespace
{
NodeItem* dropTargetContainer(QGraphicsItem* item, Types::LibraryTypes canvasType, QGraphicsScene* scene = nullptr,
                              const QPointF& scenePos = QPointF())
{
  while (item)
  {
    if (item->type() == NodeItem::Type)
    {
      auto* node = static_cast<NodeItem*>(item);
      // collapsed blocks stay hidden and don't capture drops
      if (node->isSubflowContainer() && canvasType == Types::LibraryTypes::BEHAVIOUR && !node->isCollapsedSubflow())
        return node;

      for (NodeItem* cur = node; cur; cur = cur->parentNode())
      {
        if (cur->isTaskContainer() && cur->acceptDrops())
          return cur;
      }
      return nullptr;
    }
    item = item->parentItem();
  }

  // SubflowBlock uses a border-only shape for hit-testing, so empty interior drops
  // must still resolve the container via its body rect.
  if (canvasType == Types::LibraryTypes::BEHAVIOUR && scene)
  {
    for (QGraphicsItem* gi : scene->items())
    {
      if (!gi || gi->type() != NodeItem::Type)
        continue;

      auto* node = static_cast<NodeItem*>(gi);
      if (!node->isSubflowContainer() || node->isCollapsedSubflow())
        continue;

      if (node->mapRectToScene(node->nodeRect()).contains(scenePos))
        return node;
    }
  }

  return nullptr;
}

NodeItem* taskContainerAcceptingDrop(QGraphicsItem* item)
{
  return dropTargetContainer(item, Types::LibraryTypes::STRUCTURAL);
}

NodeItem* ancestorNodeItem(QGraphicsItem* item)
{
  for (QGraphicsItem* cur = item; cur; cur = cur->parentItem())
  {
    if (cur->type() == NodeItem::Type)
      return static_cast<NodeItem*>(cur);
    if (cur->type() == Types::PORT)
      return static_cast<PortItem*>(cur)->nodeItem();
  }
  return nullptr;
}

QWidget* viewportFor(QGraphicsScene* scene)
{
  if (!scene)
    return nullptr;
  auto* view = qobject_cast<QGraphicsView*>(scene->parent());
  return view ? view->viewport() : nullptr;
}
}  // namespace

static constexpr auto MAKI_CLIPBOARD_MIME = "application/x-maki-copied-nodes";

Canvas::Canvas(const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable,
               std::shared_ptr<EdgeRouter> router, QObject* parent)
    : QGraphicsScene(parent)
    , mConfigTable(configTable)
    , mRouter(router)
    , mId(canvasId)
    , mCopiedNodes({})
{
  setBackgroundBrush(Qt::transparent);

  // SubflowBlock::boundingRect() spans a connector drawn up to its owner node, so
  // it changes whenever that owner moves or resizes — which the block cannot always
  // announce via prepareGeometryChange(). The BSP index caches item rects and would
  // then keep leaf entries pointing at freed items. A linear scan has no such cache.
  setItemIndexMethod(ItemIndexMethod::NoIndex);

  // mHoverTimer = new QTimer(this);
  // mHoverTimer->setSingleShot(true);

  mUndoStack = new QUndoStack(this);
  mUndoStack->setUndoLimit(20);

  connect(this, &QGraphicsScene::selectionChanged, this, &Canvas::onSelectionChanged);
  connect(mUndoStack, &QUndoStack::cleanChanged, this, &Canvas::onCleanChanged);
}

Canvas::~Canvas()
{
  disconnect(this, &QGraphicsScene::selectionChanged, this, &Canvas::onSelectionChanged);
  disconnect(mUndoStack, &QUndoStack::cleanChanged, this, &Canvas::onCleanChanged);
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

std::shared_ptr<EdgeRouter> Canvas::router() const
{
  return mRouter;
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

void Canvas::onSelectionChanged()
{
  const auto selected = selectedItems();
  mSelectedNodes.erase(std::remove_if(mSelectedNodes.begin(), mSelectedNodes.end(),
                                      [&selected](NodeItem* node) {
                                        return !selected.contains(node);
                                      }),
                       mSelectedNodes.end());

  for (auto* item : selected)
    if (auto* node = qgraphicsitem_cast<NodeItem*>(item))
      if (!mSelectedNodes.contains(node))
        mSelectedNodes.append(node);
}

void Canvas::onCleanChanged(bool state)
{
}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  if (!event->mimeData()->hasFormat(Constants::TYPE_NODE))
  {
    QGraphicsScene::dragEnterEvent(event);
    return;
  }
  event->acceptProposedAction();

  mDraggedNodeIsCapability = false;
  mDraggedCapabilityIconPath.clear();
  mDraggedCapabilityColor = QColor();
  clearCapabilityDropPreview();

  QByteArray data = event->mimeData()->data(Constants::TYPE_NODE);
  QDataStream stream(&data, QIODevice::ReadOnly);
  NodeSaveInfo peekInfo;
  stream >> peekInfo;
  auto cfg = mConfigTable->get(peekInfo.getnodeId());
  if (cfg && type() == Types::LibraryTypes::STRUCTURAL && cfg->libraryType == Types::LibraryTypes::STRUCTURAL && cfg->type != QStringLiteral("Koda::Task"))
  {
    mDraggedNodeIsCapability = true;
    mDraggedCapabilityColor = cfg->body.backgroundColor;
    if (cfg->body.nodeSvg.isEmpty() && !cfg->body.iconPath.isEmpty())
      mDraggedCapabilityIconPath = AppPaths::icon(cfg->body.iconPath);
  }

  updateCapabilityDropPreview(event->scenePos());
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  if (event->mimeData()->hasFormat(Constants::TYPE_NODE))
  {
    event->acceptProposedAction();
    updateCapabilityDropPreview(event->scenePos());
    return;
  }
  QGraphicsScene::dragMoveEvent(event);
}

void Canvas::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
  clearCapabilityDropPreview();
  mDraggedNodeIsCapability = false;
  mDraggedCapabilityIconPath.clear();
  mDraggedCapabilityColor = QColor();
  QGraphicsScene::dragLeaveEvent(event);
}

void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  clearCapabilityDropPreview();
  mDraggedNodeIsCapability = false;
  mDraggedCapabilityIconPath.clear();
  mDraggedCapabilityColor = QColor();

  if (event->mimeData()->hasFormat(Constants::TYPE_NODE))
  {
    NodeItem* parentNode = dropTargetContainer(itemAt(event->scenePos(), QTransform()), type(), this, event->scenePos());

    // Make sure that no other nodes are selected before dropping
    clearSelectedNodes();

    QByteArray data = event->mimeData()->data(Constants::TYPE_NODE);
    QDataStream stream(&data, QIODevice::ReadOnly);

    auto info = std::make_shared<NodeSaveInfo>();
    stream >> *info;
    info->setScale(parentView()->getScale());

    if (TransitionItem* transition = transitionAt(event->scenePos()))
    {
      auto dropConfig = mConfigTable->get(info->getnodeId());
      if (dropConfig && dropConfig->libraryType == type())
      {
        if (NodeItem* inserted = insertDroppedNodeOnTransition(transition, info))
        {
          selectNode(inserted, true);
          onNodeDroppedFromPalette(inserted);
          event->acceptProposedAction();
          dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::ArrowCursor);
          return;
        }
      }
    }

    auto node = createNode(NodeCreation::Dropping, info, event->scenePos(), parentNode);
    if (node)
    {
      if (parentNode)
        parentNode->expandSubflowToFitChildren();
      selectNode(node, true);
      onNodeDroppedFromPalette(node);
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

void Canvas::clearCapabilityDropPreview()
{
  if (mCapabilityPreviewTask)
  {
    mCapabilityPreviewTask->setHoverPreview(QString(), QColor(), false);
    mCapabilityPreviewTask = nullptr;
  }
}

void Canvas::updateCapabilityDropPreview(const QPointF& scenePos)
{
  if (mDraggedNodeIsCapability && type() == Types::LibraryTypes::STRUCTURAL)
  {
    NodeItem* task = taskContainerAcceptingDrop(itemAt(scenePos, QTransform()));
    if (!task || !mCapabilityPreviewTask || task == mCapabilityPreviewTask)
      return;

    clearCapabilityDropPreview();
    mCapabilityPreviewTask = task;
    if (mCapabilityPreviewTask)
      mCapabilityPreviewTask->setHoverPreview(mDraggedCapabilityIconPath, mDraggedCapabilityColor, true);
  }
  else if (TransitionItem* transition = transitionAt(scenePos))
  {
    transition->setSelected(true);
  }
  else
  {
    clearSelection();
  }
}

void Canvas::ensureNodeActionMenu()
{
  if (mNodeActionMenu)
    return;

  CanvasView* view = parentView();
  if (!view)
    return;

  mNodeActionMenu = new NodeActionMenu(view);
  connect(mNodeActionMenu, &NodeActionMenu::openMainFlowRequested, this, [this](NodeItem* n) {
    if (!n)
      return;
    if (Flow* flow = n->ensureMainFlow())
      emit openFlow(flow, QString());
  });
  connect(mNodeActionMenu, &NodeActionMenu::addFlowRequested, this, [this](NodeItem* n) {
    if (n)
      emit createEvent(n);
  });
  connect(mNodeActionMenu, &NodeActionMenu::addSubtaskRequested, this, &Canvas::addSubtaskTo);

  if (!mNodeActionHideTimer)
  {
    mNodeActionHideTimer = new QTimer(this);
    mNodeActionHideTimer->setSingleShot(true);

    connect(mNodeActionHideTimer, &QTimer::timeout, this, [this] {
      if (!mNodeActionMenu)
        return;

      if (mNodeActionMenu->underMouse())
        return;

      mNodeActionMenu->hideMenu();
      mHoveredActionNode = nullptr;
    });
  }
}

void Canvas::onNodeHovered(NodeItem* node, bool show)
{
  if (!node || !node->isTaskContainer())
    return;

  ensureNodeActionMenu();

  if (!mNodeActionMenu)
    return;

  if (show)
  {
    mNodeActionHideTimer->stop();
    mHoveredActionNode = node;
    mNodeActionMenu->showForTask(node, parentView());
    return;
  }
  else if (mHoveredActionNode == node)
    mNodeActionHideTimer->start(150);
}

void Canvas::addSubtaskTo(NodeItem* task)
{
  if (!task || !task->isTaskContainer() || type() != Types::LibraryTypes::STRUCTURAL)
    return;

  auto info = std::make_shared<NodeSaveInfo>();
  info->setNodeId(task->nodeId());
  info->setSize(QSizeF(static_cast<qreal>(task->config()->body.width),
                       static_cast<qreal>(task->config()->body.height)));
  if (CanvasView* view = parentView())
    info->setScale(view->getScale());

  clearSelectedNodes();
  const QPointF pos = task->scenePos() + QPointF(0, task->boundingRect().height() + 80.0);
  NodeItem* created = createNode(NodeCreation::Dropping, info, pos, task);
  if (created)
    selectNode(created, true);
}

void Canvas::openCapabilityMenu(NodeItem* task)
{
  if (!task || type() != Types::LibraryTypes::STRUCTURAL || !task->isTaskContainer())
    return;

  CanvasView* view = parentView();
  if (!view || !mConfigTable)
    return;

  QVector<CapabilityIconMenuItem> items;
  for (const auto& kv : mConfigTable->entries())
  {
    const auto& cfg = kv.second;
    if (!cfg || cfg->libraryType != Types::LibraryTypes::STRUCTURAL)
      continue;
    if (cfg->type == QStringLiteral("Koda::Task"))
      continue;

    const QString& key = kv.first;
    const int sep = key.indexOf(QLatin1String("::"));
    QString libName = cfg->libraryName;
    if (libName.isEmpty())
      libName = (sep > 0) ? key.left(sep) : QStringLiteral("(other)");

    CapabilityIconMenuItem item;
    item.id = key;
    item.name = CapabilityIconMenu::displayNameFromQualifiedType(cfg->type);
    item.section = libName;
    item.iconPath = behaviour::resolveCapabilityIconPath(QString(), key, mConfigTable.get());
    items.push_back(item);
  }

  std::sort(items.begin(), items.end(), [](const CapabilityIconMenuItem& a, const CapabilityIconMenuItem& b) {
    if (a.section != b.section)
      return a.section < b.section;
    return a.name.localeAwareCompare(b.name) < 0;
  });

  const QRectF slotRect = task->placeholderSlotSceneRect();
  const QPointF anchorScene = slotRect.center() + QPointF(0, slotRect.height() * 0.5 + 4.0);
  const QPoint globalAnchor = view->viewport()->mapToGlobal(view->mapFromScene(anchorScene));

  CapabilityIconMenu::exec(view, items, globalAnchor, [this, task](const CapabilityIconMenuItem& selected) {
    auto cfg = mConfigTable->get(selected.id);
    if (!cfg)
      return;

    auto info = std::make_shared<NodeSaveInfo>();
    info->setNodeId(selected.id);
    info->setSize(QSizeF(cfg->body.width, cfg->body.height));
    info->setScale(parentView()->getScale());

    clearSelectedNodes();
    const QPointF slotCenter = task->placeholderSlotSceneRect().center();
    NodeItem* created = createNode(NodeCreation::Dropping, info, slotCenter, task);
    if (created)
      selectNode(created, true);
  });
}

void Canvas::openCallConfigPopup(NodeItem* callNode)
{
  auto* call = dynamic_cast<CallNode*>(callNode);
  if (!call || type() != Types::LibraryTypes::BEHAVIOUR)
    return;

  CanvasView* view = parentView();
  auto storage = projectStorage();
  if (!view || !storage)
    return;

  QRectF anchorRect = call->hasCapabilitySelected() ? call->eventChipSceneRect() : call->capabilitySlotSceneRect();
  if (anchorRect.isEmpty())
    anchorRect = call->capabilitySlotSceneRect();
  const QPointF anchorScene(anchorRect.center().x(), anchorRect.bottom() + 4.0);
  const QPoint globalAnchor = view->viewport()->mapToGlobal(view->mapFromScene(anchorScene));

  CallConfigPopup::open(view, call, storage.get(), mConfigTable.get(), globalAnchor);
}

void Canvas::openWaitCapabilityMenu(NodeItem* waitNode)
{
  auto* wait = dynamic_cast<WaitNode*>(waitNode);
  if (!wait || type() != Types::LibraryTypes::BEHAVIOUR)
    return;

  CanvasView* view = parentView();
  auto storage = projectStorage();
  if (!view || !storage)
    return;

  QVector<CapabilityIconMenuItem> items;
  for (const auto& item : CapabilityIconMenu::itemsFromPossibleCallers(*storage, wait->id(), mConfigTable.get()))
  {
    const auto outs = storage->getEventsOfTypeFromNode(item.id, {Types::CallType::OUT});
    if (!outs.isEmpty())
      items.push_back(item);
  }

  QRectF chipRect = wait->waitChipSceneRect();
  if (!chipRect.isValid() || chipRect.isEmpty())
    chipRect = wait->mapRectToScene(wait->boundingRect());
  const QPointF anchorScene(chipRect.center().x(), chipRect.bottom() + 4.0);
  const QPoint globalAnchor = view->viewport()->mapToGlobal(view->mapFromScene(anchorScene));

  CapabilityIconMenu::exec(view, items, globalAnchor,
                           [wait](const CapabilityIconMenuItem& selected) { wait->assignCapability(selected.name); },
                           QObject::tr("No capabilities with OUT events"));
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

    if (mHoverPortHalo)
    {
      if (QWidget* vp = viewportFor(this))
        vp->unsetCursor();
      mHoverPortHalo = false;
    }

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (type() == Types::LibraryTypes::STRUCTURAL)
    {
      if (NodeItem* task = taskContainerAcceptingDrop(item))
      {
        if (task->placeholderSlotContainsScenePoint(event->scenePos()))
        {
          event->accept();
          openCapabilityMenu(task);
          return;
        }
      }
    }
    else if (type() == Types::LibraryTypes::BEHAVIOUR)
    {
      if (NodeItem* node = ancestorNodeItem(item))
      {
        if (auto* call = dynamic_cast<CallNode*>(node))
        {
          if (call->eventChipContainsScenePoint(event->scenePos())
              || call->capabilitySlotContainsScenePoint(event->scenePos()))
          {
            event->accept();
            openCallConfigPopup(call);
            return;
          }
        }
        if (auto* wait = dynamic_cast<WaitNode*>(node))
        {
          if (wait->waitChipContainsScenePoint(event->scenePos()))
          {
            event->accept();
            openWaitCapabilityMenu(wait);
            return;
          }
        }
        if (tryOpenNodeConfigAt(node, event->scenePos()))
        {
          event->accept();
          return;
        }
      }
    }

    if (item && item->type() == Types::PORT)
    {
      auto* port = static_cast<PortItem*>(item);
      if (beginTransitionFromOutPort(port, event->scenePos()))
      {
        event->accept();
        return;
      }
      QGraphicsScene::mousePressEvent(event);
      return;
    }
    if (!item)
    {
      if (auto* nearPort = portNearOutgoing(event->scenePos(), PortItem::kHitPadding))
      {
        if (beginTransitionFromOutPort(nearPort, event->scenePos()))
        {
          event->accept();
          return;
        }
      }
    }
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
      mSelectionStart = event->scenePos();
      auto color = Config::HIGHLIGHT;
      color.setAlpha(15);
      mSelectionRect = addRect(QRectF(mSelectionStart, mSelectionStart), QPen(color, 1, Qt::DashLine), QColor(color));
      mSelectionRect->setZValue(1'000'000);

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

bool Canvas::canAddTransition(NodeItem* node, PortItem* port) const
{
  Q_UNUSED(node);
  Q_UNUSED(port);
  return false;
}

TransitionConfig Canvas::nextTransition(NodeItem* node) const
{
  Q_UNUSED(node)
  return TransitionConfig{};
}

void Canvas::addTransition(TransitionItem* transition)
{
  Q_UNUSED(transition)
}

void Canvas::removeTransition(TransitionItem* transition)
{
  Q_UNUSED(transition)
}

void Canvas::addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  Q_UNUSED(info)
  emit nodeAdded(node);
}

void Canvas::onNodeDroppedFromPalette(NodeItem* node)
{
  Q_UNUSED(node);
}

bool Canvas::tryOpenNodeConfigAt(NodeItem* node, const QPointF& scenePos)
{
  Q_UNUSED(node);
  Q_UNUSED(scenePos);
  return false;
}

void Canvas::addedItemFlow(Flow* flow, NodeItem* node)
{
  emit flowAdded(flow, node);
}

bool Canvas::nodeClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item)
{
  NodeItem* node = static_cast<NodeItem*>(item);
  if (isModifierSet(event, Qt::ControlModifier))
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

    // Hover in case we are on a node
    if (PortItem* nearIn = portNear(event->scenePos(), static_cast<int>(PortItem::In), PortItem::kHitPadding))
      nearIn->nodeItem()->setSelected(true);
    else if (NodeItem* node = qgraphicsitem_cast<NodeItem*>(itemAt(event->scenePos(), QTransform())))
      node->setSelected(true);
    else
      clearSelection();
  }
  else if (mMouseDown)
  {
    const int dist = (event->scenePos() - event->buttonDownScreenPos(Qt::LeftButton)).manhattanLength();
    if (!mDragging && dist >= 400)
      mDragging = true;

    if (mSelectionRect)
    {
      mSelectionRect->setRect(QRectF(mSelectionStart, event->scenePos()).normalized());
      event->accept();
      return;
    }
  }
  else if (event->buttons() == Qt::NoButton)
  {
    const bool inHalo = (itemAt(event->scenePos(), QTransform()) == nullptr) && (portNearOutgoing(event->scenePos(), PortItem::kHitPadding) != nullptr);

    if (inHalo && !mHoverPortHalo)
    {
      if (QWidget* vp = viewportFor(this))
        vp->setCursor(Qt::CrossCursor);
      mHoverPortHalo = true;
    }
    else if (!inHalo && mHoverPortHalo)
    {
      if (QWidget* vp = viewportFor(this))
        vp->unsetCursor();
      mHoverPortHalo = false;
    }
  }

  QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  parentView()->setDragMode(QGraphicsView::RubberBandDrag);

  QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

  if (mTransition)
  {
    if (event->button() == Qt::LeftButton)
    {
      NodeItem* dest = ancestorNodeItem(item);

      if (!dest)
      {
        if (PortItem* nearIn = portNear(event->scenePos(), static_cast<int>(PortItem::In), PortItem::kHitPadding))
          dest = nearIn->nodeItem();
      }

      bool completed = false;
      if (dest && mNode && dest != mNode && dest->type() == NodeItem::Type && canCompleteTransitionTo(dest, event->scenePos()) &&
          canConnectNodes(mNode, dest))
      {
        const QPointF endPos = dest->incomingPortAnchor();
        mTransition->setEnd(dest->id(), endPos, {0, 0});

        // SubflowBlock loop ports have no transition events.
        if (mNode->isSubflowContainer() || dest->isSubflowContainer())
        {
          mTransition->setEvent(QString());
          mTransition->setName(QString());
        }

        mTransition->done(mNode, dest);
        addTransition(mTransition);
        clearSelection();
        mTransition->setSelected(true);
        completed = true;
      }

      if (!completed)
        removeItem(mTransition);

      mTransition = nullptr;
      mNode = nullptr;
    }
  }
  else if (item && (!mSelectionRect || item != mSelectionRect))
  {
    if (!mDragging)
    {
      NodeItem* node = nullptr;
      if (item->type() == NodeItem::Type)
        node = static_cast<NodeItem*>(item);
      else if ((item->type() == QGraphicsTextItem::Type || item->type() == QGraphicsSvgItem::Type) && item->parentItem()->type() == NodeItem::Type)
        node = static_cast<NodeItem*>(item->parentItem());
      else if (item->type() == Types::PORT)
        node = static_cast<PortItem*>(item)->nodeItem();

      if (node)
      {
        // We cannot clear if there are multiple nodes selected
        if (selectedItems().size() < 2)
          clearSelectedNodes();

        nodeClicked(node);
        selectNode(node, true);
      }
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

      for (QGraphicsItem* selectedItem : selectedItems())
      {
        QRectF itemBounds = selectedItem->sceneBoundingRect();
        if ((draggingFromRight && !selectionPath.intersects(itemBounds)) ||
            (!draggingFromRight && !selectionPath.contains(itemBounds)))
        {
          selectedItem->setSelected(false);
        }
      }
    }

    if (mSelectionRect)
    {
      removeItem(mSelectionRect);
      delete mSelectionRect;
      mSelectionRect = nullptr;
    }
  }

  mMouseDown = false;
  mDragging = false;

  QGraphicsScene::mouseReleaseEvent(event);  // Allow normal item drop behavior
}

void Canvas::createAlignMenu(QMenu* alignMenu, const QList<Types::AlignmentNode>& items)
{
  // QAction* distribute = alignMenu->addAction("Distribute");
  QAction* alignHCenter = alignMenu->addAction(iconFromTheme("align-horizontal-center"), "Align H center");
  connect(alignHCenter, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::CENTER); });

  QAction* alignLeft = alignMenu->addAction(iconFromTheme("align-horizontal-left"), "Align left");
  connect(alignLeft, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::START); });

  QAction* alignRight = alignMenu->addAction(iconFromTheme("align-horizontal-right"), "Align right");
  connect(alignRight, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::HORIZONTAL, Types::AlignmentDirection::END); });

  QAction* alignVCenter = alignMenu->addAction(iconFromTheme("align-vertical-center"), "Align V center");
  connect(alignVCenter, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::CENTER); });

  QAction* alignTop = alignMenu->addAction(iconFromTheme("align-vertical-top"), "Align top");
  connect(alignTop, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::START); });

  QAction* alignBottom = alignMenu->addAction(iconFromTheme("align-vertical-bottom"), "Align bottom");
  connect(alignBottom, &QAction::triggered, [this, items]() { requestAlignNodes(items, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::END); });

  alignMenu->setIcon(iconFromTheme("align-none"));
  alignMenu->setEnabled(items.size() > 1);

  const auto width = qobject_cast<QGraphicsView*>(parent())->viewport()->width();
  alignMenu->setMinimumWidth(width / 6);
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

void Canvas::requestAlignNodes(const QList<Types::AlignmentNode>& items,
                               Types::AlignmentMode mode, Types::AlignmentDirection direction)
{
  mUndoStack->push(new AlignCommand(this, items, mode, direction));
}

void Canvas::alignNodes(const QList<Types::AlignmentNode>& nodes,
                        Types::AlignmentMode mode, Types::AlignmentDirection direction, bool useGiven)
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
  const auto width = qobject_cast<QGraphicsView*>(parent())->viewport()->width();

  // Define menu actions
  auto* menu = new QMenu(parentView());
  menu->setAttribute(Qt::WA_DeleteOnClose);
  menu->setMinimumWidth(width / 6);

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
    addSectionLabel(menu, "Edit");

    QAction* copyAction = menu->addAction(iconFromTheme("edit-copy"), "Copy");
    copyAction->setEnabled(items.size() > 0);
    QObject::connect(copyAction, &QAction::triggered, [this]() {
      copySelectedItems(nullptr);
    });

    QAction* pasteAction = menu->addAction(iconFromTheme("edit-paste", true), "Paste");
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    pasteAction->setEnabled(mimeData && mimeData->hasFormat(MAKI_CLIPBOARD_MIME));
    QObject::connect(pasteAction, &QAction::triggered, [this]() {
      pasteCopiedItems();
    });

    QAction* deleteAction = menu->addAction(iconFromTheme("edit-delete"), "Delete");
    deleteAction->setEnabled(items.size() > 0);
    QObject::connect(deleteAction, &QAction::triggered, [this]() {
      deleteSelectedItems();
    });

    // =============================================
    addSectionLabel(menu, "Visual");

    QAction* forwardAction = menu->addAction(iconFromTheme("object-order-front"), "To front");
    forwardAction->setEnabled(items.size() > 0);
    QObject::connect(forwardAction, &QAction::triggered, [this, items]() {
      qreal topZLevel = 0;
      for (QGraphicsItem* item : this->items())
        topZLevel = qMax(item->zValue(), topZLevel);

      for (NodeItem* node : items)
        node->setZValue(topZLevel);
    });

    QAction* backwardAction = menu->addAction(iconFromTheme("object-order-back"), "To back");
    backwardAction->setEnabled(items.size() > 0);
    QObject::connect(backwardAction, &QAction::triggered, [this, items]() {
      qreal topZLevel = 0;
      for (QGraphicsItem* item : this->items())
        topZLevel = qMin(item->zValue(), topZLevel);

      for (NodeItem* node : items)
        node->setZValue(topZLevel);
    });

    QAction* toggleLabelAction = menu->addAction(iconFromTheme("view-visible"), "Toggle label");
    toggleLabelAction->setEnabled(items.size() > 0);
    QObject::connect(toggleLabelAction, &QAction::triggered, [items]() {
      for (QGraphicsItem* item : items)
      {
        if (item->type() == NodeItem::Type)
          dynamic_cast<NodeItem*>(item)->toggleLabelVisibility();
      }
    });

    QMenu* alignMenu = menu->addMenu(iconFromTheme("align-on-canvas"), tr("Align"));
    createAlignMenu(alignMenu, itemIds);
  }
  else if (item->type() == NodeItem::Type || item->type() == QGraphicsSvgItem::Type)
  {
    NodeItem* node = static_cast<NodeItem*>(item->type() == NodeItem::Type ? item : item->parentItem());

    // =============================================
    if (type() == Types::LibraryTypes::STRUCTURAL)
    {
      addSectionLabel(menu, "Creation");

      QAction* newEventAction = menu->addAction(iconFromTheme("flow"), tr("New flow"));
      newEventAction->setEnabled(node != nullptr || items.size() > 0);
      QObject::connect(newEventAction, &QAction::triggered, [this, node]() {
        emit createEvent(node);
      });
      menu->addAction(newEventAction);
    }

    // =============================================
    addSectionLabel(menu, "Edit");

    QAction* copyAction = menu->addAction(iconFromTheme("edit-copy"), "Copy");
    copyAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(copyAction, &QAction::triggered, [this, node]() {
      copySelectedItems(node);
    });

    QAction* pasteAction = menu->addAction(iconFromTheme("edit-paste"), "Paste");
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    pasteAction->setEnabled(mimeData && mimeData->hasFormat(MAKI_CLIPBOARD_MIME));
    QObject::connect(pasteAction, &QAction::triggered, [this]() {
      pasteCopiedItems();
    });

    QAction* deleteAction = menu->addAction(iconFromTheme("edit-delete"), "Delete");
    deleteAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(deleteAction, &QAction::triggered, [this]() {
      deleteSelectedItems();
    });

    // =============================================
    addSectionLabel(menu, "Visual");

    QAction* forwardAction = menu->addAction(iconFromTheme("object-order-front"), "To front");
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

    QAction* backwardAction = menu->addAction(iconFromTheme("object-order-back"), "To back");
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

    QAction* toggleLabelAction = menu->addAction(iconFromTheme("view-visible"), "Toggle label");
    toggleLabelAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(toggleLabelAction, &QAction::triggered, [items]() {
      for (QGraphicsItem* item : items)
      {
        if (item->type() == NodeItem::Type)
          dynamic_cast<NodeItem*>(item)->toggleLabelVisibility();
      }
    });

    QMenu* alignMenu = menu->addMenu(iconFromTheme("align-on-canvas"), tr("Align"));
    createAlignMenu(alignMenu, itemIds);
  }
  else if (item->type() == TransitionItem::Type)
  {
    // =============================================
    addSectionLabel(menu, "Visual");

    // TransitionItem* transition = static_cast<TransitionItem*>(item);

    QAction* toggleLabelAction = menu->addAction(iconFromTheme("view-visible"), "Toggle label");
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
  menu->popup(event->screenPos());
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

      // Deleting a subflow container deletes its owning Repeat/Within instead.
      if (node->isSubflowContainer())
      {
        NodeItem* owner = node->subflowHost();
        if (!owner)
          continue;
        node = owner;
      }

      NodeItem* parent = node->parentNode();

      // Only delete if no parent OR parent is not selected
      if ((!parent || !parent->isSelected()) && !nodesToDelete.contains(node))
        nodesToDelete.append(node);
    }
    else if (item->type() == TransitionItem::Type)
    {
      TransitionItem* transition = static_cast<TransitionItem*>(item);
      if (!(transition->source() && transition->source()->isSelected()) &&
          !(transition->destination() && transition->destination()->isSelected()))
      {
        removeTransition(transition);
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
    triggerNodeRemoval(node->saveInfo());

  mUndoStack->endMacro();
}

void Canvas::triggerNodeRemoval(const NodeSaveInfo& nodeInfo)
{
  // We have to remember that QT QUndoCommands trigger the 'redo' method after creation. Thus, the node
  // removal is not explicit (as I would like) but happens through the RemoveNodeCommand below
  LOG_INFO("Removing node: %s", qPrintable(nodeInfo.getnodeId()));
  mUndoStack->push(new RemoveNodeCommand(this, nodeInfo));
}

void Canvas::removeNode(const NodeSaveInfo info)
{
  const QString nodeId = info.getid();
  if (!findNodeWithId(nodeId))
    return;

  // Look up by id when the timer fires — a raw NodeItem* can already be gone if
  // another removal ran first (common with Repeat/Within + subflow teardown).
  QTimer::singleShot(0, this, [this, nodeId]() {
    NodeItem* node = findNodeWithId(nodeId);
    if (!node)
      return;

    // Freeze view paints for the entire teardown (removeItem + delete).
    const auto sceneViews = views();
    for (QGraphicsView* view : sceneViews)
      view->setUpdatesEnabled(false);

    mBulkRemoving = true;
    mPendingNodeRemoved.clear();
    mPendingFlowRemoved.clear();

    auto toRemove = removeNode(node);

    // Sweep hostless SubflowBlocks left behind by nested teardown edge cases.
    const QList<QGraphicsItem*> sceneSnapshot = items();
    for (QGraphicsItem* item : sceneSnapshot)
    {
      if (!item || item->type() != NodeItem::Type)
        continue;
      auto* candidate = static_cast<NodeItem*>(item);
      if (!candidate->isSubflowContainer() || candidate->subflowHost())
        continue;
      toRemove += removeNode(candidate);
    }

    // Delete children before parents; skip duplicates (nested removals can overlap).
    QSet<QGraphicsItem*> seen;
    for (int i = toRemove.size() - 1; i >= 0; --i)
    {
      QGraphicsItem* item = toRemove[i];
      if (!item || seen.contains(item))
        continue;
      seen.insert(item);

      if (auto* removedNode = dynamic_cast<NodeItem*>(item))
      {
        // Drop item caches before free — stale DeviceCoordinateCache is a known
        // source of effectiveBoundingRect crashes during the next view paint.
        removedNode->setCacheMode(QGraphicsItem::NoCache);
        LOG_DEBUG("Deleting: %s, has parent: %d", qPrintable(removedNode->id()), removedNode->parentNode() != nullptr);
      }

      delete item;
    }

    // Flush widget DeferredDeletes (e.g. properties panel) before painting again.
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

    const auto pendingNodes = mPendingNodeRemoved;
    const auto pendingFlows = mPendingFlowRemoved;
    mPendingNodeRemoved.clear();
    mPendingFlowRemoved.clear();
    mBulkRemoving = false;

    for (const auto& flow : pendingFlows)
      emit flowRemoved(flow.first, flow.second);
    for (const auto& removed : pendingNodes)
      emit nodeRemoved(removed.first, removed.second);

    for (QGraphicsView* view : sceneViews)
      view->setUpdatesEnabled(true);
  });
}

QVector<QGraphicsItem*> Canvas::cleanTransitionsOfNode(const QString& nodeId)
{
  LOG_INFO("Calling base class");
  // Do nothing
  return {};
}

void Canvas::onNodeMoved(const QString& nodeId)
{
  if (!mNodeActionMenu || !mNodeActionMenu->isVisible())
    return;

  if (mNodeActionMenu->trackedTaskId() != nodeId)
    return;

  if (CanvasView* view = parentView())
    mNodeActionMenu->updatePosition(view);

  // We should also check for dropping nodes on transitions here
  // Instead of creating, we would just update its transitions
}

QVector<QGraphicsItem*> Canvas::removeNode(NodeItem* node)
{
  if (!node)
    return {};

  // Subflow containers are owned by Repeat/Within — delete the host instead.
  // Orphan containers (host already gone) fall through and are removed directly.
  if (node->isSubflowContainer())
  {
    if (NodeItem* owner = node->subflowHost())
      return removeNode(owner);
  }

  // Detach owned subflow blocks so owner destructors will not touch them again.
  const QVector<NodeItem*> detachedBlocks = node->detachOwnedSubflowBlocks();

  removeItem(node);

  // Orphan container (host already gone). Safe to drop its links now that the
  // scene no longer indexes it by its old, connector-spanning boundingRect.
  if (auto* block = dynamic_cast<SubflowBlock*>(node))
    block->prepareForDeletion();

  // Clear any potential callback
  node->nodeModified = nullptr;
  node->flowAdded = nullptr;
  node->nodeMoved = nullptr;
  node->nodeHovered = nullptr;

  LOG_DEBUG("Removing node: %s %d", qPrintable(node->id()), (int)type());

  // Order is parent-then-descendants so the caller's reverse loop deletes
  // children before parents (avoids dangling parent pointers during paint).
  QVector<QGraphicsItem*> itemsToRemove;
  itemsToRemove.append(node);
  updateParent(node, nullptr, false);

  auto flows = node->flows();
  for (Flow* flow : flows)
  {
    node->deleteFlow(flow->id());
    if (mBulkRemoving)
      mPendingFlowRemoved.append({flow->id(), node->id()});
    else
      emit flowRemoved(flow->id(), node->id());
  }

  auto parent = node->parentNode();
  if (parent)
    parent->childRemoved(node);
  node->clearParentNode();

  itemsToRemove += cleanTransitionsOfNode(node->id());

  for (NodeItem* detachedBlock : detachedBlocks)
  {
    if (!detachedBlock)
      continue;

    itemsToRemove += cleanTransitionsOfNode(detachedBlock->id());

    detachedBlock->nodeModified = nullptr;
    detachedBlock->flowAdded = nullptr;
    detachedBlock->nodeMoved = nullptr;
    detachedBlock->nodeHovered = nullptr;

    if (detachedBlock->scene())
      removeItem(detachedBlock);

    if (auto* block = dynamic_cast<SubflowBlock*>(detachedBlock))
      block->prepareForDeletion();

    itemsToRemove.append(detachedBlock);
    mSelectedNodes.removeAll(detachedBlock);

    const QVector<NodeItem*> subflowChildren = detachedBlock->children();
    for (NodeItem* child : subflowChildren)
      itemsToRemove += removeNode(child);
  }

  auto toDelete = node->children();
  for (NodeItem* child : toDelete)
    itemsToRemove += removeNode(child);  // Do not create undo for children

  mSelectedNodes.removeAll(node);

  const QString parentId = parent ? parent->id() : QString();
  if (mBulkRemoving)
    mPendingNodeRemoved.append({node->id(), parentId});
  else
    emit nodeRemoved(node->id(), parentId);

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
  QList<CopiedNode> copiedNodes;

  const QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  auto copyNode = [&](NodeItem* node) {
    if (!node)
      return;

    auto info = node->saveInfo();

    LOG_DEBUG("Copied %s (%.2f %.2f)", qPrintable(node->id()), info.getposition().x(), info.getposition().y());

    copiedNodes.append({info, mousePosition - info.getposition()});
  };

  if (clickedNode)
  {
    copyNode(clickedNode);
  }
  else
  {
    for (QGraphicsItem* item : selectedItems())
    {
      if (item->type() != NodeItem::Type)
        continue;

      auto* node = static_cast<NodeItem*>(item);

      // Do not copy children if their parent is already copied.
      if (isParentSelected(node))
        continue;

      copyNode(node);
      selectNode(node, false);
    }
  }

  if (copiedNodes.isEmpty())
    return;

  QByteArray data;
  QDataStream out(&data, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_6_0);
  out << copiedNodes;

  auto* mimeData = new QMimeData();
  mimeData->setData(MAKI_CLIPBOARD_MIME, data);

  // Also copy as json so it can be easily transferred
  QJsonArray text;
  for (const auto& node : copiedNodes)
    text.append(node.toJson());

  mimeData->setText(QJsonDocument(text).toJson(QJsonDocument::Indented));

  QApplication::clipboard()->setMimeData(mimeData);
}

void Canvas::pasteCopiedItems(const QPointF& mousePosition, NodeItem* parentNode,
                              QList<CopiedNode> copiedNodes, bool absolute)
{
  for (const auto& copy : copiedNodes)
  {
    auto infoPtr = std::make_shared<NodeSaveInfo>(copy.info);

    QPointF newParentPosition = {0.0, 0.0};
    if (parentNode)
      newParentPosition = parentNode->saveInfo().getposition();

    auto node = createNode(NodeCreation::Pasting,
                           infoPtr,
                           absolute
                               ? mousePosition - copy.posRelativeToMouse
                               : newParentPosition + (copy.info.getposition() - copy.posRelativeToMouse),
                           parentNode);

    QList<CopiedNode> children;
    for (const auto& child : copy.info.getchildren())
      children.push_back({*std::dynamic_pointer_cast<NodeSaveInfo>(child), copy.info.getposition()});

    pasteCopiedItems(mousePosition, node, children, false);
    selectNode(node, false);
  }

  if (parentNode)
    parentNode->expandSubflowToFitChildren();
}

void Canvas::pasteCopiedItems()
{
  const QMimeData* mimeData = QApplication::clipboard()->mimeData();
  if (!mimeData || !mimeData->hasFormat(MAKI_CLIPBOARD_MIME))
    return;

  QByteArray data = mimeData->data(MAKI_CLIPBOARD_MIME);

  QList<CopiedNode> copiedNodes;
  QDataStream in(&data, QIODevice::ReadOnly);
  in.setVersion(QDataStream::Qt_6_0);
  in >> copiedNodes;

  if (copiedNodes.isEmpty())
    return;

  const QPointF mousePosition = parentView()->mapToScene(parentView()->mapFromGlobal(QCursor::pos()));

  NodeItem* parentNode = dropTargetContainer(itemAt(mousePosition, QTransform()), type(), this, mousePosition);
  if (!parentNode)
  {
    QGraphicsItem* item = itemAt(mousePosition, QTransform());
    if (item && item->type() == NodeItem::Type)
    {
      parentNode = static_cast<NodeItem*>(item);
      if (!parentNode->acceptDrops())
        return;
    }
  }

  pasteCopiedItems(mousePosition, parentNode, copiedNodes, true);
}

void Canvas::clearCanvas()
{
  const auto sceneViews = views();
  for (QGraphicsView* view : sceneViews)
    view->setUpdatesEnabled(false);

  mBulkRemoving = true;
  mPendingNodeRemoved.clear();
  mPendingFlowRemoved.clear();

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
    if (node->isSubflowContainer())
      continue;

    LOG_INFO("Removing node: %s", qPrintable(node->id()));
    toRemove += removeNode(node);
  }

  QSet<QGraphicsItem*> seen;
  for (QGraphicsItem* item : toRemove)
  {
    if (!item || seen.contains(item))
      continue;
    seen.insert(item);
    delete item;
  }

  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

  const auto pendingNodes = mPendingNodeRemoved;
  const auto pendingFlows = mPendingFlowRemoved;
  mPendingNodeRemoved.clear();
  mPendingFlowRemoved.clear();
  mBulkRemoving = false;

  for (const auto& flow : pendingFlows)
    emit flowRemoved(flow.first, flow.second);
  for (const auto& removed : pendingNodes)
    emit nodeRemoved(removed.first, removed.second);

  for (QGraphicsView* view : sceneViews)
    view->setUpdatesEnabled(true);

  LOG_DEBUG("Number of items after clearCanvas: %d", items().size());
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
    if (!nodeInfo)
      return VoidResult::Failed("Save is corrupt");

    auto node = std::make_shared<NodeSaveInfo>(*nodeInfo);

    LOG_DEBUG("Creating node %s with parent %s", qPrintable(node->getid()), qPrintable(node->getparentId()));
    auto createdNode = createNode(NodeCreation::Loading, node, node->getposition(), parent);
    if (!createdNode)
      return VoidResult::Failed("Failed to load node: " + node->getnodeId().toStdString());

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

  LOG_DEBUG("Loading canvas from save with %d nodes", info.getnodes().size());

  mDeferStructuralLayout = type() == Types::LibraryTypes::STRUCTURAL;
  const VoidResult ret = loadFromSave(info.getnodes(), nullptr);
  mDeferStructuralLayout = false;

  if (ret.IsSuccess())
    finalizeAfterLoad();

  return ret;
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

void Canvas::swapCapabilities(const QString& parentId, const QString& aId, const QString& bId)
{
  NodeItem* parentNode = findNodeWithId(parentId);
  NodeItem* aNode = findNodeWithId(aId);
  NodeItem* bNode = findNodeWithId(bId);
  if (parentNode && aNode && bNode)
    parentNode->swapCapabilityOrder(aNode, bNode);
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

NodeItem* Canvas::createNode(NodeCreation creation, std::shared_ptr<NodeSaveInfo> info,
                             const QPointF& position, NodeItem* parent)
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
  NodeItem* node = NodeFactory::create(nodeId, info, position, config);

  if (parent != nullptr)
  {
    node->addParent(parent);
    parent->addChild(node, info);
  }

  // TODO(felaze): Move these to a function or so
  node->nodeModified = [this](NodeItem* item) { emit nodeModified(item); };
  node->flowAdded = [this](Flow* flow, NodeItem* node) { addedItemFlow(flow, node); };
  node->nodeMoved = [this](const QString& id) { onNodeMoved(id); };
  node->nodeHovered = [this](NodeItem* item, bool hovered) { onNodeHovered(item, hovered); };

  node->start();

  // All nodes are children of the canvas
  addItem(node);

  if (creation != NodeCreation::Populating)
    updateParent(node, info, true);

  addedItemNode(node, info);

  // New tasks always get a Koda "main" entry flow. Skip Loading: saved flows
  // are attached afterwards and must not be duplicated.
  if (creation != NodeCreation::Loading && node->isTaskContainer())
    node->ensureMainFlow();

  if (creation != NodeCreation::Populating)
    mUndoStack->push(new AddNodeCommand(this, node->saveInfo()));

  return node;
}

PortItem* Canvas::portNear(const QPointF& scenePos, int kind, qreal radius) const
{
  const QRectF searchRect(scenePos - QPointF(radius, radius), QSizeF(radius * 2.0, radius * 2.0));
  const qreal r2 = radius * radius;

  PortItem* best = nullptr;
  qreal bestD2 = r2;
  const QList<QGraphicsItem*> hits = items(searchRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
  for (QGraphicsItem* it : hits)
  {
    if (!it || it->type() != Types::PORT)
      continue;

    auto* p = static_cast<PortItem*>(it);
    if (p->kind() != static_cast<PortItem::Kind>(kind))
      continue;

    const QPointF d = p->anchorScenePos() - scenePos;
    const qreal d2 = d.x() * d.x() + d.y() * d.y();
    if (d2 <= bestD2)
    {
      bestD2 = d2;
      best = p;
    }
  }
  return best;
}

PortItem* Canvas::portNearOutgoing(const QPointF& scenePos, qreal radius) const
{
  const QRectF searchRect(scenePos - QPointF(radius, radius), QSizeF(radius * 2.0, radius * 2.0));
  const qreal r2 = radius * radius;

  PortItem* best = nullptr;
  qreal bestD2 = r2;
  const QList<QGraphicsItem*> hits = items(searchRect, Qt::IntersectsItemShape, Qt::DescendingOrder);
  for (QGraphicsItem* it : hits)
  {
    if (!it || it->type() != Types::PORT)
      continue;
    auto* p = static_cast<PortItem*>(it);
    if (!p->isOutgoing())
      continue;
    const QPointF d = p->anchorScenePos() - scenePos;
    const qreal d2 = d.x() * d.x() + d.y() * d.y();
    if (d2 <= bestD2)
    {
      bestD2 = d2;
      best = p;
    }
  }
  return best;
}

PortItem* Canvas::portClosestOnNode(const NodeItem* node, const QPointF& scenePos, qreal radius) const
{
  if (!node)
    return nullptr;

  const qreal r2 = radius * radius;
  PortItem* best = nullptr;
  qreal bestD2 = r2;

  for (QGraphicsItem* child : node->childItems())
  {
    if (!child || child->type() != Types::PORT)
      continue;

    auto* p = static_cast<PortItem*>(child);
    const QPointF d = p->anchorScenePos() - scenePos;
    const qreal d2 = d.x() * d.x() + d.y() * d.y();
    if (d2 <= bestD2)
    {
      bestD2 = d2;
      best = p;
    }
  }
  return best;
}

bool Canvas::canCompleteTransitionTo(NodeItem* dest, const QPointF& scenePos) const
{
  if (!dest || !dest->config() || !dest->config()->hasInPort)
    return false;

  if (PortItem* closest = portClosestOnNode(dest, scenePos, PortItem::kHitPadding))
  {
    if (closest->isOutgoing())
      return false;
  }

  return true;
}

bool Canvas::beginTransitionFromOutPort(PortItem* port, const QPointF& cursorScenePos)
{
  if (!port || !port->isOutgoing())
    return false;

  NodeItem* node = port->nodeItem();
  if (!node || !canAddTransition(node, port))
    return false;

  mNode = node;
  mTransition = new TransitionItem(std::make_shared<TransitionSaveInfo>());
  mTransition->setZValue(node->zValue() - 1);
  LOG_INFO("Node: %s ZValue: %f %f", qPrintable(node->nodeId()), node->zValue(), mTransition->zValue());

  TransitionConfig config;
  if (node->isSubflowContainer())
  {
    config.event = QString();
    config.label = QString();
    config.modifiable = false;
  }
  else if (port->kind() == PortItem::Out)
    config = nextTransition(node);
  else
  {
    config.event = port->defaultTransitionEvent();
    config.label = port->defaultTransitionLabel();
    config.modifiable = false;
  }
  mTransition->setEvent(config.event);
  mTransition->setName(config.label);
  mTransition->setStart(node->id(), port->anchorScenePos(), {0, 0});
  mTransition->setEnd(Constants::TMP_CONNECTION_ID, cursorScenePos, {0, 0});

  addItem(mTransition);
  parentView()->setDragMode(QGraphicsView::NoDrag);
  return true;
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

void Canvas::onFocusNode(const QString& flowId, const QString& nodeId)
{
  // If no flow id was provided, then we are already in the right canvas, or in the structural canvas
  if (flowId.isEmpty())
  {
    auto node = findNodeWithId(nodeId);
    if (!node)
      return;

    parentView()->zoom(2 * node->baseScale() / parentView()->getScale());

    // Center the node in the view
    parentView()->centerOn(node);
    selectNode(node, true);
  }
  else
  {
    // If we have a flow id, we need to find that flow and the node inside it
    for (const auto& item : items())
    {
      if (item->type() != NodeItem::Type)
        continue;

      auto node = static_cast<NodeItem*>(item);
      const auto flow = node->getFlow(flowId);
      if (!flow)
        continue;

      for (const auto& child : flow->getNodes())
        if (child->getid() == nodeId)
        {
          emit openFlow(flow, nodeId);
          return;
        }
    }
  }
}

void Canvas::onRemoveNode(const QString& flowId, const QString& nodeId)
{
}

// ==========================================================================================
// Flow
void Canvas::populate(const FlowSaveInfo& flow)
{
  // First create all the nodes
  for (const auto& inode : flow.getnodes())
  {
    auto node = std::dynamic_pointer_cast<NodeSaveInfo>(inode);
    // LOG_DEBUG("Creating behavioral node %s with parent \"%s\"", qPrintable(node->getid()), qPrintable(node->getparentId()));
    auto created = createNode(NodeCreation::Populating, node, node->getposition(), findNodeWithId(node->getparentId()));
    if (created)
      LOG_DEBUG("Created node %s %s", qPrintable(node->getid()), qPrintable(created->id()));
  }

  // Then create the transitions between the nodes
  for (std::shared_ptr<ITransition> itransition : flow.gettransitions())
  {
    auto transition = std::dynamic_pointer_cast<TransitionSaveInfo>(itransition);
    auto srcConn = findNodeWithId(transition->getsrcId());
    if (!srcConn)
    {
      LOG_WARNING("Could not find source node");
      continue;
    }

    auto dstConn = findNodeWithId(transition->getdstId());
    if (!dstConn)
    {
      LOG_WARNING("Could not find destination node");
      continue;
    }

    LOG_DEBUG("Creating transitions %s -> %s", qPrintable(transition->getsrcId()), qPrintable(transition->getdstId()));

    auto connection = new TransitionItem(transition);

    connection->setStart(transition->getsrcId(), transition->srcPoint(), transition->srcShift());
    connection->setEnd(transition->getdstId(), transition->dstPoint(), transition->dstShift());

    addTransition(connection);
    addItem(connection);

    // First add the transition to the canvas and then mark it as done
    connection->done(srcConn, dstConn);
  }
}

void Canvas::onFlowSelected(const QString& flowId, const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
  {
    LOG_WARNING("Flow %s is not tied to any nodes", qPrintable(flowId));
    return;
  }

  auto flow = node->getFlow(flowId);
  emit openFlow(flow, "");
}

void Canvas::onCreateFlow(const QString& nodeId, std::shared_ptr<FlowSaveInfo> info)
{
  if (!info)
    return;

  auto node = findNodeWithId(nodeId);
  if (!node)
  {
    LOG_WARNING("Cannot create flow: task node %s not found", qPrintable(nodeId));
    return;
  }

  Flow* flow = node->createFlow(info->getname(), info);
  if (!flow)
    return;

  emit openFlow(flow, "");
}

void Canvas::onOpenFlowCallTarget(const QString& taskNodeId, const QString& flowName)
{
  auto node = findNodeWithId(taskNodeId);
  if (!node)
  {
    LOG_WARNING("Cannot open flow call target: task node %s not found", qPrintable(taskNodeId));
    return;
  }

  for (Flow* flow : node->flows())
  {
    if (flow->name() == flowName)
    {
      emit openFlow(flow, "");
      return;
    }
  }

  LOG_WARNING("Cannot open flow call target: flow %s not found on node %s", qPrintable(flowName), qPrintable(taskNodeId));
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

NodeItem* Canvas::insertDroppedNodeOnTransition(TransitionItem* /* transition */, std::shared_ptr<NodeSaveInfo> /* info */)
{
  return nullptr;
}

bool Canvas::canConnectNodes(NodeItem* /* source */, NodeItem* /* dest */) const
{
  return true;
}

TransitionItem* Canvas::transitionAt(const QPointF& scenePos) const
{
  const QList<QGraphicsItem*> hits = items(scenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);
  for (QGraphicsItem* item : hits)
  {
    if (item->type() == NodeItem::Type)
    {
      // SubflowBlock body is not an obstacle for drop-on-transition hit testing.
      if (static_cast<NodeItem*>(item)->isSubflowContainer())
        continue;
      return nullptr;
    }

    if (item->type() == TransitionItem::Type)
      return static_cast<TransitionItem*>(item);

    for (QGraphicsItem* parent = item->parentItem(); parent; parent = parent->parentItem())
    {
      if (parent->type() == TransitionItem::Type)
        return static_cast<TransitionItem*>(parent);
    }
  }

  return nullptr;
}

void Canvas::themeChanged()
{
  for (QGraphicsItem* item : items())
  {
    if (item->type() == TransitionItem::Type)
      qgraphicsitem_cast<TransitionItem*>(item)->updatePath();

    item->update();
  }
}

void Canvas::autoRoute()
{
  if (!router())
    return;

  QList<NodeItem*> nodes;
  QList<TransitionItem*> transitions;
  for (const auto& item : items())
  {
    if (item->type() == NodeItem::Type)
      nodes.push_back(qgraphicsitem_cast<NodeItem*>(item));
    else if (item->type() == TransitionItem::Type)
      transitions.push_back(qgraphicsitem_cast<TransitionItem*>(item));
  }

  const auto paths = router()->route(nodes, transitions);
  for (TransitionItem* transition : transitions)
  {
    if (paths.contains(transition))
      transition->updatePath(paths.value(transition));
  }
}
