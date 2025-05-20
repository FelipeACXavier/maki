#include "behaviour_canvas.h"

BehaviourCanvas::BehaviourCanvas(const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, configTable, parent)
{
}

Types::LibraryTypes BehaviourCanvas::type() const
{
  return Types::LibraryTypes::BEHAVIOUR;
}
