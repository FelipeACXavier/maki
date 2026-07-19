#pragma once

#include "elements/behaviour/behaviour_node.h"

class SyncTaskNode : public BehaviourNode
{
public:
  SyncTaskNode(const QString& id,
               std::shared_ptr<NodeSaveInfo> info,
               const QPointF& initialPosition,
               std::shared_ptr<NodeConfig> nodeConfig,
               QGraphicsItem* parent = nullptr);

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
};
