#include "plugin_pipeline.h"

#include <QTimer>

#include "logging.h"
#include "notifications.h"
#include "pipeline.h"
#include "pipeline_action.h"
#include "plugin_action_registry.h"

namespace maki
{
PluginPipeline::PluginPipeline(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mRegistry(new PipelineActionRegistry())
    , mPipeline(pipeline)
{
  connect(mPipeline, &Pipeline::finishedLast, [this](const Pipeline::Info& info, int exitCode, const QString& message) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", nullptr);

    if (exitCode == 0)
    {
      mContext.commitPendingArtifact();
      QTimer::singleShot(0, this, [this]() { LOG_WARN_ON_FAILURE(continueAfterNode()); });
    }
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](const Pipeline::Info& info, QProcess::ProcessError /* error */, const QString& message) {
    LOG_INFO("Error occurred: %s", qPrintable(message));
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
    mProgressId.clear();
  });
  connect(mPipeline, &Pipeline::startingPipeline, [this](const Pipeline::Info& info) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::startingGroup, [this](const Pipeline::Info& info, const QString& groupName) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::processStarted, [this](const Pipeline::Info& info, const QString& process, const QStringList& /* arguments */) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::finishedGroup, [this](const Pipeline::Info& info, const QString& groupName, int exitCode, const QString& message) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", mPipeline->progressWidget());
  });
}

PluginPipeline::~PluginPipeline()
{
  delete mRegistry;
}

PipelineActionRegistry* PluginPipeline::registry() const
{
  return mRegistry;
}

VoidResult PluginPipeline::runNextNode()
{
  if (mCurrentIndex >= mExecutionOrder.size())
  {
    emit pipelineFinished();
    return VoidResult();
  }

  const auto nodeId = mExecutionOrder.at(mCurrentIndex);
  const auto node = findNode(mGraph, nodeId);

  if (!node)
    return VoidResult::Failed("Pipeline node does not exist: " + nodeId.toStdString());

  auto action = mRegistry->action(node->actionId);
  if (!action)
    return VoidResult::Failed("Pipeline action is not registered: " + node->actionId.toStdString());

  auto validation = validateInputs(*action, mContext);
  if (!validation)
    return validation;

  auto args = node->parameters;
  if (args.isEmpty())
    args = action->defaultParameters();

  auto result = action->run(mContext, args, mPipeline);
  if (!result)
    return result;

  // for (const auto& art : mContext.artifacts())
  // {
  //   if (art.producer == "MAKI")
  //     continue;

  //   auto pretty = QJsonDocument(art.toJson()).toJson(QJsonDocument::Indented);
  //   LOG_DEBUG("Artifact %s %s: \n%s", qPrintable(art.id), qPrintable(art.producer), qPrintable(pretty));
  // }

  LOG_INFO("Done running action %s, pipeline size: %d", qPrintable(action->id()), mPipeline->size());
  if (mPipeline->size() == 0)
  {
    for (const auto& a : result.Value())
      mContext.addArtifact(a);

    return continueAfterNode();
  }

  for (const auto& a : result.Value())
    mContext.addPendingArtifact(a);

  mProgressId.clear();
  mPipeline->start();

  return VoidResult();
}

VoidResult PluginPipeline::continueAfterNode()
{
  const auto nodeId = mExecutionOrder.at(mCurrentIndex);
  ++mCurrentIndex;
  return runNextNode();
}

VoidResult PluginPipeline::run(const PipelineGraph& graph, PipelineContext& context)
{
  mGraph = graph;
  mContext = context;
  mCurrentIndex = 0;

  auto orderResult = executionOrder(graph);
  if (!orderResult)
    return VoidResult::Failed(orderResult.ErrorMessage());

  mExecutionOrder = orderResult.Value();
  return runNextNode();
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