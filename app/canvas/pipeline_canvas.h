#pragma once

#include "behaviour_canvas.h"
#include "document/save_info.h"
#include "pipeline/pipeline_graph.h"
#include "result.h"

namespace maki
{
struct PipelineGraph;
}

class PipelineCanvas : public BehaviourCanvas
{
  Q_OBJECT
public:
  PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<EdgeRouter> router, QObject* parent = nullptr);
  ~PipelineCanvas();

  Types::LibraryTypes type() const override;

  QString requestCreateNode(NodeSaveInfo info, NodeCreation creation) override;
  void requestRemoveNode(NodeSaveInfo info) override;

  void requestCreateTransition(TransitionSaveInfo info) override;
  void requestRemoveTransition(TransitionSaveInfo info) override;

  void onNodeHovered(NodeItem* node, bool entered) override;
};
