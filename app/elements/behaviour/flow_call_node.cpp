#include "elements/behaviour/flow_call_node.h"

#include <QFontMetricsF>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPolygonF>
#include <QSvgRenderer>

#include "app_configs.h"
#include "elements/behaviour/component_overlay.h"
#include "keys.h"
#include "style_helpers.h"
#include "system/behaviour_canvas.h"

namespace
{
constexpr const char* kTaskPropertyId = "task";
constexpr int kTaskPropertyFlowIndex = 0;
/** Bottom of the orange flow glyph in node_flow.svg (viewBox height 240). */
constexpr qreal kNodeFlowIconBottomInViewBox = 162.76;
constexpr qreal kNodeFlowViewBoxWidth = 264.45;
constexpr qreal kNodeFlowViewBoxHeight = 240.0;
/** Orange button rect inside node_flow.svg viewBox. */
constexpr qreal kFlowGlyphX = 82.98;
constexpr qreal kFlowGlyphY = 77.24;
constexpr qreal kFlowGlyphW = 98.49;
constexpr qreal kFlowGlyphH = 85.52;
constexpr qreal kFlowNameGap = 3.0;
/** Nudge the flow glyph slightly above geometric center. */
constexpr qreal kFlowIconUpNudgeFactor = 0.08;
constexpr qreal kChipChevronGap = 3.0;
constexpr qreal kChipHitPad = 4.0;

QSvgRenderer& sharedFlowIconRenderer()
{
  static QSvgRenderer renderer(iconPathFromTheme(QStringLiteral("node_flow.svg")));
  return renderer;
}

QRectF fittedSvgTargetForViewBox(const QRectF& viewBox, const QRectF& drawingBounds)
{
  QRectF vb = viewBox;
  if (!vb.isValid() || vb.isEmpty())
    vb = QRectF(0, 0, 1, 1);

  constexpr qreal padding = 2.0;
  const QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
  const qreal sx = contentRect.width() / vb.width();
  const qreal sy = contentRect.height() / vb.height();
  const qreal scale = qMin(sx, sy);
  const QSizeF scaledSize(vb.width() * scale, vb.height() * scale);
  return QRectF(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0,
                contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
                scaledSize.width(),
                scaledSize.height());
}

QRectF fittedFlowSvgTarget(const QRectF& drawingBounds)
{
  QSvgRenderer& renderer = sharedFlowIconRenderer();
  QRectF viewBox = renderer.isValid() ? renderer.viewBoxF() : QRectF(0, 0, kNodeFlowViewBoxWidth, kNodeFlowViewBoxHeight);
  if (!viewBox.isValid() || viewBox.isEmpty())
    viewBox = QRectF(0, 0, kNodeFlowViewBoxWidth, kNodeFlowViewBoxHeight);

  QRectF target = fittedSvgTargetForViewBox(viewBox, drawingBounds);
  target.translate(0.0, -target.height() * kFlowIconUpNudgeFactor);
  return target;
}

QFont flowChipFont(const QRectF& drawingBounds)
{
  const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * 0.8
                         * behaviour::kComponentOverlayDiameterFactor;
  QFont font;
  font.setPointSizeF(qBound(5.5, diameter * 0.16, 7.5));
  return font;
}

qreal chipChevronSize(const QFontMetricsF& fm)
{
  return qMax(5.0, fm.height() * 0.45);
}

void paintDownChevron(QPainter* painter, const QRectF& rect)
{
  QPolygonF triangle;
  triangle << QPointF(rect.left(), rect.top() + rect.height() * 0.2)
           << QPointF(rect.right(), rect.top() + rect.height() * 0.2)
           << QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.15);
  painter->drawPolygon(triangle);
}
}  // namespace

namespace flow_call_visual
{
QRectF flowIconLocalRect(const QRectF& drawingBounds)
{
  if (!drawingBounds.isValid())
    return {};

  const QRectF target = fittedFlowSvgTarget(drawingBounds);
  if (!target.isValid())
    return {};

  const qreal sx = target.width() / kNodeFlowViewBoxWidth;
  const qreal sy = target.height() / kNodeFlowViewBoxHeight;
  return QRectF(target.x() + kFlowGlyphX * sx,
                target.y() + kFlowGlyphY * sy,
                kFlowGlyphW * sx,
                kFlowGlyphH * sy);
}

QRectF flowChipLocalRect(const QRectF& drawingBounds, const QString& chipText)
{
  if (!drawingBounds.isValid() || chipText.isEmpty())
    return {};

  const QRectF target = fittedFlowSvgTarget(drawingBounds);
  if (!target.isValid())
    return {};

  const QFont font = flowChipFont(drawingBounds);
  const QFontMetricsF fm(font);
  const qreal chevron = chipChevronSize(fm);
  const qreal maxW = qMax(0.0, drawingBounds.width() - 4.0);
  const qreal maxTextW = qMax(0.0, maxW - kChipChevronGap - chevron);
  const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
  const qreal textW = fm.horizontalAdvance(elided);
  const qreal chipW = qMin(maxW, textW + kChipChevronGap + chevron);
  const qreal chipH = fm.height();
  const qreal iconBottom = target.top() + target.height() * (kNodeFlowIconBottomInViewBox / kNodeFlowViewBoxHeight);
  QRectF chip(drawingBounds.center().x() - chipW * 0.5, iconBottom + kFlowNameGap, chipW, chipH);
  if (chip.bottom() > drawingBounds.bottom() - 1.0)
    chip.setBottom(drawingBounds.bottom() - 1.0);
  if (chip.height() < fm.height() * 0.6)
    return {};
  return chip;
}

QRectF paintFlowIcon(QPainter* painter, const QRectF& drawingBounds)
{
  if (!painter || !drawingBounds.isValid())
    return {};

  QSvgRenderer& iconRenderer = sharedFlowIconRenderer();
  if (!iconRenderer.isValid())
    return {};

  const QRectF target = fittedFlowSvgTarget(drawingBounds);
  painter->setRenderHint(QPainter::Antialiasing, true);
  iconRenderer.render(painter, target);
  return target;
}

void paintFlowChip(QPainter* painter, const QRectF& drawingBounds, const QString& chipText, bool hovered)
{
  if (!painter)
    return;

  const QRectF chip = flowChipLocalRect(drawingBounds, chipText);
  if (chip.isEmpty())
    return;

  const QFont font = flowChipFont(drawingBounds);
  const QFontMetricsF fm(font);
  const qreal chevron = chipChevronSize(fm);
  const qreal maxTextW = qMax(0.0, chip.width() - kChipChevronGap - chevron);
  const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
  const QColor color = hovered ? Config::HOVER : Config::FOREGROUND;

  painter->setRenderHint(QPainter::Antialiasing, true);
  const QRectF textRect(chip.left(), chip.top(), maxTextW, chip.height());
  painter->setFont(font);
  painter->setPen(QPen(color));
  painter->setBrush(Qt::NoBrush);
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

  const QRectF chevronRect(chip.right() - chevron, chip.center().y() - chevron * 0.5, chevron, chevron);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(color));
  paintDownChevron(painter, chevronRect);
}
}  // namespace flow_call_visual

FlowCallNode::FlowCallNode(const QString& id,
                           std::shared_ptr<NodeSaveInfo> info,
                           const QPointF& initialPosition,
                           std::shared_ptr<NodeConfig> nodeConfig,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptHoverEvents(true);
}

QString FlowCallNode::calledFlowName() const
{
  const QVariant propValue = getProperty(QString::fromLatin1(kTaskPropertyId));
  if (!propValue.isValid())
    return {};

  const QJsonArray options = propValue.toJsonObject().value(ConfigKeys::OPTIONS).toArray();
  if (options.size() <= kTaskPropertyFlowIndex)
    return {};

  const QString flowName = options.at(kTaskPropertyFlowIndex).toObject().value(ConfigKeys::DATA).toString().trimmed();
  if (flowName.isEmpty() || flowName == Constants::EMPTY_COMBO)
    return {};
  return flowName;
}

QString FlowCallNode::chipLabel() const
{
  const QString name = calledFlowName();
  return name.isEmpty() ? QObject::tr("Select flow") : name;
}

QRectF FlowCallNode::chipLocalRect() const
{
  return flow_call_visual::flowChipLocalRect(drawingRect(nodeRect()), chipLabel());
}

QRectF FlowCallNode::flowIconSceneRect() const
{
  return mapRectToScene(flow_call_visual::flowIconLocalRect(drawingRect(nodeRect())));
}

bool FlowCallNode::flowIconContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = flowIconSceneRect();
  return r.isValid() && r.contains(scenePos);
}

QRectF FlowCallNode::flowChipSceneRect() const
{
  return mapRectToScene(chipLocalRect());
}

bool FlowCallNode::flowChipContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = flowChipSceneRect();
  if (!r.isValid() || r.isEmpty())
    return false;
  return r.adjusted(-kChipHitPad, -kChipHitPad, kChipHitPad, kChipHitPad).contains(scenePos);
}

void FlowCallNode::setHoverTarget(HoverTarget target)
{
  if (mHoverTarget == target)
    return;

  mHoverTarget = target;

  if (scene())
  {
    if (auto* view = dynamic_cast<QGraphicsView*>(scene()->parent()))
    {
      const bool hand = target == HoverTarget::FlowIcon || target == HoverTarget::FlowChip;
      view->setCursor(hand ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
  }

  if (target == HoverTarget::FlowIcon)
    setToolTip(QObject::tr("Double-click to navigate to this flow."));
  else if (target == HoverTarget::FlowChip)
    setToolTip(QObject::tr("Select task and flow"));
  else
    setToolTip(QString());

  update();
}

void FlowCallNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  if (flowChipContainsScenePoint(event->scenePos()))
    setHoverTarget(HoverTarget::FlowChip);
  else if (!calledFlowName().isEmpty() && flowIconContainsScenePoint(event->scenePos()))
    setHoverTarget(HoverTarget::FlowIcon);
  else
    setHoverTarget(HoverTarget::None);
  QGraphicsItem::hoverMoveEvent(event);
}

void FlowCallNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setHoverTarget(HoverTarget::None);
  NodeItem::hoverLeaveEvent(event);
}

void FlowCallNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  if (!painter)
    return;

  const QRectF drawingBounds = drawingRect(nodeRect());
  const QRectF target = flow_call_visual::paintFlowIcon(painter, drawingBounds);
  if (!target.isValid())
    return;

  flow_call_visual::paintFlowChip(painter, drawingBounds, chipLabel(), mHoverTarget == HoverTarget::FlowChip);
}

void FlowCallNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && labelContainsScenePoint(event->scenePos()))
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  if (event->button() == Qt::LeftButton && !calledFlowName().isEmpty()
      && flowIconContainsScenePoint(event->scenePos()))
  {
    if (auto* canvas = dynamic_cast<BehaviourCanvas*>(scene()))
      canvas->navigateToFlowCallTarget(this);
    event->accept();
    return;
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}
