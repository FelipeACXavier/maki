#pragma once

#include "elements/behaviour/behaviour_node.h"

class SubFlow;
class SubFlowMarker;

class RepeatNode : public BehaviourNode
{
public:
  RepeatNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
             QGraphicsItem* parent = nullptr);

  SubFlow* subFlow() const;

  void updatePosition(const QPointF& position) override;
  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info) override;
  void childRemoved(NodeItem* child) override;

protected:
  QPointF constrainChildPosition(const NodeItem* child, const QPointF& proposedPosition) const override;
  void childPositionUpdated() override;
  QRectF childAreaSceneRect() const override;
  bool constrainChildren() const override;

  virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
  void layoutSubFlow();
  void layoutSubFlowMarkers();

  SubFlow* mSubFlow{nullptr};

  void toggleCollapsed(SubFlow* flow, bool collapsed);
};
