#include "elements/behaviour/flow_call_node.h"

#include <QFontMetricsF>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
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
constexpr qreal kFlowNameGap = 2.0;
/** Nudge the flow glyph slightly above geometric center. */
constexpr qreal kFlowIconUpNudgeFactor = 0.06;

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

QRectF FlowCallNode::flowIconSceneRect() const
{
  return mapRectToScene(flow_call_visual::flowIconLocalRect(drawingRect(nodeRect())));
}

bool FlowCallNode::flowIconContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = flowIconSceneRect();
  return r.isValid() && r.contains(scenePos);
}

void FlowCallNode::setFlowIconHovered(bool hovered)
{
  if (mFlowIconHovered == hovered)
    return;

  mFlowIconHovered = hovered;

  if (scene())
  {
    if (auto* view = dynamic_cast<QGraphicsView*>(scene()->parent()))
      view->setCursor(hovered ? Qt::PointingHandCursor : Qt::ArrowCursor);
  }

  setToolTip(hovered ? QObject::tr("Select task and flow") : QString());
}

void FlowCallNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  setFlowIconHovered(flowIconContainsScenePoint(event->scenePos()));
  QGraphicsItem::hoverMoveEvent(event);
}

void FlowCallNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setFlowIconHovered(false);
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

  const QString flowName = calledFlowName();
  if (flowName.isEmpty())
    return;

  const qreal iconBottom = target.top() + target.height() * (kNodeFlowIconBottomInViewBox / kNodeFlowViewBoxHeight);
  const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * config()->body.iconScale
                         * behaviour::kComponentOverlayDiameterFactor;
  QFont nameFont;
  nameFont.setPointSizeF(qBound(5.5, diameter * 0.16, 7.5));
  const QFontMetricsF fm(nameFont);
  const qreal textTop = iconBottom + kFlowNameGap;
  const qreal maxTextBottom = drawingBounds.bottom() - 1.0;
  const qreal textHeight = qMin(fm.height(), qMax(0.0, maxTextBottom - textTop));
  if (textHeight <= 0.0)
    return;

  const QRectF textRect(drawingBounds.left(), textTop, drawingBounds.width(), textHeight);
  painter->setFont(nameFont);
  painter->setPen(QPen(Config::FOREGROUND));
  painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, fm.elidedText(flowName, Qt::ElideRight, textRect.width()));
}

void FlowCallNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    if (auto* canvas = dynamic_cast<BehaviourCanvas*>(scene()))
      canvas->navigateToFlowCallTarget(this);
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}
