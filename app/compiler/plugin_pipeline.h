#pragma once

#include <QObject>
#include <QSet>

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

  std::optional<PipelineNode> findNode(const PipelineGraph& graph, const QString& nodeId) const;
  VoidResult validateInputs(const IPipelineAction& action, const PipelineContext& context) const;
  Result<QList<QString>> executionOrder(const PipelineGraph& graph) const;

  VoidResult runNextNode();
  VoidResult continueAfterNode();

  int mInitialCount = 0;
  QVector<QWidget*> mOldWidgets = {};
  QWidget* progressWidget() const;
};

}  // namespace maki