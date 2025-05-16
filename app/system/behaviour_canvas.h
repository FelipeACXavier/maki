#pragma once

#include "canvas.h"

class BehaviourCanvas : public Canvas
{
public:
  BehaviourCanvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;
};
