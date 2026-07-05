#pragma once

#include "elements/behaviour/behaviour_node.h"

class AsyncTaskNode : public BehaviourNode
{
public:
  using BehaviourNode::BehaviourNode;

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
};
