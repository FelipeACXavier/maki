#include "canvas.h"

#include <qgraphicsitem.h>
#include <qgridlayout.h>

#include <QBuffer>
#include <QClipboard>
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
#include <QScrollArea>
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
#include "elements/flow.h"
#include "elements/node.h"
#include "elements/port.h"
#include "elements/transition.h"
#include "flow_info.h"
#include "logging.h"
#include "node_info.h"
#include "result.h"
#include "save_info.h"
#include "undo_commands/add_node.h"
#include "undo_commands/align.h"
#include "undo_commands/remove_node.h"
#include "widgets/structure/node_action_menu.h"
#include "widgets/widget_factory.h"

namespace
{
NodeItem* taskContainerAcceptingDrop(QGraphicsItem* item)
{
  while (item)
  {
    if (item->type() == NodeItem::Type)
    {
      auto* node = static_cast<NodeItem*>(item);
      for (NodeItem* cur = node; cur; cur = cur->parentNode())
      {
        if (cur->isTaskContainer() && cur->acceptDrops())
          return cur;
      }
      return nullptr;
    }
    item = item->parentItem();
  }
  return nullptr;
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
  // setItemIndexMethod(ItemIndexMethod::NoIndex);

  // mHoverTimer = new QTimer(this);
  // mHoverTimer->setSingleShot(true);

  mUndoStack = new QUndoStack(this);
  mUndoStack->setUndoLimit(20);

  connect(this, &QGraphicsScene::selectionChanged, this, &Canvas::onSelectionChanged);
}

Canvas::~Canvas()
{
  disconnect(this, &QGraphicsScene::selectionChanged, this, &Canvas::onSelectionChanged);
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
    NodeItem* parentNode = taskContainerAcceptingDrop(itemAt(event->scenePos(), QTransform()));

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
          event->acceptProposedAction();
          dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::ArrowCursor);
          return;
        }
      }
    }

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
  if (!view)
    return;

  QMenu menu(view);

  using CapRow = std::pair<QString, std::shared_ptr<NodeConfig>>;
  std::map<QString, QVector<CapRow>> byLibrary;

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

    byLibrary[libName].push_back({key, cfg});
  }

  for (auto it = byLibrary.begin(); it != byLibrary.end(); ++it)
  {
    std::sort(it->second.begin(), it->second.end(), [](const CapRow& a, const CapRow& b) {
      return a.second->type < b.second->type;
    });
  }

  struct CapabilityTile
  {
    QString name;
    QWidget* cell = nullptr;
  };

  struct LibrarySection
  {
    QLabel* header = nullptr;
    QWidget* gridHost = nullptr;
    QGridLayout* grid = nullptr;
    QVector<CapabilityTile> tiles;
  };

  QVector<LibrarySection> sections;

  // This should be it't own class. I think it would be quite simple to implement it as a child of LibraryContainer
  // or something similar to it. It already puts the nodes in a grid like layout and it is quite easy to configure.
  // We would only need to put the searchwidget above it
  // I also think it might make sense to construct this at the system level since it is always the same, it might take
  // some more ram, but at least we don't need to populate the container everytime.
  auto* host = new QWidget(&menu);
  oclero::qlementine::QlementineStyle::setAutoIconColor(host, oclero::qlementine::AutoIconColor::None);
  auto* vbox = new QVBoxLayout(host);
  vbox->setContentsMargins(6, 4, 6, 4);
  vbox->setSpacing(4);

  auto* search = new maki::SearchWidget(tr("Filter capabilities"), host);
  vbox->addWidget(search);

  constexpr int kCols = 5;
  constexpr int kIconPx = 28;
  constexpr int kIconBtnPx = 36;
  constexpr int kMinTileW = 72;
  constexpr int kMaxTileW = 110;
  constexpr int kTileHPadding = 8;

  const QFontMetrics fm(host->font());
  int widest = kMinTileW - kTileHPadding;
  for (const auto& libIt : byLibrary)
  {
    for (const CapRow& row : libIt.second)
    {
      const int w = fm.horizontalAdvance(row.second->type);
      widest = qMax(widest, w);
    }
  }
  const int tileW = qBound(kMinTileW, widest + kTileHPadding, kMaxTileW);

  for (auto libIt = byLibrary.begin(); libIt != byLibrary.end(); ++libIt)
  {
    LibrarySection section;
    section.header = new QLabel(libIt->first, host);
    QFont hf = section.header->font();
    hf.setBold(true);
    section.header->setFont(hf);
    vbox->addWidget(section.header);

    section.gridHost = new QWidget(host);
    section.grid = new QGridLayout(section.gridHost);
    section.grid->setContentsMargins(0, 0, 0, 0);
    section.grid->setHorizontalSpacing(4);
    section.grid->setVerticalSpacing(4);

    int idx = 0;
    for (const CapRow& row : libIt->second)
    {
      const QString nodeIdKey = row.first;
      const std::shared_ptr<NodeConfig> cfgRow = row.second;

      auto* cell = new QWidget(section.gridHost);
      cell->setFixedWidth(tileW);
      auto* cv = new QVBoxLayout(cell);
      cv->setContentsMargins(0, 0, 0, 0);
      cv->setSpacing(2);

      auto* btn = new QToolButton(cell);
      btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
      btn->setAutoRaise(true);
      btn->setIconSize(QSize(kIconPx, kIconPx));
      btn->setFixedSize(QSize(kIconBtnPx, kIconBtnPx));
      if (!cfgRow->body.iconPath.isEmpty())
      {
        const QString path = AppPaths::icon(cfgRow->body.iconPath);
        if (!path.isEmpty())
          btn->setIcon(QIcon(path));
      }
      cv->addWidget(btn, 0, Qt::AlignHCenter);

      auto* lbl = new QLabel(cfgRow->type, cell);
      QFont lf = lbl->font();
      lf.setPointSizeF(qMax(7.0, lf.pointSizeF() - 1.0));
      lbl->setFont(lf);
      lbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
      lbl->setWordWrap(true);
      lbl->setTextInteractionFlags(Qt::NoTextInteraction);
      lbl->setFixedWidth(tileW - kTileHPadding);
      lbl->setToolTip(cfgRow->type);
      cv->addWidget(lbl, 0, Qt::AlignHCenter);

      QObject::connect(btn, &QToolButton::clicked, view, [this, task, menuPtr = &menu, nodeIdKey, cfgRow]() {
        auto info = std::make_shared<NodeSaveInfo>();
        info->setNodeId(nodeIdKey);
        info->setSize(QSizeF(cfgRow->body.width, cfgRow->body.height));
        info->setScale(parentView()->getScale());

        clearSelectedNodes();
        const QPointF slotCenter = task->placeholderSlotSceneRect().center();
        NodeItem* created = createNode(NodeCreation::Dropping, info, slotCenter, task);
        if (created)
          selectNode(created, true);

        menuPtr->close();
      });

      section.grid->addWidget(cell, idx / kCols, idx % kCols);
      section.tiles.push_back({cfgRow->type, cell});
      ++idx;
    }

    vbox->addWidget(section.gridHost);
    sections.push_back(section);
  }

  const auto reflowSection = [](LibrarySection& section, const QVector<QWidget*>& visibleCells) {
    while (QLayoutItem* item = section.grid->takeAt(0))
    {
      if (item->widget())
        item->widget()->setParent(section.gridHost);
      delete item;
    }

    for (int i = 0; i < visibleCells.size(); ++i)
      section.grid->addWidget(visibleCells[i], i / kCols, i % kCols);

    const bool hasVisible = !visibleCells.isEmpty();
    section.header->setVisible(hasVisible);
    section.gridHost->setVisible(hasVisible);
  };

  const auto applyFilter = [reflowSection, &sections](const QString& query) {
    const QString needle = query.trimmed();
    for (LibrarySection& section : sections)
    {
      QVector<QWidget*> visible;
      visible.reserve(section.tiles.size());
      for (const CapabilityTile& tile : section.tiles)
      {
        const bool matches = needle.isEmpty() || tile.name.contains(needle, Qt::CaseInsensitive);
        tile.cell->setVisible(matches);
        if (matches)
          visible.push_back(tile.cell);
      }
      reflowSection(section, visible);
    }
  };

  QObject::connect(search, &maki::SearchWidget::valueChanged, host, applyFilter);

  host->adjustSize();

  auto* scroll = new QScrollArea(&menu);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setWidgetResizable(true);
  scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scroll->setWidget(host);

  const int viewportH = view->viewport() ? view->viewport()->height() : view->height();
  const int maxPopupH = qMax(240, static_cast<int>(viewportH * 0.7));

  const QSize hostHint = host->sizeHint();
  scroll->setMaximumHeight(qMin(hostHint.height() + 2, maxPopupH));
  scroll->setMinimumWidth(hostHint.width() + 2);

  auto* wa = new QWidgetAction(&menu);
  wa->setDefaultWidget(scroll);
  menu.addAction(wa);

  const QRectF slotRect = task->placeholderSlotSceneRect();
  const QPointF anchorScene = slotRect.center() + QPointF(0, slotRect.height() * 0.5 + 4.0);

  menu.ensurePolished();
  const QSize menuHint = menu.sizeHint();

  QPoint v = view->mapFromScene(anchorScene);
  v.rx() -= menuHint.width() / 2;
  QTimer::singleShot(0, search, [search]() { search->widget()->setFocus(); });
  menu.exec(view->viewport()->mapToGlobal(v));
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
      if (dest && mNode && dest != mNode && dest->type() == NodeItem::Type && canCompleteTransitionTo(dest, event->scenePos()))
      {
        const QPointF endPos = dest->incomingPortAnchor();
        mTransition->setEnd(dest->id(), endPos, {0, 0});
        mTransition->done(mNode, dest);
        addTransition(mTransition);
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

        qDebug() << "clicked item" << item
                 << "type" << item->type()
                 << "br" << item->boundingRect()
                 << "scene br" << item->sceneBoundingRect()
                 << "shape scene"
                 << item->mapToScene(item->shape()).boundingRect();
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
    addSectionLabel(menu, "Creation");

    QAction* newEventAction = menu->addAction(iconFromTheme("flow"), tr("New flow"));
    newEventAction->setEnabled(node != nullptr || items.size() > 0);
    QObject::connect(newEventAction, &QAction::triggered, [this, node]() {
      emit createEvent(node);
    });
    menu->addAction(newEventAction);

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
      NodeItem* parent = static_cast<NodeItem*>(node->parentNode());

      // Only delete if no parent OR parent is not selected
      if (!parent || !parent->isSelected())
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
  auto node = findNodeWithId(info.getid());
  if (!node)
    return;

  QTimer::singleShot(0, this, [this, node]() {
    auto toRemove = removeNode(node);

    // Delete children before parents (important if parent owns child QGraphicsItems)
    for (int i = toRemove.size() - 1; i >= 0; --i)
    {
      if (auto node = dynamic_cast<NodeItem*>(toRemove[i]))
        LOG_DEBUG("Deleting: %s, has parent: %d", qPrintable(node->id()), node->parentNode() != nullptr);

      delete toRemove[i];
    }
  });
}

QVector<QGraphicsItem*> Canvas::cleanTransitionsOfNode(const QString& nodeId)
{
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

  removeItem(node);

  // Clear any potential callback
  node->nodeModified = nullptr;
  node->flowAdded = nullptr;
  node->nodeMoved = nullptr;

  LOG_TRACE("Removing node: %s", qPrintable(node->id()));

  QVector<QGraphicsItem*> itemsToRemove = {node};
  updateParent(node, nullptr, false);

  auto flows = node->flows();
  for (Flow* flow : flows)
  {
    node->deleteFlow(flow->id());
    emit flowRemoved(flow->id(), node->id());
  }

  auto parent = node->parentNode();
  if (parent)
    parent->childRemoved(node);

  itemsToRemove += cleanTransitionsOfNode(node->id());

  auto toDelete = node->children();
  for (NodeItem* child : toDelete)
    itemsToRemove += removeNode(child);  // Do not create undo for children

  mSelectedNodes.removeAll(node);

  emit nodeRemoved(node->id(), parent ? parent->id() : "");

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
                                node->setSelected(bool selectedtrue)                              QList<CopiedNode> copiedNodes, bool absolute);
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

  NodeItem* parentNode = nullptr;
  QGraphicsItem* item = itemAt(mousePosition, QTransform());
  if (item && item->type() == NodeItem::Type)
  {
    parentNode = static_cast<NodeItem*>(item);

    if (!parentNode->acceptDrops())
      return;
  }

  pasteCopiedItems(mousePosition, parentNode, copiedNodes, true);
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

    toRemove += removeNode(node);
  }

  QTimer::singleShot(0, this, [toRemove]() {
    for (QGraphicsItem* item : toRemove)
      delete item;
  });

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
  NodeItem* node = new NodeItem(nodeId, info, position, config);

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
  if (port->kind() == PortItem::Out)
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
      LOG_DEBUG("Created node %s", qPrintable(created->id()));
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

TransitionItem* Canvas::transitionAt(const QPointF& scenePos) const
{
  const QList<QGraphicsItem*> hits = items(scenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);
  for (QGraphicsItem* item : hits)
  {
    if (item->type() == NodeItem::Type)
      return nullptr;

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
