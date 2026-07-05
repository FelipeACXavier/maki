#include "structural_node.h"

#include <qhashfunctions.h>

#include <QFileInfo>
#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QHash>
#include <QJsonObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QUndoStack>
#include <algorithm>
#include <optional>
#include <utility>

#include "app_configs.h"
#include "flow.h"
#include "keys.h"
#include "logging.h"
#include "port.h"
#include "style_helpers.h"
#include "subtask_connector.h"
#include "system/canvas.h"
#include "system/config_table.h"
#include "system/structure_canvas.h"
#include "system/undo_commands/move_node.h"
#include "system/undo_commands/resize_node.h"
#include "system/undo_commands/swap_capabilities.h"
#include "types.h"

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

std::optional<QColor> optionalColorProperty(const QVariant& value)
{
  if (!value.isValid())
    return std::nullopt;

  const QString name = value.toString().trimmed();
  if (name.isEmpty() || !QColor::isValidColorName(name))
    return std::nullopt;

  return QColor::fromString(name);
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
constexpr qreal margin = 16.0;

/** Gap between slot-circle rows as fraction of slot diameter (shared with relayoutCapabilitySlots). */
constexpr qreal kSlotVerticalGapFactor = 0.35;
constexpr qreal kSlotHorizontalGapFactor = 0.35;

qreal taskSlotColumnStep(const QSizeF& size, qreal slotDiam)
{
  const qreal w = size.width();
  const qreal anchorW = qMin(w, kTaskAspectWidth);
  const qreal legacyStep = (kTaskSlotRightX - kTaskSlotLeftX) * anchorW;
  const qreal minStep = slotDiam * (1.0 + kSlotHorizontalGapFactor);
  return qMax(legacyStep, minStep);
}

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
  return margin + kTaskInnerPadding;
}

qreal minTaskWidthForSlots(qreal slotDiam, qreal colStep)
{
  return colStep + slotDiam + 2.0 * taskSlotTopInset();
}

qreal taskSlotBottomInset(qreal slotDiam)
{
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

  const qreal w = size.width();
  const qreal h = size.height();
  const qreal slotDiam = qMin(w, h) * kTaskSlotDiameterFactor;
  const qreal colStep = taskSlotColumnStep(size, slotDiam);

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

// ─────────────────────────────────────────────────────────────────────────────
// StructuralNode – constructor / destructor
// ─────────────────────────────────────────────────────────────────────────────

StructuralNode::StructuralNode(const QString& id,
                               std::shared_ptr<NodeSaveInfo> info,
                               const QPointF& initialPosition,
                               std::shared_ptr<NodeConfig> nodeConfig,
                               QGraphicsItem* parent)
    : NodeItem(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  setAcceptHoverEvents(true);
}

StructuralNode::~StructuralNode()
{
  destroySubtaskConnector();
}

// ─────────────────────────────────────────────────────────────────────────────
// Protected virtual overrides – size & ports
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::initializeNodeSize()
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Koda::Task"))
  {
    mSize = structural_layout::taskAspectSizeFromWidth(mSize.width());
    mStorage->setSize(mSize);
  }
}

void StructuralNode::configurePorts()
{
  // Structural nodes carry no flow-transition ports.
}

// ─────────────────────────────────────────────────────────────────────────────
// start
// ─────────────────────────────────────────────────────────────────────────────

VoidResult StructuralNode::start()
{
  const auto isCapabilityOrTask = isTaskContainer() || rendersAsInsetCapability();
  LOG_INFO("%s is cap: %d", qPrintable(nodeType()), isCapabilityOrTask);
  if (config()->body.nodeSvg.isEmpty() && !mStorage->getIcon().isEmpty() && !isCapabilityOrTask)
    setIcon(mStorage->getIcon(), config()->body.iconColor);

  return NodeBase::start();
}

// ─────────────────────────────────────────────────────────────────────────────
// paint / shape
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  const auto background = optionalColorProperty(getProperty(QStringLiteral("color"))).value_or(config()->body.backgroundColor);

  if (rendersAsInsetCapability())
  {
    Q_UNUSED(style);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
    painter->setPen(pen);
    painter->setBrush(QBrush(background));
    const QRectF r = nodeRect().adjusted(2, 2, -2, -2);
    painter->drawEllipse(r);
    if (!config()->body.iconPath.isEmpty())
    {
      const QString iconAbsPath = iconPathFromTheme(config()->body.iconPath);
      renderSvgInEllipse(painter, iconAbsPath, r.center(), qMin(r.width(), r.height()));
    }

    NodeBase::paintLabel(painter, pen);
    if (isSelected())
    {
      const QRectF r2 = nodeRect().adjusted(2, 2, -2, -2);
      QPen selPen(Config::HIGHLIGHT);
      selPen.setWidthF(2.0);
      selPen.setCosmetic(true);
      painter->setPen(selPen);
      painter->setBrush(Qt::NoBrush);
      painter->drawEllipse(r2);
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
    painter->setBrush(background);

    const QRectF bodyRect = nodeRect().adjusted(kTaskInnerPadding, kTaskInnerPadding, -kTaskInnerPadding, -kTaskInnerPadding);
    painter->drawRoundedRect(bodyRect, kTaskCornerRadius, kTaskCornerRadius);

    const qreal bbW = nodeRect().width();
    const qreal bbH = nodeRect().height();
    const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
    const qreal slotRadius = slotDiameter * 0.5;
    const int n = static_cast<int>(structuralCapabilityChildren().size());
    const QVector<QPointF> centers = structural_layout::taskSlotCenters(nodeRect().size(), n + 1);
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

    NodeBase::paintLabel(painter, pen);
    if (isSelected())
    {
      QPen selPen(Config::HIGHLIGHT);
      selPen.setWidthF(2.0);
      selPen.setCosmetic(true);
      painter->setPen(selPen);
      painter->setBrush(Qt::NoBrush);
      painter->drawRoundedRect(bodyRect, kTaskCornerRadius, kTaskCornerRadius);
    }
    return;
  }

  const QPen outlinePen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
  const bool isStructuralCapability = function() == Types::LibraryTypes::STRUCTURAL && !isTaskContainer();

  NodeBase::paintNode(nodeRect(), background, outlinePen, painter);

  if (isStructuralCapability && !config()->body.iconPath.isEmpty())
  {
    const QRectF r = nodeRect().adjusted(2, 2, -2, -2);
    renderSvgInEllipse(painter, iconPathFromTheme(config()->body.iconPath), r.center(), qMin(r.width(), r.height()));
  }

  if (isSelected() && !config()->body.nodeSvg.isEmpty())
    paintSelectionOutline(painter, nodeRect());
}

QPainterPath StructuralNode::shape() const
{
  if (rendersAsInsetCapability())
  {
    QPainterPath path;
    path.addEllipse(nodeRect().adjusted(2, 2, -2, -2));
    return path;
  }
  return NodeBase::nodeShape(nodeRect());
}

// ─────────────────────────────────────────────────────────────────────────────
// Parent / child management
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::addParent(NodeItem* parent)
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

void StructuralNode::childRemoved(NodeItem* child)
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

// ─────────────────────────────────────────────────────────────────────────────
// Structural-type queries
// ─────────────────────────────────────────────────────────────────────────────

bool StructuralNode::isTaskContainer() const
{
  return config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Koda::Task");
}

bool StructuralNode::isStructuralSubtask() const
{
  return isTaskContainer() && mParentNode && mParentNode->isTaskContainer();
}

bool StructuralNode::rendersAsInsetCapability() const
{
  return function() == Types::LibraryTypes::STRUCTURAL && mParentNode && mParentNode->isTaskContainer() && !isTaskContainer();
}

// ─────────────────────────────────────────────────────────────────────────────
// Child-collection accessors
// ─────────────────────────────────────────────────────────────────────────────

QVector<NodeItem*> StructuralNode::structuralSubtaskChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (c->isTaskContainer())
      out.append(c);
  }
  return out;
}

QVector<NodeItem*> StructuralNode::structuralCapabilityChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (!c->isTaskContainer())
      out.append(c);
  }
  return out;
}

// ─────────────────────────────────────────────────────────────────────────────
// Layout – tidy tree
// ─────────────────────────────────────────────────────────────────────────────

NodeItem* StructuralNode::rootStructuralTask() const
{
  NodeItem* root = const_cast<StructuralNode*>(this);
  NodeItem* parent = mParentNode;
  while (parent && parent->isTaskContainer())
  {
    root = parent;
    parent = parent->parentNode();
  }
  return root;
}

void StructuralNode::applyStructuralLayoutTopLeft(const QPointF& topLeftScene)
{
  prepareGeometryChange();
  setPos(topLeftScene);
  mLastPosition = topLeftScene;
  updateExtrasPosition();
  mStorage->setPosition(pos() + boundingRect().center());

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void StructuralNode::finalizeStructuralPackedPositions(const QPointF& subtreeSceneTopLeft)
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

void StructuralNode::layoutSubtasks()
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

// ─────────────────────────────────────────────────────────────────────────────
// Layout – capability slots
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::relayoutCapabilitySlots()
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

  const int numRows =
      slotCount <= 1 ? 1 : (slotCount + 1) / 2;  // ceil(slotCount/2) for slotCount >= 2

  qreal trialW = cfgW;
  qreal trialH = cfgH;
  constexpr int kSizingIters = 12;
  for (int iter = 0; iter < kSizingIters; ++iter)
  {
    const qreal minDim = qMin(trialW, trialH);
    const qreal sd = minDim * kTaskSlotDiameterFactor;
    const qreal verticalGap = sd * structural_layout::kSlotVerticalGapFactor;
    const qreal pitch = sd + verticalGap;
    const qreal topInset = structural_layout::taskSlotTopInset();
    const qreal bottomInset = structural_layout::taskSlotBottomInset(sd);
    qreal needH =
        topInset + sd + static_cast<qreal>(numRows - 1) * pitch + bottomInset;
    needH = qMax(cfgH, needH);

    qreal needW = trialW;
    if (slotCount >= 2)
    {
      const qreal colStep = structural_layout::taskSlotColumnStep(QSizeF(trialW, trialH), sd);
      needW = qMax(cfgW, structural_layout::minTaskWidthForSlots(sd, colStep));
    }

    if (needH <= trialH + 0.51 && needW <= trialW + 0.51)
      break;
    trialH = needH;
    trialW = needW;
  }
  const qreal reqW = trialW;
  qreal reqH = trialH;

  if (qAbs(reqW - W) > 0.5 || qAbs(reqH - H) > 0.5)
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

void StructuralNode::swapCapabilityOrder(NodeItem* a, NodeItem* b)
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

NodeItem* StructuralNode::capabilityAtScenePos(const QPointF& scenePos, NodeItem* exclude) const
{
  const QVector<NodeItem*> caps = structuralCapabilityChildren();
  const int slotCount = caps.size() + 1;
  if (slotCount <= 0)
    return nullptr;

  const qreal bbW = nodeRect().width();
  const qreal bbH = nodeRect().height();
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

// ─────────────────────────────────────────────────────────────────────────────
// Placeholder slot
// ─────────────────────────────────────────────────────────────────────────────

QRectF StructuralNode::placeholderSlotSceneRect() const
{
  if (!isTaskContainer())
    return {};

  const qreal bbW = nodeRect().width();
  const qreal bbH = nodeRect().height();
  const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
  const qreal slotRadius = slotDiameter * 0.5;
  const int n = static_cast<int>(structuralCapabilityChildren().size());
  const QVector<QPointF> centers = structural_layout::taskSlotCenters(boundingRect().size(), n + 1);
  if (centers.isEmpty())
    return {};

  const QPointF c = mapToScene(centers.last());
  return QRectF(c.x() - slotRadius, c.y() - slotRadius, slotDiameter, slotDiameter);
}

bool StructuralNode::placeholderSlotContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = placeholderSlotSceneRect();
  if (r.isEmpty())
    return false;
  const QPointF d = scenePos - r.center();
  const qreal rr = r.width() * 0.5;
  return (d.x() * d.x() + d.y() * d.y()) <= rr * rr;
}

// ─────────────────────────────────────────────────────────────────────────────
// SubtaskConnector management
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::ensureSubtaskConnector(StructureCanvas* canvas)
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

void StructuralNode::destroySubtaskConnector()
{
  if (!mSubtaskConnector)
    return;

  if (mSubtaskConnector->scene())
    mSubtaskConnector->scene()->removeItem(mSubtaskConnector);

  delete mSubtaskConnector;
  mSubtaskConnector = nullptr;
}

void StructuralNode::syncSubtaskConnector()
{
  if (mSubtaskConnector)
    mSubtaskConnector->syncGeometry();
}

// ─────────────────────────────────────────────────────────────────────────────
// Size & hover-preview
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);

  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  const qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabelSize(newFontSize, mSize);
  updatePortPositions();
  update();

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void StructuralNode::updatePosition(const QPointF& position)
{
  NodeItem::updatePosition(position);

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void StructuralNode::setHoverPreview(const QString& iconPath, const QColor& color, bool active)
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

// ─────────────────────────────────────────────────────────────────────────────
// Mouse events
// ─────────────────────────────────────────────────────────────────────────────

void StructuralNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void StructuralNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
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

void StructuralNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
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
    const QPointF myCenterScene = sceneNodeRect().center();
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

// ─────────────────────────────────────────────────────────────────────────────
// itemChange
// ─────────────────────────────────────────────────────────────────────────────

QVariant StructuralNode::itemChange(GraphicsItemChange change, const QVariant& value)
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
      QRectF parentRect = parent->nodeRect();
      parentRect = parentRect.adjusted(10, 10, -10, -10);
      parentRect.translate(parent->pos());

      // Child rect in its own coords
      QRectF childLocalRect = nodeRect();

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
