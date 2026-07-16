#include "elements/behaviour/flow_call_node.h"

#include <QGraphicsSceneMouseEvent>

#include "system/behaviour_canvas.h"

void FlowCallNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    if (auto* canvas = dynamic_cast<BehaviourCanvas*>(scene()))
      canvas->navigateToFlowCallTarget(this);
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}
