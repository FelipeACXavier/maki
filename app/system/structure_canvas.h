#pragma once

#include "canvas.h"

class StructureCanvas : public Canvas
{
public:
  StructureCanvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;
};
