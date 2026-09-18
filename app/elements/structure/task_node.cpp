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
#include "capability_node.h"
#include "document/node_info.h"
#include "elements/flow.h"
#include "logging.h"
#include "widgets/controls/capability_selector.h"

static constexpr qreal TASK_WIDTH = 324.0;
static constexpr qreal TASK_HEIGHT = 300.0;
static constexpr qreal TASK_ASPECT_RATIO = TASK_WIDTH / TASK_HEIGHT;
static constexpr qreal SLOT_DIAMETER_FACTOR = 0.25;
static constexpr qreal MARGIN = 16;

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
  ensureMainFlowExists();

  if (mEmptySlot)
  {
    const qreal bbW = nodeRect().width();
    const qreal bbH = nodeRect().height();
    mEmptySlot->setDiameter(qMin(bbW, bbH) * SLOT_DIAMETER_FACTOR);
  }

  relayoutCapabilitySlots();

  return NodeItem::start();
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

  NodeBase::paintLabel(painter, drawingRect(nodeRect()), pen);
}

QPainterPath TaskNode::shape() const
{
  return NodeBase::nodeShape(nodeRect());
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

TaskNode::SlotLayout TaskNode::slotLayout(const QSizeF& size, int slotCount) const
{
  SlotLayout layout;

  if (slotCount <= 0)
    return layout;

  layout.columnCount = mColumnCount;
  layout.rowCount = (slotCount + layout.columnCount - 1) / layout.columnCount;

  layout.contentRect = QRectF(QPointF(0.0, 0.0), size).adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);
  layout.diameter = qMin(layout.contentRect.width(), layout.contentRect.height()) * SLOT_DIAMETER_FACTOR;
  layout.labelHeight = capabilityLabelHeight(layout.diameter);

  const qreal cellHeight = layout.diameter + layout.labelHeight;
  const int usedColumns = qMin(slotCount, layout.columnCount);

  const qreal horizontalFreeSpace = layout.contentRect.width() - usedColumns * layout.diameter;
  layout.horizontalGap = qMax<qreal>(0.0, horizontalFreeSpace / (usedColumns + 1));

  const qreal verticalFreeSpace = layout.contentRect.height() - layout.rowCount * cellHeight;
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

  const auto initialLayout = slotLayout(nodeRect().size(), slotCount);
  const QSizeF requiredSize = minimumSizeForCapabilitySlots(initialLayout, slotCount);
  const QSizeF currentSize = nodeRect().size();
  const QSizeF newSize(qMax(currentSize.width(), requiredSize.width()), qMax(currentSize.height(), requiredSize.height()));
  if (newSize != currentSize)
    applySize(newSize);

  const auto layout = slotLayout(nodeRect().size(), slotCount);
  const auto centers = capabilitySlotCenters(layout, slotCount);
  if (centers.size() != slotCount)
    return;

  for (qsizetype i = 0; i < capabilities.size(); ++i)
  {
    auto* capability = capabilities[i];
    capability->applySize(QSizeF(layout.diameter, layout.diameter));
    capability->setCenterPosition(mapToScene(centers[i]));
  }

  mEmptySlot->setDiameter(layout.diameter);
  mEmptySlot->setPos(centers.back());
}

QSizeF TaskNode::minimumSizeForCapabilitySlots(const SlotLayout& layout, int slotCount) const
{
  if (slotCount <= 0)
    return QSizeF(TASK_WIDTH, TASK_HEIGHT);

  const qreal diameter = qMin(TASK_WIDTH, TASK_HEIGHT) * SLOT_DIAMETER_FACTOR;
  const qreal labelHeight = capabilityLabelHeight(diameter);
  const qreal cellHeight = diameter + labelHeight;
  const qreal requiredWidth = 2.0 * MARGIN + layout.columnCount * diameter + (layout.columnCount - 1) * MARGIN;
  const qreal requiredHeight = 2.0 * MARGIN + layout.rowCount * cellHeight + (layout.rowCount - 1) * MARGIN;

  return QSizeF(qMax(requiredWidth, TASK_WIDTH), qMax(requiredHeight, TASK_HEIGHT));
}

qreal TaskNode::capabilityLabelHeight(qreal slotDiameter) const
{
  const qreal requestedPointSize = qMax(Fonts::BaseSize, slotDiameter / Fonts::BaseFactor);

  QFont font;
  font.setPointSizeF(qMin(Fonts::MaxSize, requestedPointSize));

  const QFontMetricsF metrics(font);
  constexpr qreal labelSpacing = 2.0;

  return labelSpacing + metrics.height();
}

QVector<QPointF> TaskNode::capabilitySlotCenters(const SlotLayout& layout, int slotCount) const
{
  QVector<QPointF> centers;

  if (slotCount <= 0)
    return centers;

  centers.reserve(slotCount);

  const qreal radius = layout.diameter * 0.5;
  const qreal cellHeight = layout.diameter + layout.labelHeight;

  for (int row = 0; row < layout.rowCount; ++row)
  {
    const int firstIndex = row * layout.columnCount;
    const int remaining = slotCount - firstIndex;
    const int itemsInRow = qMin(layout.columnCount, remaining);

    const qreal centerY = layout.contentRect.top() + layout.verticalGap + radius + row * (cellHeight + layout.verticalGap);
    if (itemsInRow == 1)
    {
      centers.emplaceBack(layout.contentRect.center().x(), centerY);
      continue;
    }

    const qreal horizontalFreeSpace = layout.contentRect.width() - itemsInRow * layout.diameter;
    const qreal gap = horizontalFreeSpace / (itemsInRow + 1);
    for (int column = 0; column < itemsInRow; ++column)
    {
      const qreal centerX = layout.contentRect.left() + gap + radius + column * (layout.diameter + gap);
      centers.emplaceBack(centerX, centerY);
    }
  }

  return centers;
}

void TaskNode::ensureMainFlowExists()
{
  for (const auto& flow : flowConfigs())
    if (flow->getname() == Constants::MAIN_FLOW)
      return;

  auto info = std::make_shared<FlowSaveInfo>();
  info->setType(Types::CallType::USER);
  info->setModifiable(true);
  info->setReturnType(koda::types::TypeReference::createVoid());
  (void)createFlow(Constants::MAIN_FLOW, info);
}
