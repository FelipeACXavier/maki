#include "pipeline_canvas.h"

#include <memory>

#include "canvas/behaviour_canvas.h"
#include "document/flow_info.h"

PipelineCanvas::PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : BehaviourCanvas(storage, "", router, parent)
{
}

PipelineCanvas::~PipelineCanvas()
{
  cleanFlow();
}

Types::LibraryTypes PipelineCanvas::type() const
{
  return Types::LibraryTypes::PIPELINE;
}
