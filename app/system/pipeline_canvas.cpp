#include "pipeline_canvas.h"

#include <memory>

#include "compiler/pipeline_graph.h"
#include "elements/flow.h"
#include "flow_info.h"
#include "logging.h"
#include "result.h"
#include "system/behaviour_canvas.h"

PipelineCanvas::PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : BehaviourCanvas(new Flow(storage->getname(), storage), nullptr, configTable, parent)
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
