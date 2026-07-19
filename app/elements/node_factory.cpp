#include "elements/node_factory.h"

#include <QHash>

#include "elements/behaviour/async_task_node.h"
#include "elements/behaviour/behaviour_node.h"
#include "elements/behaviour/call_node.h"
#include "elements/behaviour/continue_node.h"
#include "elements/behaviour/failure_node.h"
#include "elements/behaviour/flow_call_node.h"
#include "elements/behaviour/join_node.h"
#include "elements/behaviour/repeat_node.h"
#include "elements/behaviour/start_node.h"
#include "elements/behaviour/success_node.h"
#include "elements/behaviour/sync_task_node.h"
#include "elements/behaviour/terminate_node.h"
#include "elements/behaviour/within_node.h"
#include "elements/node.h"
#include "elements/structural_node.h"

namespace
{
QHash<QString, NodeFactory::Creator>& registry()
{
  static QHash<QString, NodeFactory::Creator> sRegistry;
  return sRegistry;
}

bool sBuiltinsRegistered = false;

template <typename TNode>
NodeItem* makeNode(const QString& id,
                   std::shared_ptr<NodeSaveInfo> info,
                   const QPointF& position,
                   std::shared_ptr<NodeConfig> config,
                   QGraphicsItem* parent)
{
  return new TNode(id, info, position, config, parent);
}

void registerKodaBehaviourTypes()
{
  NodeFactory::registerType(QStringLiteral("Koda::Start"), makeNode<StartNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Success"), makeNode<SuccessNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Failure"), makeNode<FailureNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Continue"), makeNode<ContinueNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Terminate"), makeNode<TerminateNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Call"), makeNode<CallNode>);
  // Legacy node ids (still used by older saves / Koda mental model)
  NodeFactory::registerType(QStringLiteral("Koda::Async task"), makeNode<AsyncTaskNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Sync task"), makeNode<SyncTaskNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Flow call"), makeNode<FlowCallNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Join"), makeNode<JoinNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Within"), makeNode<WithinNode>);
  NodeFactory::registerType(QStringLiteral("Koda::Repeat"), makeNode<RepeatNode>);
}
}  // namespace

void NodeFactory::registerType(const QString& typeKey, Creator creator)
{
  registry().insert(typeKey, creator);
}

NodeFactory::Creator NodeFactory::lookup(const QString& typeKey)
{
  return registry().value(typeKey, nullptr);
}

void NodeFactory::registerBuiltins()
{
  if (sBuiltinsRegistered)
    return;

  registerKodaBehaviourTypes();
  sBuiltinsRegistered = true;
}

NodeItem* NodeFactory::create(const QString& id,
                              std::shared_ptr<NodeSaveInfo> info,
                              const QPointF& position,
                              std::shared_ptr<NodeConfig> config,
                              QGraphicsItem* parent)
{
  registerBuiltins();

  if (config)
  {
    if (Creator creator = lookup(config->type))
      return creator(id, info, position, config, parent);

    switch (config->libraryType)
    {
      case Types::LibraryTypes::STRUCTURAL:
        return new StructuralNode(id, info, position, config, parent);
      case Types::LibraryTypes::BEHAVIOUR:
        return new BehaviourNode(id, info, position, config, parent);
      default:
        break;
    }
  }

  return new NodeItem(id, info, position, config, parent);
}
