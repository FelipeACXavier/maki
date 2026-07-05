#pragma once

#include "elements/behaviour/behaviour_node.h"

class SyncTaskNode : public BehaviourNode
{
public:
  using BehaviourNode::BehaviourNode;

protected:
  void paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
};
