#include "behaviour_canvas.h"

#include "elements/flow.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(flow->id(), storage, configTable, parent)
    , mFlow(flow)
{
}

Types::LibraryTypes BehaviourCanvas::type() const
{
  return Types::LibraryTypes::BEHAVIOUR;
}

void BehaviourCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  if (mFlow == nullptr)
    return;

  if (adding)
    mFlow->updateFlow(node, storage);
  else
    mFlow->removeNode(node);
}
