#pragma once

#include "elements/node.h"

class TransitionItem;

/**
 * @brief Base class for behavioural flow nodes.
 */
class BehaviourNode : public NodeItem
{
public:
  BehaviourNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

protected:
  void initializeNodeSize() override;

  /** Override to add type-specific painting after the default body. */
  virtual void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget);

  /** Hook invoked when a transition is added to this node (no-op by default). */
  virtual void onTransitionAdded(TransitionItem* transition);
};
