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

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  qreal labelCenterOffsetX() const override;

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  void ensureSubflowBlocks();
  void setSubflowsCollapsed(bool collapsed);
  void toggleSubflowsCollapsed();
  bool subflowsCollapsed() const;

  SubflowBlock* mDoBlock = nullptr;
  SubflowBlock* mElseBlock = nullptr;
};
