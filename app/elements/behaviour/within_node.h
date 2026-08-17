#pragma once

#include "elements/behaviour/subflow_block.h"

class WithinNode : public SubflowHostNode
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
  bool subflowsCollapsed() const override;
  void toggleSubflowsCollapsed() override;

private:
  void ensureSubflowBlocks();
  void setSubflowsCollapsed(bool collapsed);

  SubflowBlock* mDoBlock = nullptr;
  SubflowBlock* mElseBlock = nullptr;
};
