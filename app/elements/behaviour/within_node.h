#pragma once

#include "elements/behaviour/behaviour_node.h"

class SubFlow;
class SubFlowMarker;

class WithinNode : public BehaviourNode
{
public:
  WithinNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
             QGraphicsItem* parent = nullptr);

  SubFlow* subFlow(const QString& subflow) const;

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
  SubFlow* mDoFlow{nullptr};
  SubFlow* mElseFlow{nullptr};

  void layoutSubFlows();
  void layoutSubFlowMarkers();

  QRectF childrenSceneRect(const QString& subFlowId) const;
  QList<SubFlow*> subFlows() const;

  bool hasExpandedSubFlow() const;
  QRectF expandedSubFlowsRect() const;
  QRectF expandedSubFlowsSceneRect() const;

  void toggleCollapsed(SubFlow* flow, bool collapsed);
};
