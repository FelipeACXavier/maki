#include "pipeline_canvas.h"

#include <memory>

#include "flow.h"
#include "flow_info.h"

PipelineCanvas::PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : BehaviourCanvas(new Flow(storage->getname(), storage), configTable, router, parent)
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
