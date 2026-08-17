#pragma once

#include "elements/behaviour/subflow_block.h"

class RepeatNode : public SubflowHostNode
{
public:
  RepeatNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  ~RepeatNode() override;

  void updatePosition(const QPointF& position) override;
  QVector<NodeItem*> detachOwnedSubflowBlocks() override;

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
  bool subflowsCollapsed() const override;
  void toggleSubflowsCollapsed() override;

private:
  void ensureSubflowBlock();
  void setSubflowsCollapsed(bool collapsed);

  SubflowBlock* mBlock = nullptr;
};
