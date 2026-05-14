#pragma once

#include "canvas.h"
#include "compiler/pipeline_graph.h"
#include "result.h"

namespace maki
{
struct PipelineGraph;
}

class PipelineCanvas : public Canvas
{
  Q_OBJECT
public:
  PipelineCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;
};
