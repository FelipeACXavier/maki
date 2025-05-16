#include "structure_canvas.h"

StructureCanvas::StructureCanvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(configTable, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}
