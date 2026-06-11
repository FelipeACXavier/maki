#pragma once

#include "behaviour_canvas.h"

class PipelineCanvas : public BehaviourCanvas
{
  Q_OBJECT
public:
  PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent = nullptr);
  ~PipelineCanvas();

  Types::LibraryTypes type() const override;
};
