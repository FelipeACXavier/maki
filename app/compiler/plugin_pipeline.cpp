#include "plugin_pipeline.h"

#include "plugin_action_registry.h"

namespace maki
{
PluginPipeline::PluginPipeline(QObject* parent)
    : QObject(parent)
    , mRegistry(new PipelineActionRegistry())
{
}

PluginPipeline::~PluginPipeline()
{
  delete mRegistry;
}

PipelineActionRegistry* PluginPipeline::registry() const
{
  return mRegistry;
}

VoidResult PluginPipeline::run(const PipelineGraph& graph, PipelineContext& context)
{
  auto orderResult = executionOrder(graph);
  if (!orderResult)
    return VoidResult::Failed(orderResult.ErrorMessage());

  const auto order = orderResult.Value();
  for (const auto& nodeId : order)
  {
    auto node = findNode(graph, nodeId);
    if (!node)
      return VoidResult::Failed(QString("Pipeline node '%1' does not exist.").arg(nodeId).toStdString());

    auto action = mRegistry->action(node->actionId);
    if (!action)
      return VoidResult::Failed(QString("Pipeline action '%1' is not registered.").arg(node->actionId).toStdString());

    auto validation = validateInputs(*action, context);
    if (!validation)
      return validation;

    emit nodeStarted(node->id, action->displayName());

    auto result = action->run(context, node->parameters);
    if (!result)
    {
      emit nodeFailed(node->id, QString::fromStdString(result.ErrorMessage()));
      return result;
    }

    emit nodeFinished(node->id);
  }

  emit pipelineFinished();
  return VoidResult();
}

std::optional<PipelineNode> PluginPipeline::findNode(const PipelineGraph& graph, const QString& nodeId) const
{
  for (const auto& node : graph.nodes)
  {
    if (node.id == nodeId)
      return node;
  }

  return std::nullopt;
}

VoidResult PluginPipeline::validateInputs(const IPipelineAction& action, const PipelineContext& context) const
{
  for (const auto& requiredType : action.consumes())
  {
    if (!context.hasType(requiredType))
    {
      return VoidResult::Failed(
          QString("Action '%1' requires artefact type '%2', but none exists.")
              .arg(action.id(), requiredType)
              .toStdString());
    }
  }

  return VoidResult();
}

Result<QList<QString>> PluginPipeline::executionOrder(const PipelineGraph& graph) const
{
  // Here, we need to add the start node that gives type diagram

  QMap<QString, QList<QString>> outgoing;
  QMap<QString, int> indegree;

  for (const auto& node : graph.nodes)
    indegree.insert(node.id, 0);

  for (const auto& edge : graph.edges)
  {
    if (!indegree.contains(edge.from))
      return Result<QList<QString>>::Failed(QString("Unknown source node '%1'.").arg(edge.from).toStdString());

    if (!indegree.contains(edge.to))
      return Result<QList<QString>>::Failed(QString("Unknown target node '%1'.").arg(edge.to).toStdString());

    outgoing[edge.from].append(edge.to);
    indegree[edge.to]++;
  }

  QList<QString> queue;
  for (auto it = indegree.begin(); it != indegree.end(); ++it)
  {
    if (it.value() == 0)
      queue.append(it.key());
  }

  QList<QString> order;

  while (!queue.isEmpty())
  {
    const auto current = queue.takeFirst();
    order.append(current);

    for (const auto& next : outgoing.value(current))
    {
      indegree[next]--;

      if (indegree[next] == 0)
        queue.append(next);
    }
  }

  if (order.size() != graph.nodes.size())
    return Result<QList<QString>>::Failed("Pipeline graph contains a cycle.");

  return Result<QList<QString>>(order);
}
}  // namespace maki