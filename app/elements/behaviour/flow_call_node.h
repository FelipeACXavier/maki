#pragma once

#include <QRectF>

#include "elements/behaviour/behaviour_node.h"

class QPainter;

// namespace flow_call
// {
// inline constexpr const char* kTaskPropertyId = "task";
// inline constexpr int kFlowOptionIndex = 0;
// }  // namespace flow_call

// namespace flow_call_visual
// {
// /**
//  * Paints node_flow.svg into @p drawingBounds (slightly above center).
//  * @return The target rect used for the SVG, or empty if the icon could not be rendered.
//  */
// QRectF paintFlowIcon(QPainter* painter, const QRectF& drawingBounds);

// /** Local rect of the orange flow glyph (navigate target) inside @p drawingBounds. */
// QRectF flowIconLocalRect(const QRectF& drawingBounds);

// /** Local rect of the flow-name chip that opens the Task/Flow menu. */
// QRectF flowChipLocalRect(const QRectF& drawingBounds, const QString& chipText);

// void paintFlowChip(QPainter* painter, const QRectF& drawingBounds, const QString& chipText, bool hovered);
// }  // namespace flow_call_visual

class FlowCallNode : public BehaviourNode
{
public:
  FlowCallNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
               QGraphicsItem* parent = nullptr);

  std::pair<QString, QString> calledFlowId() const;

protected:
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
};