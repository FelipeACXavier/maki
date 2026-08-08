#pragma once

#include "elements/behaviour/behaviour_node.h"

class SaveInfo;

/** Front-end Call node; backend codegen treats it as Async or Sync via call_mode. */
class CallNode : public BehaviourNode
{
public:
  CallNode(const QString& id,
           std::shared_ptr<NodeSaveInfo> info,
           const QPointF& initialPosition,
           std::shared_ptr<NodeConfig> nodeConfig,
           QGraphicsItem* parent = nullptr);

  void setProperty(const QString& key, QVariant value) override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  bool hasCapabilitySelected() const;
  QRectF capabilitySlotSceneRect() const;
  bool capabilitySlotContainsScenePoint(const QPointF& scenePos) const;

  QRectF eventChipSceneRect() const;
  bool eventChipContainsScenePoint(const QPointF& scenePos) const;

  /** Assigns a capability from the canvas picker and picks a sensible call_mode / event. */
  void assignCapability(const QString& capabilityName, const QString& capabilityId, SaveInfo* storage);

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  void syncNodeSvgFromState();
  void syncAbortPortFromMode();
  void syncCallLabels();
  void removeOutgoingAbortTransitions();
  qreal capabilitySlotDiameter() const;
  QRectF eventChipLocalRect() const;
  void paintEventChip(QPainter* painter) const;
  void setEmptySlotHovered(bool hovered);

  /** True while the pointer is over the empty (add-capability) slot. */
  bool mEmptySlotHovered = false;
};
