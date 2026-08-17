#pragma once

#include "elements/behaviour/behaviour_node.h"

/** Behavioural Wait node: select a capability and author OUT-signal transitions from its out port. */
class WaitNode : public BehaviourNode
{
public:
  WaitNode(const QString& id,
           std::shared_ptr<NodeSaveInfo> info,
           const QPointF& initialPosition,
           std::shared_ptr<NodeConfig> nodeConfig,
           QGraphicsItem* parent = nullptr);

  void setProperty(const QString& key, QVariant value) override;

  bool hasCapabilitySelected() const;

  QRectF waitChipSceneRect() const;
  bool waitChipContainsScenePoint(const QPointF& scenePos) const;

  /** Assigns a capability from the canvas picker (name only; no mode/event). */
  void assignCapability(const QString& capabilityName);

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  enum class HoverTarget
  {
    None,
    Chip,
  };

  void syncWaitLabels();
  void clearOutgoingSignalEvents();
  qreal waitIconDiameter() const;
  QRectF chipLocalRect() const;
  void setHoverTarget(HoverTarget target);

  HoverTarget mHoverTarget = HoverTarget::None;
};
