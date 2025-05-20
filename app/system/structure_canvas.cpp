#include "structure_canvas.h"

StructureCanvas::StructureCanvas(const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, configTable, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}
