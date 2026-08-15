#pragma once

#include <QRectF>

#include "elements/behaviour/behaviour_node.h"

class QPainter;

namespace flow_call_visual
{
/**
 * Paints node_flow.svg into @p drawingBounds (slightly above center).
 * @return The target rect used for the SVG, or empty if the icon could not be rendered.
 */
QRectF paintFlowIcon(QPainter* painter, const QRectF& drawingBounds);

/** Local rect of the orange flow glyph (clickable button area) inside @p drawingBounds. */
QRectF flowIconLocalRect(const QRectF& drawingBounds);

/**
 * Local rect for node_flow_arrow.svg, placed to the right of the orange flow glyph.
 * Empty when the flow icon rect is invalid.
 */
QRectF navigateArrowLocalRect(const QRectF& drawingBounds);

/** Paints the navigate arrow at @ref navigateArrowLocalRect. */
void paintNavigateArrow(QPainter* painter, const QRectF& drawingBounds);
}  // namespace flow_call_visual

class FlowCallNode : public BehaviourNode
{
public:
  FlowCallNode(const QString& id,
               std::shared_ptr<NodeSaveInfo> info,
               const QPointF& initialPosition,
               std::shared_ptr<NodeConfig> nodeConfig,
               QGraphicsItem* parent = nullptr);

  /** Selected flow name from the task property, or empty if unset. */
  QString calledFlowName() const;

  QRectF flowIconSceneRect() const;
  bool flowIconContainsScenePoint(const QPointF& scenePos) const;

  QRectF navigateArrowSceneRect() const;
  bool navigateArrowContainsScenePoint(const QPointF& scenePos) const;

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
  enum class HoverTarget
  {
    None,
    FlowIcon,
    NavigateArrow,
  };

  void setHoverTarget(HoverTarget target);

  HoverTarget mHoverTarget = HoverTarget::None;
};
