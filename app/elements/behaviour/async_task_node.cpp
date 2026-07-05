#include "elements/behaviour/async_task_node.h"

#include "elements/behaviour/component_overlay.h"

void AsyncTaskNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  behaviour::paintSelectedComponentOverlay(this, painter);
}
