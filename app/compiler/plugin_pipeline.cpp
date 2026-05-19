#include "plugin_pipeline.h"

#include <qobject.h>

#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>
#include <mutex>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include "logging.h"
#include "notifications.h"
#include "pipeline.h"
#include "pipeline_action.h"
#include "plugin_action_registry.h"
#include "result.h"

namespace maki
{
PluginPipeline::PluginPipeline(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mRegistry(new PipelineActionRegistry())
    , mPipeline(pipeline)
{
  connect(mPipeline, &Pipeline::finishedLast, [this](const Pipeline::Info& info, int exitCode, const QString& message) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
    // mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", nullptr);
    mOldWidgets.push_back(mPipeline->progressWidget(true));

    if (exitCode == 0)
    {
      mContext.commitPendingArtifact();
      QTimer::singleShot(0, this, [this]() { LOG_WARN_ON_FAILURE(continueAfterNode()); });
    }
    else
    {
      QTimer::singleShot(0, this, [this]() { done(""); });
    }
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](const Pipeline::Info& info, QProcess::ProcessError /* error */, const QString& message) {
    LOG_INFO("Error occurred: %s", qPrintable(message));
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
  });
  connect(mPipeline, &Pipeline::startingPipeline, [this](const Pipeline::Info& info) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
  });
  connect(mPipeline, &Pipeline::startingGroup, [this](const Pipeline::Info& info, const QString& groupName) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
  });
  connect(mPipeline, &Pipeline::processStarted, [this](const Pipeline::Info& info, const QString& process, const QStringList& /* arguments */) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
  });
  connect(mPipeline, &Pipeline::finishedGroup, [this](const Pipeline::Info& info, const QString& groupName, int exitCode, const QString& message) {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
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

bool PluginPipeline::isRunning() const
{
  std::unique_lock<std::mutex> lock(mStateMutex);
  return mRunning == State::Running;
}

VoidResult PluginPipeline::abort()
{
  if (!isRunning())
    return VoidResult();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mRunning = State::Aborting;
  }

  if (mPipeline->isRunning())
    RETURN_ON_FAILURE(mPipeline->abort());

  return VoidResult();
}

VoidResult PluginPipeline::runNextNode()
{
  if (!isRunning())
  {
    LOG_DEBUG("Run cancelled");
    return VoidResult();
  }

  if (mCurrentIndex >= mExecutionOrder.size())
  {
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());
    if (mPipeline)
      mOldWidgets.push_back(mPipeline->progressWidget(true));

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", nullptr);

    done(mContext.projectDir.absolutePath());
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

  for (const auto& arg : args)
    qDebug() << "Running with : " << arg;

  auto result = action->run(mContext, args, mPipeline);
  if (!result)
    return result;

  LOG_INFO("Done running action %s, pipeline size: %d", qPrintable(action->id()), mPipeline->size());
  if (mPipeline->size() == 0)
  {
    for (const auto& a : result.Value())
      mContext.addArtifact(a);

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Pipeline Progress", progressWidget());

    return continueAfterNode();
  }

  for (const auto& a : result.Value())
    mContext.addPendingArtifact(a);

  mPipeline->start();

  return VoidResult();
}

VoidResult PluginPipeline::continueAfterNode()
{
  const auto nodeId = mExecutionOrder.at(mCurrentIndex);
  ++mCurrentIndex;
  auto ran = runNextNode();
  if (!ran.IsSuccess())
    emit pipelineFinished("");

  return ran;
}

VoidResult PluginPipeline::run(const PipelineGraph& graph, PipelineContext& context)
{
  mGraph = graph;
  mContext = context;
  mCurrentIndex = 0;

  auto orderResult = executionOrder(graph);
  if (!orderResult)
    return VoidResult::Failed(orderResult.ErrorMessage());

  mOldWidgets.clear();
  mProgressId.clear();
  mExecutionOrder = orderResult.Value();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mRunning = State::Running;
  }

  emit pipelineStarted();

  auto ran = runNextNode();
  if (!ran.IsSuccess())
    done("");

  return ran;
}

void PluginPipeline::done(const QString& message)
{
  emit pipelineFinished(message);

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mRunning = State::Idle;
  }
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

QWidget* PluginPipeline::progressWidget() const
{
  QWidget* container = new QWidget();
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* layout = new QVBoxLayout(container);
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return container;

  const auto theme = qlementineStyle->theme();
  layout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  layout->setSpacing(theme.spacing);

  // For each group, we have
  auto* row = new QWidget(container);
  row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto* rowLayout = new QVBoxLayout(row);
  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->setSpacing(theme.spacing);

  // Header row: group name + count
  auto* header = new QWidget(row);
  header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 0, 0, 0);
  headerLayout->setSpacing(theme.spacing);

  auto* nameLabel = new oclero::qlementine::Label("Tasks", header);
  nameLabel->setRole(oclero::qlementine::TextRole::H4);
  nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  const auto completed = mCurrentIndex;
  const auto total = mExecutionOrder.size();
  auto* countLabel = new QLabel(QString("%1 / %2").arg(completed).arg(total), header);

  headerLayout->addWidget(nameLabel);
  headerLayout->addWidget(countLabel);

  // Progress bar
  auto* progress = new QProgressBar(row);
  progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  progress->setRange(0, total);
  progress->setValue(completed);
  progress->setTextVisible(false);

  // Current task label
  QWidget* currentTaskWidget = nullptr;
  if (mPipeline && mPipeline->size() > 0)
  {
    currentTaskWidget = mPipeline->progressWidget(true);
  }
  else if (mCurrentIndex < mExecutionOrder.size())
  {
    const auto nodeId = mExecutionOrder.at(mCurrentIndex);
    const auto node = findNode(mGraph, nodeId);
    currentTaskWidget = new oclero::qlementine::Label(node->displayName, row);
    qobject_cast<oclero::qlementine::Label*>(currentTaskWidget)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    qobject_cast<oclero::qlementine::Label*>(currentTaskWidget)->setWordWrap(false);
    qobject_cast<oclero::qlementine::Label*>(currentTaskWidget)->setRole(oclero::qlementine::TextRole::H5);
  }

  rowLayout->addWidget(header);
  rowLayout->addWidget(progress);
  for (QWidget* old : mOldWidgets)
    rowLayout->addWidget(old);
  if (currentTaskWidget)
    rowLayout->addWidget(currentTaskWidget);

  layout->addWidget(row);

  return container;
}
}  // namespace maki
