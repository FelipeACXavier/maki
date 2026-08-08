#pragma once

#include "elements/behaviour/behaviour_node.h"

namespace link_nodes
{
inline const QString kLinkInType = QStringLiteral("Koda::Link in");
inline const QString kLinkOutType = QStringLiteral("Koda::Link out");
inline const QString kTargetPropertyId = QStringLiteral("target");
}  // namespace link_nodes

class LinkOutNode : public BehaviourNode
{
public:
  using BehaviourNode::BehaviourNode;

  /** Resolved Link in node id from the target property, or empty if unset. */
  QString targetId() const;

  /** Display name of the target Link in in this scene, or empty if unresolved. */
  QString targetName() const;

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
};
