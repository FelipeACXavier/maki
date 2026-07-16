#pragma once

#include "elements/behaviour/behaviour_node.h"

class FlowCallNode : public BehaviourNode
{
public:
  using BehaviourNode::BehaviourNode;

protected:
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
};
