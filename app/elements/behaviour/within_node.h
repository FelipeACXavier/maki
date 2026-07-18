#pragma once

#include "elements/behaviour/behaviour_node.h"

class SubflowBlock;

class WithinNode : public BehaviourNode
{
public:
  WithinNode(const QString& id,
             std::shared_ptr<NodeSaveInfo> info,
             const QPointF& initialPosition,
             std::shared_ptr<NodeConfig> nodeConfig,
             QGraphicsItem* parent = nullptr);

  ~WithinNode() override;

  void updatePosition(const QPointF& position) override;
  QVector<NodeItem*> detachOwnedSubflowBlocks() override;

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
  void ensureSubflowBlocks();

  SubflowBlock* mDoBlock = nullptr;
  SubflowBlock* mElseBlock = nullptr;
};
