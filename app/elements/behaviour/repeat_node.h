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

  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info) override;
  void childRemoved(NodeItem* child) override;
  QRectF sceneAlignRect() const override;

  QRectF nodeRect() const override;
  QPainterPath shape() const override;

  void setProperty(const QString& key, const maki::Value& value) override;

protected:
  QRectF extraBoundingRect() const override;
  QPointF constrainChildPosition(const NodeItem* child, const QPointF& proposedPosition) const override;
  void childPositionUpdated(NodeItem* child) override;
  QRectF childAreaSceneRect(const QString& subflow) const override;
  bool constrainChildren() const override;
  void updateExtrasPosition(Config::NodeMove reason) override;

  void createPorts() override;
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
  void layoutSubFlow();
  void layoutSubFlowMarkers();

  SubFlow* mSubFlow{nullptr};

  void toggleCollapsed(SubFlow* flow, bool collapsed);
};
