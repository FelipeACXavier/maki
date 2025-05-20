#include "structure_canvas.h"

StructureCanvas::StructureCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, storage, configTable, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}
