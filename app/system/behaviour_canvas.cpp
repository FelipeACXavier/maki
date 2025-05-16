#include "behaviour_canvas.h"

BehaviourCanvas::BehaviourCanvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(configTable, parent)
{
}

Types::LibraryTypes BehaviourCanvas::type() const
{
  return Types::LibraryTypes::BEHAVIOUR;
}
