#pragma once

#include <QRectF>

#include "elements/behaviour/behaviour_node.h"

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