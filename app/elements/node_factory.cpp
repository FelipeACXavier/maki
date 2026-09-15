#include "node_factory.h"

#include "elements/behaviour/continue_node.h"
#include "elements/behaviour/start_node.h"
#include "elements/behaviour/success_node.h"
#include "elements/behaviour/terminate_node.h"
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
  if (info->getnodeId() == "Koda::Async task")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Sync task")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Flow call")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Within")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Repeat")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  // Link nodes
  else if (info->getnodeId() == "Koda::LinkIn")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::LinkOut")
    return new NodeItem(id, info, initialPosition, nodeConfig, parent);
  // Header only nodes
  else if (info->getnodeId() == "Koda::Continue")
    return new ContinueNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Start")
    return new StartNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Success")
    return new SuccessNode(id, info, initialPosition, nodeConfig, parent);
  else if (info->getnodeId() == "Koda::Terminate")
    return new TerminateNode(id, info, initialPosition, nodeConfig, parent);
  // Structure
  else if (info->getnodeId() == "Koda::Task")
    return new TaskNode(id, info, initialPosition, nodeConfig, parent);
  else
    return new CapabilityNode(id, info, initialPosition, nodeConfig, parent);

  return nullptr;
}
