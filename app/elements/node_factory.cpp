#include "node_factory.h"

#include "elements/behaviour/capability_call_node.h"
#include "elements/behaviour/continue_node.h"
#include "elements/behaviour/failure_node.h"
#include "elements/behaviour/flow_call_node.h"
#include "elements/behaviour/join_node.h"
#include "elements/behaviour/link_in_node.h"
#include "elements/behaviour/link_out_node.h"
#include "elements/behaviour/repeat_node.h"
#include "elements/behaviour/start_node.h"
#include "elements/behaviour/success_node.h"
#include "elements/behaviour/terminate_node.h"
#include "elements/behaviour/within_node.h"
#include "elements/structure/capability_node.h"
#include "elements/structure/task_node.h"
#include "keys.h"
#include "logging.h"

NodeItem* NodeFactory::create(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                              QGraphicsItem* parent)
{
  if (!info)
    return nullptr;

  LOG_DEBUG("Creating: {}", info->getnodeId());

  if (info->getnodeId() == ConfigKeys::CAPABILITY_CALL_NODE)
    return new CapabilityCallNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::FLOW_CALL_NODE)
    return new FlowCallNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::WITHIN_NODE)
    return new WithinNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::REPEAT_NODE)
    return new RepeatNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::JOIN_NODE)
    return new JoinNode(id, info, initialPosition, nodeConfig, parent);
  // Link nodes
  else if (info->getnodeId() == ConfigKeys::LINK_IN_NODE)
    return new LinkInNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::LINK_OUT_NODE)
    return new LinkOutNode(id, info, initialPosition, nodeConfig, parent);
  // Header only nodes
  else if (info->getnodeId() == ConfigKeys::CONTINUE_NODE)
    return new ContinueNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::START_NODE)
    return new StartNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::SUCCESS_NODE)
    return new SuccessNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::TERMINATE_NODE)
    return new TerminateNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == ConfigKeys::FAILURE_NODE)
    return new FailureNode(id, info, initialPosition, nodeConfig, parent);
  // Structure
  else if (info->getnodeId() == ConfigKeys::TASK_NODE)
    return new TaskNode(id, info, initialPosition, nodeConfig, parent);
  else
    return new CapabilityNode(id, info, initialPosition, nodeConfig, parent);

  return nullptr;
}
