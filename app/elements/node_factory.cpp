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
#include "logging.h"

NodeItem* NodeFactory::create(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                              QGraphicsItem* parent)
{
  if (!info)
    return nullptr;

  LOG_DEBUG("Creating: {}", info->getnodeId());

  // TODO: Stop using these fleamsy strings
  if (info->getnodeId() == "Koda::Call capability")
    return new CapabilityCallNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Flow call")
    return new FlowCallNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Within")
    return new WithinNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Repeat")
    return new RepeatNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Join")
    return new JoinNode(id, info, initialPosition, nodeConfig, parent);
  // Link nodes
  else if (info->getnodeId() == "Koda::Link in")
    return new LinkInNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Link out")
    return new LinkOutNode(id, info, initialPosition, nodeConfig, parent);
  // Header only nodes
  else if (info->getnodeId() == "Koda::Continue")
    return new ContinueNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Start")
    return new StartNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Success")
    return new SuccessNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Terminate")
    return new TerminateNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Failure")
    return new FailureNode(id, info, initialPosition, nodeConfig, parent);
  // Structure
  else if (info->getnodeId() == "Koda::Task")
    return new TaskNode(id, info, initialPosition, nodeConfig, parent);
  else
    return new CapabilityNode(id, info, initialPosition, nodeConfig, parent);

  return nullptr;
}
