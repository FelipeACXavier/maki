#include "pipeline_canvas.h"

PipelineCanvas::PipelineCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, storage, configTable, parent)
{
}

Types::LibraryTypes PipelineCanvas::type() const
{
  return Types::LibraryTypes::PIPELINE;
}
