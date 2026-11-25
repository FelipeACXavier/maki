#include "task_node.h"

#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "canvas/canvas_view.h"
#include "canvas/elements/flow.h"
#include "capability_node.h"
#include "document/node_info.h"
#include "document/save_info.h"
#include "logging.h"
#include "macros.h"
#include "settings.h"
#include "widgets/controls/capability_selector.h"

static constexpr qreal TASK_WIDTH = 324.0;
static constexpr qreal TASK_HEIGHT = 300.0;
static constexpr qreal TASK_ASPECT_RATIO = TASK_WIDTH / TASK_HEIGHT;
static constexpr qreal SLOT_DIAMETER_FACTOR = 0.25;
static constexpr qreal MARGIN = 16;

static constexpr qreal defaultSlotDiameter()
{
  return (qMin(TASK_WIDTH, TASK_HEIGHT) - 2.0 * MARGIN) * SLOT_DIAMETER_FACTOR;
}

TaskNode::TaskNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                   QGraphicsItem* parent)
    : NodeItem(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  setAcceptHoverEvents(true);
  mEmptySlot = new EmptySlot(0, this);
  mEmptySlot->clicked = [this](const QPointF& scenePos) {
    if (nodeControlRequested)
      nodeControlRequested(this, scenePos, CapabilitySelector::create());
  };
}

TaskNode::~TaskNode()
{
}

void TaskNode::initializeNodeSize()
{
  mSize = QSizeF(mSize.width(), mSize.width() / TASK_ASPECT_RATIO);
  mStorage->setSize(mSize);
}

VoidResult TaskNode::start()
{
  RETURN_ON_FAILURE(NodeItem::start());

  ensureMainFlowExists();
  relayoutCapabilitySlots();

  return VoidResult();
}

// ─────────────────────────────────────────────────────────────────────────────
// paint / shape
// ─────────────────────────────────────────────────────────────────────────────

void TaskNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  oclero::qlementine::Theme theme;
  if (auto qlementinestyle = oclero::qlementine::appStyle())
    theme = qlementinestyle->theme();

  const auto background = theme.backgroundColorMain1;
  const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.5 / baseScale());

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(pen);
  painter->setBrush(background);

  const QRectF bodyRect = nodeRect().adjusted(theme.spacing, theme.spacing, -theme.spacing, -theme.spacing);
  painter->drawRoundedRect(bodyRect, 2 * theme.borderRadius, 2 * theme.borderRadius);

  NodeBase::paintLabel(painter, labelBoundingRect(), pen);
}

// ─────────────────────────────────────────────────────────────────────────────
// Parent / child management
// ─────────────────────────────────────────────────────────────────────────────
void TaskNode::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  NodeItem::addChild(node, info);
  LOG_DEBUG("Child added to task: {}, relayouting", nodeName());
  relayoutCapabilitySlots();
}

void TaskNode::childRemoved(NodeItem* child)
{
  NodeItem::childRemoved(child);
  relayoutCapabilitySlots();
}

void TaskNode::settingsChanged(const AppearanceSettings& appearance)
{
  if (appearance.numberOfColumnsTask != mColumnCount)
  {
    mColumnCount = appearance.numberOfColumnsTask;
    relayoutCapabilitySlots();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// Layout – capability slots
// ─────────────────────────────────────────────────────────────────────────────
void TaskNode::swapCapabilityOrder(NodeItem* a, NodeItem* b)
{
  // if (!a || !b || a == b)
  //   return;

  // const int ia = mChildrenNodes.indexOf(a);
  // const int ib = mChildrenNodes.indexOf(b);
  // if (ia < 0 || ib < 0)
  //   return;

  // std::swap(mChildrenNodes[ia], mChildrenNodes[ib]);
  // relayoutCapabilitySlots();
}

TaskNode::SlotLayout TaskNode::slotLayout(const QSizeF& size, const QList<CapabilityNode*>& capabilities) const
{
  SlotLayout layout;

  const int slotCount = capabilities.size() + 1;

  layout.columnCount = qMax(1, mColumnCount);
  layout.rowCount = (slotCount + layout.columnCount - 1) / layout.columnCount;
  layout.contentRect = QRectF(QPointF(0.0, 0.0), size).adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);
  layout.diameter = defaultSlotDiameter();

  // Every row contains at least one circle.
  layout.rowHeights.fill(layout.diameter, layout.rowCount);

  // Account for the actual space occupied by each capability.
  for (qsizetype i = 0; i < capabilities.size(); ++i)
  {
    const auto* capability = capabilities[i];
    const int row = i / layout.columnCount;

    const qreal height = capability->nodeRect().united(capability->labelBoundingRect()).height();

    layout.rowHeights[row] = qMax(layout.rowHeights[row], height);
  }

  const int usedColumns = qMin(slotCount, layout.columnCount);

  const qreal horizontalFreeSpace = layout.contentRect.width() - usedColumns * layout.diameter;

  layout.horizontalGap = qMax<qreal>(0.0, horizontalFreeSpace / (usedColumns + 1));

  const qreal totalRowHeight = std::accumulate(layout.rowHeights.cbegin(), layout.rowHeights.cend(), 0.0);

  const qreal verticalFreeSpace = layout.contentRect.height() - totalRowHeight;

  layout.verticalGap = qMax<qreal>(0.0, verticalFreeSpace / (layout.rowCount + 1));

  return layout;
}

void TaskNode::relayoutCapabilitySlots()
{
  if (!mEmptySlot)
    return;

  const auto childNodes = children();

  QList<CapabilityNode*> capabilities;
  capabilities.reserve(childNodes.size());

  for (auto* child : childNodes)
    if (auto* capability = qgraphicsitem_cast<CapabilityNode*>(child))
      capabilities.append(capability);

  const int slotCount = capabilities.size() + 1;
  const qreal diameter = defaultSlotDiameter();

  // Resize before measuring the labels.
  for (auto* capability : capabilities)
    capability->applySize(QSizeF(diameter, diameter));

  const auto initialLayout = slotLayout(nodeRect().size(), capabilities);
  const QSizeF requiredSize = minimumSizeForCapabilitySlots(initialLayout, slotCount);

  // Allow the task to both grow and shrink.
  if (requiredSize != nodeRect().size())
    applySize(requiredSize);

  const auto layout = slotLayout(nodeRect().size(), capabilities);
  const auto centers = capabilitySlotCenters(layout, slotCount);

  if (centers.size() != slotCount)
    return;

  for (qsizetype i = 0; i < capabilities.size(); ++i)
    capabilities[i]->setCenterPosition(mapToScene(centers[i]));

  mEmptySlot->setDiameter(layout.diameter);
  mEmptySlot->setPos(centers.back());
}

QSizeF TaskNode::minimumSizeForCapabilitySlots(const SlotLayout& layout, int slotCount) const
{
  if (slotCount <= 0)
    return QSizeF(TASK_WIDTH, TASK_HEIGHT);

  const int usedColumns = qMin(slotCount, layout.columnCount);
  const qreal requiredWidth = 2.0 * MARGIN + usedColumns * layout.diameter + (usedColumns - 1) * MARGIN;
  const qreal totalRowHeight = std::accumulate(layout.rowHeights.cbegin(), layout.rowHeights.cend(), 0.0);
  const qreal requiredHeight = 2.0 * MARGIN + totalRowHeight + (layout.rowCount - 1) * MARGIN;

  return QSizeF(qMax(requiredWidth, TASK_WIDTH), qMax(requiredHeight, TASK_HEIGHT));
}

qreal TaskNode::capabilityLabelHeight(qreal slotDiameter) const
{
  return NodeBase::maximumLabelHeight(NodeBase::labelFontForSize(slotDiameter));
}

QVector<QPointF> TaskNode::capabilitySlotCenters(const SlotLayout& layout, int slotCount) const
{
  QVector<QPointF> centers;

  if (slotCount <= 0)
    return centers;

  centers.reserve(slotCount);

  const qreal radius = layout.diameter / 2.0;

  qreal rowY = layout.contentRect.top() + layout.verticalGap;

  for (int row = 0; row < layout.rowCount; ++row)
  {
    const int firstIndex = row * layout.columnCount;
    const int remaining = slotCount - firstIndex;
    const int itemsInRow = qMin(layout.columnCount, remaining);

    const qreal centerY = rowY + radius;
    const qreal horizontalFreeSpace = layout.contentRect.width() - itemsInRow * layout.diameter;
    const qreal gap = horizontalFreeSpace / (itemsInRow + 1);
    for (int column = 0; column < itemsInRow; ++column)
    {
      const qreal centerX = layout.contentRect.left() + gap + radius + column * (layout.diameter + gap);

      centers.emplaceBack(centerX, centerY);
    }

    rowY += layout.rowHeights[row] + layout.verticalGap;
  }

  return centers;
}

void TaskNode::ensureMainFlowExists()
{
  auto existingMain = maki::findIf(flowConfigs(), [](const auto& flow) { return flow->getname() == Constants::MAIN_FLOW; });
  if (existingMain)
  {
    // During copy, the main flow was already created, so we must emit the signal here
    SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::FlowAdded,
                                               .data = FlowEvent{
                                                 .id = (*existingMain)->getid(),
                                                 .ownerNodeId = id(),
                                               }});
    return;
  }

  auto info = std::make_shared<FlowSaveInfo>();
  info->setName(Constants::MAIN_FLOW);

  QList<QPointF> initialPos = {};
  if (auto canvas = qobject_cast<CanvasView*>(scene()->parent()))
  {
    const QRectF visible = canvas->mapToScene(canvas->viewport()->rect()).boundingRect();
    initialPos << QPointF{visible.left() + 75, 0};
    initialPos << QPointF{visible.left() + visible.width() - 75, 0};
  }

  SaveInfo::instance().addFlowToNode(id(), info, initialPos);
}
