#include <utility>

#include <algorithm>

#include "node.h"

#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QHash>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QSvgRenderer>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include <QUndoStack>
#include <QUuid>

#include "app_configs.h"
#include "app_paths.h"
#include "flow.h"
#include "port.h"
#include "logging.h"
#include "style_helpers.h"
#include "subtask_connector.h"
#include "system/canvas.h"
#include "system/undo_commands/move_node.h"
#include "system/undo_commands/resize_node.h"
#include "system/undo_commands/swap_capabilities.h"

namespace
{
/** Shared by tidy-tree packing and structural task layout finalization */
QHash<NodeItem*, QPointF> g_packNodeOriginInSubtree;
QHash<NodeItem*, QVector<QPair<NodeItem*, QPointF>>> g_childSubtreeTopLeftOffsets;

void renderSvgInEllipse(QPainter* painter, const QString& svgPath, const QPointF& center, qreal diameter)
{
  if (svgPath.isEmpty())
    return;
  QSvgRenderer renderer(svgPath);
  if (!renderer.isValid())
    return;
  QRectF viewBox = renderer.viewBoxF();
  if (!viewBox.isValid() || viewBox.isEmpty())
    viewBox = QRectF(0, 0, 1, 1);

  QRectF drawingBounds(center.x() - diameter * 0.5, center.y() - diameter * 0.5, diameter, diameter);
  constexpr qreal padding = 2.0;
  QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
  const qreal sx = contentRect.width() / viewBox.width();
  const qreal sy = contentRect.height() / viewBox.height();
  const qreal scale = qMin(sx, sy);
  const QSizeF scaledSize(viewBox.width() * scale, viewBox.height() * scale);
  const QRectF targetRect(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0,
                          contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
                          scaledSize.width(),
                          scaledSize.height());
  renderer.render(painter, targetRect);
}
}  // namespace

namespace structural_layout
{
constexpr qreal kTaskCornerRadius = 28.0;
constexpr qreal kTaskInnerPadding = 6.0;
constexpr qreal kTaskSlotDiameterFactor = 0.30;  // relative to min(width,height)
constexpr qreal kTaskSlotTopY = 0.30;
constexpr qreal kTaskSlotBottomY = 0.70;
constexpr qreal kTaskSlotLeftX = 0.30;
constexpr qreal kTaskSlotRightX = 0.70;
constexpr qreal kTaskAspectWidth = 324.0;
constexpr qreal kTaskAspectHeight = 300.0;
constexpr qreal kTaskAspectRatio = kTaskAspectWidth / kTaskAspectHeight;
/** Gap between slot-circle rows as fraction of slot diameter (shared with relayoutCapabilitySlots). */
constexpr qreal kSlotVerticalGapFactor = 0.35;

/** Estimated caption height below an inset capability circle (matches NodeItem label sizing). */
qreal estimatedCapabilityLabelOverhang(qreal slotDiam)
{
  const qreal fontPt = qMax(Fonts::BaseSize, slotDiam / Fonts::BaseFactor);
  QFont font;
  font.setPointSizeF(qMin(Fonts::MaxSize, fontPt));
  const QFontMetricsF fm(font);
  constexpr qreal kLabelGapBelowCircle = 2.0;
  return kLabelGapBelowCircle + fm.height();
}

qreal taskSlotTopInset()
{
  constexpr qreal margin = 16.0;
  return margin + kTaskInnerPadding;
}

qreal taskSlotBottomInset(qreal slotDiam)
{
  constexpr qreal margin = 16.0;
  return margin + kTaskInnerPadding + estimatedCapabilityLabelOverhang(slotDiam);
}

/**
 * Capability + placeholder slot centres in node-local coordinates (basis: boundingRect 0..w x 0..h).
 * - count == 1: single centred near top spine.
 * - count == 2: one row, side-by-side in two columns (left/right).
 * - count >= 3: at most two columns, row-major; if count is odd, the final slot sits centred on row 2.
 */
QVector<QPointF> taskSlotCenters(const QSizeF& size, int count)
{
  QVector<QPointF> out;
  if (count <= 0)
    return out;

  constexpr qreal margin = 16.0;
  const qreal w = size.width();
  const qreal h = size.height();
  const qreal anchorW = qMin(w, kTaskAspectWidth);
  const qreal colStep = (kTaskSlotRightX - kTaskSlotLeftX) * anchorW;
  const qreal slotDiam = qMin(w, h) * kTaskSlotDiameterFactor;

  const qreal leftColCenter = (w - colStep) * 0.5;
  const qreal rightColCenter = leftColCenter + colStep;

  if (count == 1)
  {
    out.append(QPointF(w * 0.5, kTaskSlotTopY * h));
    return out;
  }

  if (count == 2)
  {
    const qreal yy = ((kTaskSlotTopY + kTaskSlotBottomY) * 0.5) * h;
    out.append(QPointF(leftColCenter, yy));
    out.append(QPointF(rightColCenter, yy));
    return out;
  }

  const int numRows = (count + 1) / 2;
  const qreal pitch =
      (numRows > 1) ? slotDiam * (1.0 + kSlotVerticalGapFactor) : static_cast<qreal>(0.0);
  const qreal totalSpan = (numRows > 1) ? static_cast<qreal>(numRows - 1) * pitch : 0.0;
  const qreal topInset = taskSlotTopInset();
  const qreal bottomInset = taskSlotBottomInset(slotDiam);
  const qreal yMinTop = topInset + slotDiam * 0.5;
  const qreal yMaxTop = (numRows > 1) ? (h - bottomInset - slotDiam * 0.5 - totalSpan)
                                     : yMinTop;
  qreal topRowY = (numRows > 1) ? ((h - totalSpan) * 0.5)
                                : ((kTaskSlotTopY + kTaskSlotBottomY) * 0.5) * h;
  if (numRows > 1)
    topRowY = std::clamp(topRowY, yMinTop, qMax(yMinTop, yMaxTop));

  out.reserve(count);
  for (int i = 0; i < count; ++i)
  {
    const int row = i / 2;
    const int col = i % 2;

    qreal cx = (col == 0) ? leftColCenter : rightColCenter;
    if (count % 2 == 1 && i == count - 1)
      cx = w * 0.5;

    qreal yy = 0;
    if (numRows <= 1)
      yy = ((kTaskSlotTopY + kTaskSlotBottomY) * 0.5) * h;
    else
      yy = topRowY + static_cast<qreal>(row) * pitch;

    out.append(QPointF(cx, yy));
  }

  return out;
}

// --- Non-layered compact horizontal packing of structural subtasks ( subtree geometry ) ---

constexpr qreal kSubtreeLevelGap = 72.0;
constexpr qreal kSubtreeSiblingGap = 32.0;

struct PackResult
{
  qreal width = 0;
  qreal height = 0;
};

PackResult structuralPackSubtree(NodeItem* v)
{
  const qreal myW = v->boundingRect().width();
  const qreal myH = v->boundingRect().height();
  QVector<NodeItem*> subs = v->structuralSubtaskChildren();

  if (subs.isEmpty())
  {
    g_packNodeOriginInSubtree[v] = QPointF(0, 0);
    return {myW, myH};
  }

  QVector<PackResult> cp;
  cp.reserve(subs.size());
  qreal rowWidth = -(kSubtreeSiblingGap);
  qreal maxSubtreeHBelow = 0;
  for (NodeItem* st : subs)
  {
    PackResult pr = structuralPackSubtree(st);
    cp.append(pr);
    rowWidth += pr.width + kSubtreeSiblingGap;
    maxSubtreeHBelow = qMax(maxSubtreeHBelow, pr.height);
  }

  const qreal subtreeW = qMax(myW, rowWidth);
  const qreal subtreeH = myH + kSubtreeLevelGap + maxSubtreeHBelow;

  const qreal px = (subtreeW - myW) * 0.5;
  g_packNodeOriginInSubtree[v] = QPointF(px, 0);

  qreal cum = (subtreeW - rowWidth) * 0.5;
  QVector<QPair<NodeItem*, QPointF>> origins;
  origins.reserve(subs.size());
  for (int i = 0; i < subs.size(); ++i)
  {
    origins.append(qMakePair(subs[i], QPointF(cum, myH + kSubtreeLevelGap)));
    cum += cp[i].width + kSubtreeSiblingGap;
  }
  g_childSubtreeTopLeftOffsets[v] = origins;

  return {subtreeW, subtreeH};
}

void recursiveRelayoutCapabilitiesPostorder(NodeItem* v)
{
  for (NodeItem* st : v->structuralSubtaskChildren())
    recursiveRelayoutCapabilitiesPostorder(st);
  v->relayoutCapabilitySlots();
}

void layoutNonLayeredTidyTree(NodeItem* root)
{
  g_packNodeOriginInSubtree.clear();
  g_childSubtreeTopLeftOffsets.clear();

  recursiveRelayoutCapabilitiesPostorder(root);
  (void)structuralPackSubtree(root);

  const QPointF preservedRootTopLeft = root->pos();
  const QPointF rootOff = g_packNodeOriginInSubtree.value(root, QPointF(0, 0));
  const QPointF subtreeSceneTopLeft = preservedRootTopLeft - rootOff;
  root->finalizeStructuralPackedPositions(subtreeSceneTopLeft);
  recursiveRelayoutCapabilitiesPostorder(root);
}

QSizeF taskAspectSizeFromWidth(qreal width)
{
  return QSizeF(width, width / kTaskAspectRatio);
}
}  // namespace structural_layout

using structural_layout::kTaskAspectRatio;
using structural_layout::kTaskAspectWidth;
using structural_layout::kTaskCornerRadius;
using structural_layout::kTaskInnerPadding;
using structural_layout::kTaskSlotDiameterFactor;

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->getnodeId(), nodeConfig, parent)
    , mStorage(info)
    , mParentNode(nullptr)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->getScale() : 1.0)
    , mSize(mStorage->getSize())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  mStorage->setId(this->id());
  mStorage->setNodeId(this->nodeId());

  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Task"))
  {
    mSize = structural_layout::taskAspectSizeFromWidth(mSize.width());
    mStorage->setSize(mSize);
  }

  // Children are created by the canvas, so we must make sure that there is no trailing children information
  mStorage->clearChildren();

  for (const auto& property : config()->properties)
  {
    if (!mStorage->getproperties().contains(property.id))
      mStorage->addProperty(property.id, property.defaultValue);
  }

  for (const auto& event : config()->events)
  {
    bool found = false;
    for (const auto& flow : mStorage->getflows())
    {
      if (flow->getname() != event.name)
        continue;

      found = true;
      break;
    }

    if (found)
      continue;

    mStorage->addFlow(std::make_shared<FlowSaveInfo>(event));
  }

  // node svg replaces icon if set
  const bool structuralCapability =
      config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type != QStringLiteral("Task");
  if (config()->body.nodeSvg.isEmpty() && !mStorage->getIcon().isEmpty() && !structuralCapability)
    setIcon(mStorage->getIcon(), config()->body.iconColor);

  qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabel(getProperty("name").toString(), labelSize);

  if (config()->libraryType == Types::LibraryTypes::BEHAVIOUR)
  {
    if (config()->hasInPort)
      mInPort = new PortItem(PortItem::In, this);
    if (config()->hasOutPort)
      mOutPort = new PortItem(PortItem::Out, this);
    updatePortPositions();
  }

  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
  mLastPosition = pos();

  LOG_DEBUG("%s created at: (%f, %f) with size (%f, %f) and scale %f", qPrintable(id()), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale());
}

NodeItem::~NodeItem()
{
  destroySubtaskConnector();
}

int NodeItem::type() const
{
  return Type;
}

Types::LibraryTypes NodeItem::function() const
{
  return config()->libraryType;
}

QString NodeItem::nodeName() const
{
  auto name = getProperty("name");
  return name.toString();
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return nodeId();
}

qreal NodeItem::baseScale() const
{
  return mStorage->getScale();
}

VoidResult NodeItem::start()
{
  return NodeBase::start();
}

QRectF NodeItem::boundingRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  auto color = getProperty("color");
  auto background = color.isValid() ? QColor::fromString(color.toString()) : config()->body.backgroundColor;

  if (rendersAsInsetCapability())
  {
    Q_UNUSED(style);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
    painter->setPen(pen);
    painter->setBrush(QBrush(background));
    const QRectF r = boundingRect().adjusted(2, 2, -2, -2);
    painter->drawEllipse(r);
    if (!config()->body.iconPath.isEmpty())
    {
      const QString iconAbsPath = AppPaths::icon(config()->body.iconPath);
      renderSvgInEllipse(painter, iconAbsPath, r.center(), qMin(r.width(), r.height()));
    }
    return;
  }

  if (isTaskContainer())
  {
    Q_UNUSED(style);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.5 / baseScale());
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const QRectF bodyRect = boundingRect().adjusted(kTaskInnerPadding,
                                                    kTaskInnerPadding,
                                                    -kTaskInnerPadding,
                                                    -kTaskInnerPadding);
    painter->drawRoundedRect(bodyRect, kTaskCornerRadius, kTaskCornerRadius);

    const qreal bbW = boundingRect().width();
    const qreal bbH = boundingRect().height();
    const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
    const qreal slotRadius = slotDiameter * 0.5;
    const int n = static_cast<int>(structuralCapabilityChildren().size());
    const QVector<QPointF> centers = structural_layout::taskSlotCenters(boundingRect().size(), n + 1);
    if (!centers.isEmpty())
    {
      const QPointF placeholderCenter = centers.last();
      if (mHoverPreviewActive && mHoverPreviewColor.isValid())
      {
        const QPen capPen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
        painter->setPen(capPen);
        painter->setBrush(QBrush(mHoverPreviewColor));
        painter->drawEllipse(placeholderCenter, slotRadius, slotRadius);
        renderSvgInEllipse(painter, mHoverPreviewIcon, placeholderCenter, slotDiameter);
      }
      else
      {
        QPen dashPen(Qt::black, 1.5 / baseScale(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(dashPen);
        painter->setBrush(QBrush(QColor(0xe6, 0xe6, 0xe6)));
        painter->drawEllipse(placeholderCenter, slotRadius, slotRadius);
      }
    }
    return;
  }

  const QPen outlinePen =
      isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
  const bool isStructuralCapability =
      function() == Types::LibraryTypes::STRUCTURAL && !isTaskContainer();

  NodeBase::paintNode(boundingRect(), background, outlinePen, painter);

  if (isStructuralCapability && !config()->body.iconPath.isEmpty())
  {
    const QRectF r = boundingRect().adjusted(2, 2, -2, -2);
    renderSvgInEllipse(painter, AppPaths::icon(config()->body.iconPath), r.center(), qMin(r.width(), r.height()));
  }
}

QPainterPath NodeItem::shape() const
{
  if (rendersAsInsetCapability())
  {
    QPainterPath path;
    path.addEllipse(boundingRect().adjusted(2, 2, -2, -2));
    return path;
  }
  return NodeBase::nodeShape(boundingRect());
}

QVector<PropertyConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QMap<QString, QVariant> NodeItem::properties() const
{
  return mStorage->getproperties();
}

QVector<std::shared_ptr<IProperty>> NodeItem::fields() const
{
  return mStorage->getfields();
}

QVector<std::shared_ptr<IFlow>> NodeItem::events() const
{
  return mStorage->getflows();
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

QVariant NodeItem::getProperty(const QString& key) const
{
  if (!mStorage)
    return QVariant();

  return mStorage->getProperty(key);
}

void NodeItem::setProperty(const QString& key, QVariant value)
{
  LOG_DEBUG("[%s] Setting property %s of node: %s", qPrintable(id()), qPrintable(key), qPrintable(nodeId()));
  if (!mStorage)
    return;

  // TODO(felaze): We should check for success here
  mStorage->addProperty(key, value);

  if (key == "name")
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

void NodeItem::setEvent(int index, const FlowConfig& event)
{
  // if (!mStorage)
  //   return;

  // if (index < mStorage->events.size())
  //   mStorage->events[index] = event;
  // else
  //   mStorage->events.push_back(event);
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(std::make_shared<PropertyInfo>(property)));

  return VoidResult();
}

VoidResult NodeItem::setField(const QString& key, std::shared_ptr<PropertyInfo> property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(property));

  return VoidResult();
}

PropertyInfo NodeItem::getField(const QString& key) const
{
  if (!mStorage)
    return PropertyInfo();

  return mStorage->getField(key);
}

void NodeItem::removeField(const QString& key)
{
  if (!mStorage)
    return;

  mStorage->removeField(key);
}

QVector<NodeItem*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addParent(NodeItem* parent)
{
  if (!parent)
    return;

  mParentNode = parent;
  mStorage->setParentId(parent->id());
  setZValue(parent->zValue() + 2);

  if (function() == Types::LibraryTypes::STRUCTURAL && parent->isTaskContainer())
  {
    if (isTaskContainer())
      setFlag(ItemIsMovable, false);
    return;
  }

  fitInsideParent(20);
}

void NodeItem::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (!info)
    return;

  mStorage->addChild(info);
  mChildrenNodes.push_back(node);
}

void NodeItem::childRemoved(NodeItem* child)
{
  mStorage->removeChild(child->id());
  mChildrenNodes.removeAll(child);

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
  {
    if (child->isTaskContainer())
      layoutSubtasks();
    else
    {
      relayoutCapabilitySlots();
      layoutSubtasks();
    }
  }
}

bool NodeItem::isTaskContainer() const
{
  return config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Task");
}

bool NodeItem::isStructuralSubtask() const
{
  return isTaskContainer() && mParentNode && mParentNode->isTaskContainer();
}

bool NodeItem::rendersAsInsetCapability() const
{
  return function() == Types::LibraryTypes::STRUCTURAL && mParentNode && mParentNode->isTaskContainer() && !isTaskContainer();
}

QVector<NodeItem*> NodeItem::structuralSubtaskChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (c->isTaskContainer())
      out.append(c);
  }
  return out;
}

QVector<NodeItem*> NodeItem::structuralCapabilityChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (!c->isTaskContainer())
      out.append(c);
  }
  return out;
}

NodeItem* NodeItem::rootStructuralTask() const
{
  NodeItem* root = const_cast<NodeItem*>(this);
  NodeItem* parent = mParentNode;
  while (parent && parent->isTaskContainer())
  {
    root = parent;
    parent = parent->parentNode();
  }
  return root;
}

void NodeItem::applyStructuralLayoutTopLeft(const QPointF& topLeftScene)
{
  prepareGeometryChange();
  setPos(topLeftScene);
  mLastPosition = topLeftScene;
  updateExtrasPosition();
  mStorage->setPosition(pos() + boundingRect().center());

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void NodeItem::finalizeStructuralPackedPositions(const QPointF& subtreeSceneTopLeft)
{
  applyStructuralLayoutTopLeft(subtreeSceneTopLeft + g_packNodeOriginInSubtree.value(this, QPointF(0, 0)));

  const QVector<QPair<NodeItem*, QPointF>> ch =
      g_childSubtreeTopLeftOffsets.value(this, QVector<QPair<NodeItem*, QPointF>>());

  for (const auto& kv : ch)
  {
    if (kv.first)
      kv.first->finalizeStructuralPackedPositions(subtreeSceneTopLeft + kv.second);
  }
}

void NodeItem::layoutSubtasks()
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  NodeItem* root = rootStructuralTask();
  if (root != this)
  {
    root->layoutSubtasks();
    return;
  }

  structural_layout::layoutNonLayeredTidyTree(root);
}

void NodeItem::relayoutCapabilitySlots()
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  QVector<NodeItem*> caps = structuralCapabilityChildren();
  const int n = caps.size();

  constexpr qreal margin = 16.0;

  qreal W = mSize.width();
  qreal H = mSize.height();

  const int slotCount = n + 1;

  const qreal cfgW = static_cast<qreal>(config()->body.width);
  const qreal cfgH = static_cast<qreal>(config()->body.height);
  const qreal reqW = qMax(cfgW, W);  // do not shrink below config or current width

  const int numRows =
      slotCount <= 1 ? 1 : (slotCount + 1) / 2;  // ceil(slotCount/2) for slotCount >= 2

  // Match taskSlotCenters: diameter uses qMin(W,H); sizing on cfg min underestimated height
  // for wide Tasks once H grows beyond min(cfgW,cfgH).
  qreal trialH = qMax(cfgH, H);
  constexpr int kSizingIters = 12;
  for (int iter = 0; iter < kSizingIters; ++iter)
  {
    const qreal minDim = qMin(reqW, trialH);
    const qreal sd = minDim * kTaskSlotDiameterFactor;
    const qreal verticalGap = sd * structural_layout::kSlotVerticalGapFactor;
    const qreal pitch = sd + verticalGap;
    const qreal topInset = structural_layout::taskSlotTopInset();
    const qreal bottomInset = structural_layout::taskSlotBottomInset(sd);
    qreal needH =
        topInset + sd + static_cast<qreal>(numRows - 1) * pitch + bottomInset;
    needH = qMax(cfgH, needH);

    if (needH <= trialH + 0.51)
      break;
    trialH = needH;
  }
  qreal reqH = trialH;

  if (reqH > H + 0.5 || reqW > W + 0.5)
    applySize(QSizeF(reqW, reqH));
  W = mSize.width();
  H = mSize.height();

  QVector<QPointF> centers = structural_layout::taskSlotCenters(QSizeF(W, H), slotCount);
  qreal slotDiam = qMin(W, H) * kTaskSlotDiameterFactor;

  for (int i = 0; i < n; ++i)
  {
    const QPointF centerLocal = centers[i];
    const QPointF centerScene = mapToScene(centerLocal);
    NodeItem* cap = caps[i];
    cap->applySize(QSizeF(slotDiam, slotDiam));
    const QSizeF cs = cap->boundingRect().size();
    cap->updatePosition(centerScene - QPointF(cs.width() / 2.0, cs.height() / 2.0));
  }

  // Measure real label overhang after capability resize; grow once if captions still clip the border.
  qreal lowestContentBottom = 0.0;
  for (int i = 0; i < centers.size(); ++i)
  {
    const qreal slotBottom = centers[i].y() + slotDiam * 0.5;
    const qreal labelBelow = (i < n) ? caps[i]->labelExtentBelowBody()
                                     : structural_layout::estimatedCapabilityLabelOverhang(slotDiam);
    lowestContentBottom = qMax(lowestContentBottom, slotBottom + labelBelow);
  }

  const qreal bottomClearance = margin + kTaskInnerPadding;
  const qreal requiredH = lowestContentBottom + bottomClearance;
  if (requiredH > H + 0.5)
  {
    applySize(QSizeF(reqW, requiredH));
    H = mSize.height();
    centers = structural_layout::taskSlotCenters(QSizeF(W, H), slotCount);
    slotDiam = qMin(W, H) * kTaskSlotDiameterFactor;
    for (int i = 0; i < n; ++i)
    {
      const QPointF centerLocal = centers[i];
      const QPointF centerScene = mapToScene(centerLocal);
      NodeItem* cap = caps[i];
      cap->applySize(QSizeF(slotDiam, slotDiam));
      const QSizeF cs = cap->boundingRect().size();
      cap->updatePosition(centerScene - QPointF(cs.width() / 2.0, cs.height() / 2.0));
    }
  }

  syncSubtaskConnector();
}

void NodeItem::swapCapabilityOrder(NodeItem* a, NodeItem* b)
{
  if (!a || !b || a == b)
    return;
  const int ia = mChildrenNodes.indexOf(a);
  const int ib = mChildrenNodes.indexOf(b);
  if (ia < 0 || ib < 0)
    return;
  std::swap(mChildrenNodes[ia], mChildrenNodes[ib]);
  relayoutCapabilitySlots();
}

NodeItem* NodeItem::capabilityAtScenePos(const QPointF& scenePos, NodeItem* exclude) const
{
  const QVector<NodeItem*> caps = structuralCapabilityChildren();
  const int slotCount = caps.size() + 1;
  if (slotCount <= 0)
    return nullptr;

  const qreal bbW = boundingRect().width();
  const qreal bbH = boundingRect().height();
  const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
  const qreal slotRadiusSq = (slotDiameter * 0.5) * (slotDiameter * 0.5);
  const QVector<QPointF> centers = structural_layout::taskSlotCenters(boundingRect().size(), slotCount);

  for (int i = 0; i < caps.size() && i < centers.size(); ++i)
  {
    NodeItem* cap = caps[i];
    if (cap == exclude)
      continue;
    const QPointF c = mapToScene(centers[i]);
    const QPointF d = scenePos - c;
    if (d.x() * d.x() + d.y() * d.y() <= slotRadiusSq)
      return cap;
  }
  return nullptr;
}

QRectF NodeItem::placeholderSlotSceneRect() const
{
  if (!isTaskContainer())
    return {};

  const qreal bbW = boundingRect().width();
  const qreal bbH = boundingRect().height();
  const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
  const qreal slotRadius = slotDiameter * 0.5;
  const int n = static_cast<int>(structuralCapabilityChildren().size());
  const QVector<QPointF> centers = structural_layout::taskSlotCenters(boundingRect().size(), n + 1);
  if (centers.isEmpty())
    return {};

  const QPointF c = mapToScene(centers.last());
  return QRectF(c.x() - slotRadius, c.y() - slotRadius, slotDiameter, slotDiameter);
}

bool NodeItem::placeholderSlotContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = placeholderSlotSceneRect();
  if (r.isEmpty())
    return false;
  const QPointF d = scenePos - r.center();
  const qreal rr = r.width() * 0.5;
  return (d.x() * d.x() + d.y() * d.y()) <= rr * rr;
}

void NodeItem::ensureSubtaskConnector(StructureCanvas* canvas)
{
  Q_UNUSED(canvas);
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  if (mSubtaskConnector)
  {
    mSubtaskConnector->syncGeometry();
    return;
  }

  QGraphicsScene* sc = scene();
  if (!sc)
    return;

  mSubtaskConnector = new SubtaskConnector(this);
  sc->addItem(mSubtaskConnector);
  mSubtaskConnector->syncGeometry();
}

void NodeItem::destroySubtaskConnector()
{
  if (!mSubtaskConnector)
    return;

  if (mSubtaskConnector->scene())
    mSubtaskConnector->scene()->removeItem(mSubtaskConnector);

  delete mSubtaskConnector;
  mSubtaskConnector = nullptr;
}

void NodeItem::syncSubtaskConnector()
{
  if (mSubtaskConnector)
    mSubtaskConnector->syncGeometry();
}

NodeItem* NodeItem::parentNode() const
{
  return mParentNode;
}

QString NodeItem::behaviour() const
{
  return config()->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return config()->help;
}

QRectF NodeItem::parentInnerSceneRect(qreal padding) const
{
  if (!parentNode())
    return {};

  QRectF r = parentNode()->mapRectToScene(parentNode()->boundingRect());
  return r.adjusted(padding, padding, -padding, -padding);
}

// Apply a new logical size to this node in one place
void NodeItem::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);

  // Same scale logic as before
  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);

  setLabelSize(newFontSize, mSize);
  updatePortPositions();
  update();

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void NodeItem::setHoverPreview(const QString& iconPath, const QColor& color, bool active)
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  mHoverPreviewActive = active;
  if (active)
  {
    mHoverPreviewIcon = iconPath;
    mHoverPreviewColor = color;
  }
  else
  {
    mHoverPreviewIcon.clear();
    mHoverPreviewColor = QColor();
  }
  update();
}

// Clamp this node's position so its scene rect stays inside `inner`
QPointF NodeItem::clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const
{
  QPointF posScene = pos();
  QPointF offset = childSceneRect.topLeft() - posScene;  // pos() → top-left

  QPointF newPos = posScene;

  const qreal minX = inner.left() - offset.x();
  const qreal maxX = inner.right() - (childSceneRect.width() + offset.x());
  const qreal minY = inner.top() - offset.y();
  const qreal maxY = inner.bottom() - (childSceneRect.height() + offset.y());

  newPos.setX(std::clamp(newPos.x(), minX, maxX));
  newPos.setY(std::clamp(newPos.y(), minY, maxY));

  return newPos;
}

QSizeF NodeItem::clampSize(qreal width, qreal height) const
{
  if (isStructuralSubtask())
    return QSizeF(width, height);

  if (!parentNode())
    return QSizeF(width, height);

  QRectF inner = parentInnerSceneRect(10);
  if (!inner.isValid())
    return QSizeF(width, height);

  // child pos
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF currentTopLeft = childSceneRect.topLeft();

  // Max width/height keeping bottom-right within inner
  qreal maxWidth = inner.right() - currentTopLeft.x();
  qreal maxHeight = inner.bottom() - currentTopLeft.y();

  qreal clampedW = qMin(width, qMax<qreal>(0.0, maxWidth));
  qreal clampedH = qMin(height, qMax<qreal>(0.0, maxHeight));

  return QSizeF(clampedW, clampedH);
}

void NodeItem::fitInsideParent(qreal padding)
{
  if (isStructuralSubtask())
    return;

  if (rendersAsInsetCapability())
    return;

  QRectF inner = parentInnerSceneRect(padding);
  if (!inner.isValid())
    return;

  // 1) Clamp size so we're not bigger than the inner rect
  QSizeF currentSize = mSize;
  qreal maxW = inner.width();
  qreal maxH = inner.height();

  maxW = qMax(maxW, Config::MINIMUM_NODE_SIZE);
  maxH = qMax(maxH, Config::MINIMUM_NODE_SIZE);

  qreal newW = qMin(currentSize.width(), maxW);
  qreal newH = qMin(currentSize.height(), maxH);

  if (newW != currentSize.width() || newH != currentSize.height())
    applySize(QSizeF(newW, newH));

  // 2) Clamp position so we're fully inside `inner`
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF newPos = clampPosInside(inner, childSceneRect);
  updatePosition(newPos);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing && (event->modifiers() & Qt::ShiftModifier))
  {
    QPointF delta = event->pos() - mResizeStartMousePos;

    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.width() + delta.x());
    qreal newHeight = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.height() + delta.y());

    QSizeF clampedSize = clampSize(newWidth, newHeight);

    applySize(clampedSize);

    if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && isTaskContainer())
    {
      relayoutCapabilitySlots();
      layoutSubtasks();
    }
    else
    {
      for (auto* child : children())
      {
        auto* nodeChild = static_cast<NodeItem*>(child);
        nodeChild->fitInsideParent(10);
      }
    }
  }
  else
  {
    if (isStructuralSubtask())
    {
      NodeItem* root = rootStructuralTask();
      if (root && root != this)
      {
        const QPointF delta = event->scenePos() - mTreeDragStartScenePos;
        root->updatePosition(mTreeDragRootStartPos + delta);
      }
      event->accept();
      return;
    }

    QGraphicsItem::mouseMoveEvent(event);
  }
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && event->modifiers() & Qt::ShiftModifier)
  {
    mIsResizing = true;
    mResizeStartMousePos = event->pos();
    mResizeStartSize = mSize;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::SizeFDiagCursor);
    event->accept();
  }
  else
  {
    mDragStartPos = pos();
    if (isStructuralSubtask())
    {
      NodeItem* root = rootStructuralTask();
      mTreeDragRootStartPos = root ? root->pos() : QPointF();
      mTreeDragStartScenePos = event->scenePos();
    }
    else if (rendersAsInsetCapability() && mParentNode)
    {
      mCapDragStartIndex = mParentNode->children().indexOf(this);
    }
    QGraphicsItem::mousePressEvent(event);
  }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing)
  {
    mIsResizing = false;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::ArrowCursor);

    auto canvas = static_cast<Canvas*>(scene());
    if (canvas)
      canvas->undoStack()->push(new ResizeNodeCommand(canvas, id(), mResizeStartSize, mSize));
  }
  else if (isStructuralSubtask())
  {
    auto canvas = static_cast<Canvas*>(scene());
    NodeItem* root = rootStructuralTask();
    if (canvas && root && root->pos() != mTreeDragRootStartPos)
      canvas->undoStack()->push(new MoveNodeCommand(canvas, root->id(), mTreeDragRootStartPos, root->pos()));
  }
  else if (rendersAsInsetCapability() && mParentNode)
  {
    const QPointF myCenterScene = mapToScene(boundingRect().center());
    NodeItem* target = mParentNode->capabilityAtScenePos(myCenterScene, this);
    auto* canvas = static_cast<Canvas*>(scene());
    if (target && canvas)
      canvas->undoStack()->push(new SwapCapabilitiesCommand(canvas, mParentNode->id(), id(), target->id()));
    else
      mParentNode->relayoutCapabilitySlots();
    mCapDragStartIndex = -1;
  }
  else if (pos() != mDragStartPos)
  {
    auto canvas = static_cast<Canvas*>(scene());
    if (canvas)
      canvas->undoStack()->push(new MoveNodeCommand(canvas, id(), mDragStartPos, pos()));
  }

  QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    if (NodeItem* parent = parentNode())
    {
      // Structural subtasks are not clamped inside the parent rect: they live
      // below the parent task. The user-drag redirect to the root is handled
      // explicitly in mouseMoveEvent so this path must remain a no-op for them.
      if (isStructuralSubtask())
        return QGraphicsItem::itemChange(change, value);

      QPointF newPos = value.toPointF();  // proposed new pos in scene coords

      // auto parent = parentNode();
      // if (parent == nullptr)
      //   return QGraphicsItem::itemChange(change, value);

      QRectF parentRect = parent->boundingRect();
      parentRect = parentRect.adjusted(10, 10, -10, -10);
      parentRect.translate(parent->pos());

      // Child rect in its own coords
      QRectF childLocalRect = boundingRect();

      // Compute allowed range so childSceneRect stays inside parentRect
      const qreal minX = parentRect.left();
      const qreal maxX = parentRect.right() - childLocalRect.width();
      const qreal minY = parentRect.top();
      const qreal maxY = parentRect.bottom() - childLocalRect.height();

      // Clamp
      newPos.setX(std::clamp(newPos.x(), minX, maxX));
      newPos.setY(std::clamp(newPos.y(), minY, maxY));

      return newPos;  // this replaces the proposed position
    }
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    updatePosition(value.toPointF());
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& newPosition)
{
  prepareGeometryChange();
  setPos(newPosition);

  QPointF delta = newPosition - mLastPosition;
  for (auto* child : children())
  {
    auto childNode = static_cast<NodeItem*>(child);
    childNode->updatePosition(childNode->pos() + delta);
  }

  mLastPosition = newPosition;

  updateExtrasPosition();
  mStorage->setPosition(pos() + boundingRect().center());

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void NodeItem::updateExtrasPosition()
{
  updatePortPositions();
  for (auto& transition : transitions())
    transition->updatePath();

  updateLabelPosition();
}

void NodeItem::updatePortPositions()
{
  const qreal w = boundingRect().width();
  const qreal h = boundingRect().height();
  if (mInPort)
    mInPort->setPos(-PortItem::kSize - PortItem::kGap, (h - PortItem::kSize) / 2.0);
  if (mOutPort)
    mOutPort->setPos(w + PortItem::kGap, (h - PortItem::kSize) / 2.0);
}

// Slots
void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

NodeSaveInfo NodeItem::saveInfo() const
{
  return *mStorage;
}

QVector<TransitionItem*> NodeItem::transitions() const
{
  return mTransitions;
}

void NodeItem::addTransition(TransitionItem* transition)
{
  // Make sure the source node holds the transition info
  if (transition->destination() && (id() != transition->destination()->id()))
  {
    bool found = false;
    for (const auto& t : mStorage->gettransitions())
    {
      if (t->getid() == transition->id())
      {
        found = true;
        break;
      }
    }

    if (!found)
      mStorage->addTransition(transition->storage());

    for (auto& t : transitions())
    {
      // If I am the source of this transition
      // Check whether we have another transition with me as destination
      auto src1 = transition->source()->id();
      auto dst1 = transition->destination()->id();
      auto src2 = t->source()->id();
      auto dst2 = t->destination()->id();
      if (((src1 == dst2) && (src2 == dst1)))
      {
        transition->setEdge(TransitionItem::Edge::FORWARD);
        t->setEdge(TransitionItem::Edge::BACKWARD);
      }
    }
  }

  bool found = false;
  for (const auto& t : transitions())
  {
    if (t->id() == transition->id())
    {
      found = true;
      break;
    }
  }

  if (!found)
    mTransitions.push_back(transition);

  prepareGeometryChange();
  updateExtrasPosition();
}

void NodeItem::removeTransition(TransitionItem* transition)
{
  mStorage->removeTransition(transition->storage());
  mTransitions.removeIf([transition](TransitionItem* item) {
    return item->id() == transition->id();
  });
}

QPointF NodeItem::edgePointToward(const QPointF& targetScenePos, bool fromOutgoingPort) const
{
  if (fromOutgoingPort && mOutPort)
    return mOutPort->anchorScenePos();
  if (!fromOutgoingPort && mInPort)
    return mInPort->anchorScenePos();

  QPointF center = sceneBoundingRect().center();
  QPointF dir = targetScenePos - center;

  if (dir.manhattanLength() < 0.001)
    return center;  // avoid divide by zero

  // Normalise and scale
  dir /= std::hypot(dir.x(), dir.y());
  qreal radius = boundingRect().width() / 2.0;
  return center + dir * radius;
}

bool NodeItem::canAddTransition() const
{
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  return config()->transitions.isEmpty() || index < config()->transitions.size();
}

TransitionConfig NodeItem::nextTransition() const
{
  // Only count the transitions coming from this
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  if (config()->transitions.isEmpty() || index >= config()->transitions.size())
    return TransitionConfig();

  return config()->transitions.at(index);
}

QVector<TransitionConfig> NodeItem::configTransitions() const
{
  return config()->transitions;
}

QVector<Flow*> NodeItem::flows() const
{
  return mFlows;
}

Flow* NodeItem::createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info)
{
  std::shared_ptr<FlowSaveInfo> flowConfig = info;
  bool found = false;
  if (info != nullptr)
  {
    // Clean up
    for (const auto& f : mStorage->getflows())
    {
      if (f->getid() != info->getid())
        continue;

      flowConfig = std::dynamic_pointer_cast<FlowSaveInfo>(f);
      found = true;
      break;
    }
  }
  else
  {
    flowConfig = std::make_shared<FlowSaveInfo>();
  }

  if (!found)
  {
    // Set this as owner of the flow
    flowConfig->setOwner(id());
    mStorage->addFlow(flowConfig);
  }

  Flow* flow = new Flow(flowName, flowConfig);
  mFlows.push_back(flow);

  if (flowAdded)
    flowAdded(flow, this);

  return flow;
}

Flow* NodeItem::getFlow(const QString& flowId) const
{
  for (const auto& flow : mFlows)
  {
    if (flow->id() == flowId)
      return flow;
  }

  return nullptr;
}

void NodeItem::deleteFlow(const QString& flowId)
{
  mStorage->removeFlow(flowId);
  mFlows.removeIf([flowId](Flow* flow) { return flow->id() == flowId; });
}

void NodeItem::updateFlow()
{
}
