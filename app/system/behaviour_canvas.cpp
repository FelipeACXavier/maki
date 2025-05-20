#include "behaviour_canvas.h"

#include "elements/flow.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(flow->id(), configTable, parent)
    , mFlow(flow)
{
}

Types::LibraryTypes BehaviourCanvas::type() const
{
  return Types::LibraryTypes::BEHAVIOUR;
}

void BehaviourCanvas::updateParent(NodeItem* node, bool adding)
{
  if (mFlow == nullptr)
    return;

  if (adding)
    mFlow->addNode(node);
  else
    mFlow->removeNode(node);
}

void BehaviourCanvas::updateParent(ConnectionItem* connection, bool adding)
{
  if (mFlow == nullptr)
    return;

  if (adding)
    mFlow->addConnection(connection);
  else
    mFlow->removeConnection(connection);
}
