#pragma once

#include <QObject>
#include <QSet>
#include <mutex>

#include "pipeline_action.h"
#include "pipeline_context.h"
#include "pipeline_graph.h"
#include "result.h"

class Pipeline;

namespace maki
{
class PipelineActionRegistry;

class PluginPipeline : public QObject
{
  Q_OBJECT

public:
  PluginPipeline(Pipeline* pipeline, QObject* parent = nullptr);
  ~PluginPipeline();

  VoidResult run(const PipelineGraph& graph, PipelineContext& context);

  PipelineActionRegistry* registry() const;
  bool isRunning() const;
  VoidResult abort();

signals:
  void nodeStarted(const QString& nodeId, const QString& displayName);
  void nodeFinished(const QString& nodeId);
  void nodeFailed(const QString& nodeId, const QString& error);
  void pipelineStarted();
  void pipelineFinished(const QString& generationDir);

private:
  PipelineActionRegistry* mRegistry = nullptr;
  Pipeline* mPipeline;
  QStringList mExecutionOrder;
  int mCurrentIndex;
  QString mCurrentGroup;
  PipelineContext mContext;
  PipelineGraph mGraph;
  QString mProgressId;

  mutable std::mutex mStateMutex;
  enum class State
  {
    Idle,
    Running,
    Aborting
  } mRunning;

  std::optional<PipelineNode> findNode(const PipelineGraph& graph, const QString& nodeId) const;
  VoidResult validateInputs(const IPipelineAction& action, const PipelineContext& context) const;
  Result<QList<QString>> executionOrder(const PipelineGraph& graph) const;

  VoidResult runNextNode();
  VoidResult continueAfterNode();
  void done(const QString& message);

  int mInitialCount = 0;
  QVector<QWidget*> mOldWidgets = {};
  QWidget* progressWidget() const;
};

}  // namespace maki
