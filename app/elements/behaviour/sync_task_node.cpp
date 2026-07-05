#include "elements/behaviour/sync_task_node.h"

#include "elements/behaviour/component_overlay.h"

void SyncTaskNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  behaviour::paintSelectedComponentOverlay(this, painter);
}
