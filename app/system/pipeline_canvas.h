#pragma once

#include "canvas.h"

class PipelineCanvas : public Canvas
{
public:
  PipelineCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;
};
