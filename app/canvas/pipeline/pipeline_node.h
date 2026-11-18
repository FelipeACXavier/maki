#pragma once

#include "canvas/behaviour/behaviour_node.h"

class PipelineNode : public BehaviourNode
{
public:
  PipelineNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
               QGraphicsItem* parent = nullptr);

  virtual ~PipelineNode();

  VoidResult start() override;

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  QSvgRenderer* mIconRenderer = nullptr;

  bool ensureRenderer();
};
