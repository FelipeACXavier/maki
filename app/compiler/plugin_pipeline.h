#pragma once

#include <QObject>
#include <QSet>

#include "pipeline_action.h"
#include "pipeline_context.h"
#include "pipeline_graph.h"
#include "result.h"

namespace maki
{
class PipelineActionRegistry;

class PluginPipeline : public QObject
{
  Q_OBJECT

public:
  PluginPipeline(QObject* parent = nullptr);
  ~PluginPipeline();

  VoidResult run(const PipelineGraph& graph, PipelineContext& context);

  PipelineActionRegistry* registry() const;

signals:
  void nodeStarted(const QString& nodeId, const QString& displayName);
  void nodeFinished(const QString& nodeId);
  void nodeFailed(const QString& nodeId, const QString& error);
  void pipelineFinished();

private:
  PipelineActionRegistry* mRegistry = nullptr;

  std::optional<PipelineNode> findNode(const PipelineGraph& graph, const QString& nodeId) const;
  VoidResult validateInputs(const IPipelineAction& action, const PipelineContext& context) const;
  Result<QList<QString>> executionOrder(const PipelineGraph& graph) const;
};

}  // namespace maki