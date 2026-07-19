#include "elements/behaviour/sync_task_node.h"

#include "elements/behaviour/component_overlay.h"
#include "elements/port.h"

SyncTaskNode::SyncTaskNode(const QString& id,
                           std::shared_ptr<NodeSaveInfo> info,
                           const QPointF& initialPosition,
                           std::shared_ptr<NodeConfig> nodeConfig,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  // Sync has no abort port — only error above the node.
  if (mAbortPort)
  {
    mAbortPort->setVisible(false);
    updatePortPositions();
  }
}

void SyncTaskNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  behaviour::paintSelectedComponentOverlay(this, painter);
}
