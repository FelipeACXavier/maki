#include "elements/behaviour/flow_call_node.h"

#include <QFontMetricsF>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QSvgRenderer>

#include "app_configs.h"
#include "keys.h"
#include "mission_parameter.h"
#include "system/behaviour_canvas.h"

// namespace
// {
// /** Bottom of the orange flow glyph in node_flow.svg (viewBox height 240). */
// constexpr qreal kNodeFlowIconBottomInViewBox = 162.76;
// constexpr qreal kNodeFlowViewBoxWidth = 264.45;
// constexpr qreal kNodeFlowViewBoxHeight = 240.0;
// /** Orange button rect inside node_flow.svg viewBox. */
// constexpr qreal kFlowGlyphX = 82.98;
// constexpr qreal kFlowGlyphY = 77.24;
// constexpr qreal kFlowGlyphW = 98.49;
// constexpr qreal kFlowGlyphH = 85.52;
// constexpr qreal kFlowNameGap = 3.0;
// /** Nudge the flow glyph slightly above geometric center. */
// constexpr qreal kFlowIconUpNudgeFactor = 0.08;
// constexpr qreal kChipHitPad = 4.0;

// QSvgRenderer& sharedFlowIconRenderer()
// {
//   static QSvgRenderer renderer(iconPathFromTheme(QStringLiteral("node_flow.svg")));
//   return renderer;
// }

// QRectF fittedSvgTargetForViewBox(const QRectF& viewBox, const QRectF& drawingBounds)
// {
//   QRectF vb = viewBox;
//   if (!vb.isValid() || vb.isEmpty())
//     vb = QRectF(0, 0, 1, 1);

//   constexpr qreal padding = 2.0;
//   const QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
//   const qreal sx = contentRect.width() / vb.width();
//   const qreal sy = contentRect.height() / vb.height();
//   const qreal scale = qMin(sx, sy);
//   const QSizeF scaledSize(vb.width() * scale, vb.height() * scale);
//   return QRectF(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0, contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
//                 scaledSize.width(), scaledSize.height());
// }

// QRectF fittedFlowSvgTarget(const QRectF& drawingBounds)
// {
//   QSvgRenderer& renderer = sharedFlowIconRenderer();
//   QRectF viewBox = renderer.isValid() ? renderer.viewBoxF() : QRectF(0, 0, kNodeFlowViewBoxWidth, kNodeFlowViewBoxHeight);
//   if (!viewBox.isValid() || viewBox.isEmpty())
//     viewBox = QRectF(0, 0, kNodeFlowViewBoxWidth, kNodeFlowViewBoxHeight);

//   QRectF target = fittedSvgTargetForViewBox(viewBox, drawingBounds);
//   target.translate(0.0, -target.height() * kFlowIconUpNudgeFactor);
//   return target;
// }

// QFont flowChipFont(const QRectF& drawingBounds)
// {
//   const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * 0.8 * behaviour::kComponentOverlayDiameterFactor;
//   QFont font;
//   font.setPointSizeF(qBound(5.5, diameter * 0.16, 7.5));
//   return font;
// }
// }  // namespace

// namespace flow_call_visual
// {
// QRectF flowIconLocalRect(const QRectF& drawingBounds)
// {
//   if (!drawingBounds.isValid())
//     return {};

//   const QRectF target = fittedFlowSvgTarget(drawingBounds);
//   if (!target.isValid())
//     return {};

//   const qreal sx = target.width() / kNodeFlowViewBoxWidth;
//   const qreal sy = target.height() / kNodeFlowViewBoxHeight;
//   return QRectF(target.x() + kFlowGlyphX * sx, target.y() + kFlowGlyphY * sy, kFlowGlyphW * sx, kFlowGlyphH * sy);
// }

// QRectF flowChipLocalRect(const QRectF& drawingBounds, const QString& chipText)
// {
//   if (!drawingBounds.isValid() || chipText.isEmpty())
//     return {};

//   const QRectF target = fittedFlowSvgTarget(drawingBounds);
//   if (!target.isValid())
//     return {};

//   const QFont font = flowChipFont(drawingBounds);
//   const QFontMetricsF fm(font);
//   const qreal chevron = behaviour::callChipChevronSize(fm);
//   const qreal maxW = qMax(0.0, drawingBounds.width() - 4.0);
//   const qreal maxTextW = qMax(0.0, maxW - behaviour::kCallChipChevronGap - chevron);
//   const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
//   const qreal textW = fm.horizontalAdvance(elided);
//   const qreal chipW = qMin(maxW, textW + behaviour::kCallChipChevronGap + chevron);
//   const qreal chipH = fm.height();
//   const qreal iconBottom = target.top() + target.height() * (kNodeFlowIconBottomInViewBox / kNodeFlowViewBoxHeight);
//   QRectF chip(drawingBounds.center().x() - chipW * 0.5, iconBottom + kFlowNameGap, chipW, chipH);
//   if (chip.bottom() > drawingBounds.bottom() - 1.0)
//     chip.setBottom(drawingBounds.bottom() - 1.0);
//   if (chip.height() < fm.height() * 0.6)
//     return {};
//   return chip;
// }

// QRectF paintFlowIcon(QPainter* painter, const QRectF& drawingBounds)
// {
//   if (!painter || !drawingBounds.isValid())
//     return {};

//   QSvgRenderer& iconRenderer = sharedFlowIconRenderer();
//   if (!iconRenderer.isValid())
//     return {};

//   const QRectF target = fittedFlowSvgTarget(drawingBounds);
//   painter->setRenderHint(QPainter::Antialiasing, true);
//   iconRenderer.render(painter, target);
//   return target;
// }

// void paintFlowChip(QPainter* painter, const QRectF& drawingBounds, const QString& chipText, bool hovered)
// {
//   if (!painter)
//     return;

//   const QRectF chip = flowChipLocalRect(drawingBounds, chipText);
//   if (chip.isEmpty())
//     return;

//   const QFont font = flowChipFont(drawingBounds);
//   const QFontMetricsF fm(font);
//   const qreal chevron = behaviour::callChipChevronSize(fm);
//   const qreal maxTextW = qMax(0.0, chip.width() - behaviour::kCallChipChevronGap - chevron);
//   const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
//   const QColor color = hovered ? Config::HOVER : Config::FOREGROUND;

//   painter->setRenderHint(QPainter::Antialiasing, true);
//   const QRectF textRect(chip.left(), chip.top(), maxTextW, chip.height());
//   painter->setFont(font);
//   painter->setPen(QPen(color));
//   painter->setBrush(Qt::NoBrush);
//   painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

//   const QRectF chevronRect(chip.right() - chevron, chip.center().y() - chevron * 0.5, chevron, chevron);
//   painter->setPen(Qt::NoPen);
//   painter->setBrush(QBrush(color));
//   behaviour::paintDownChevron(painter, chevronRect);
// }
// }  // namespace flow_call_visual

FlowCallNode::FlowCallNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptHoverEvents(true);
}

QString FlowCallNode::calledFlowName() const
{
  const auto propValue = getProperty("task");
  if (!propValue || !propValue->getvalue())
    return QString();

  auto prop = maki::asValue(propValue->getvalue());
  if (!prop->isRecord())
    return QString();

  return maki::recordString(prop->toRecord(), "flow");
}

void FlowCallNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  // Show information about the called flow
  NodeItem::hoverMoveEvent(event);
}

void FlowCallNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  NodeItem::hoverLeaveEvent(event);
}

void FlowCallNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  const auto flowName = calledFlowName();
  if (event->button() != Qt::LeftButton && !flowName.isEmpty())
  {
    if (focusOn)
      focusOn(flowName, Types::FLOW);

    event->accept();
    return;
  }

  NodeItem::mouseDoubleClickEvent(event);
}