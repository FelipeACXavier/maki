#include "flow.h"

#include <qdir.h>

#include "logging.h"
#include "node.h"
#include "transition_info.h"

Flow::Flow(const QString& name, std::shared_ptr<FlowSaveInfo> storage)
    : mId((!storage->getid().isEmpty() && !storage->getid().isNull()) ? storage->getid() : QUuid::createUuid().toString())
    , mName(name)
    , mStorage(storage)
{
  mStorage->setId(this->id());
  mStorage->setName(this->name());

  // auto startNode = std::make_shared<NodeSaveInfo>();
  // startNode->id = "Start";
  // startNode->nodeId = "Generic::Start";
  // startNode->position = {-250, 0};
  // startNode->size = {50, 50};
  // startNode->scale = {1.0};

  // auto endNode = std::make_shared<NodeSaveInfo>();
  // endNode->id = "End";
  // endNode->nodeId = "Generic::End";
  // endNode->position = {250, 0};
  // endNode->size = {50, 50};
  // endNode->scale = {1.0};

  // updateFlow(nullptr, startNode);
  // updateFlow(nullptr, endNode);
}

QString Flow::id() const
{
  return mId;
}

int Flow::type() const
{
  return Type;
}

QString Flow::name() const
{
  return mName;
}

bool Flow::modifiable() const
{
  return mStorage->getmodifiable();
}

void Flow::removeNode(NodeItem* node)
{
  mStorage->removeNode(std::make_shared<NodeSaveInfo>(node->saveInfo()));
}

void Flow::updateFlow(NodeItem* /* node */, std::shared_ptr<NodeSaveInfo> storage)
{
  // Add the node info directly to our shared knowledge
  mStorage->addNode(storage);
}

std::shared_ptr<FlowSaveInfo> Flow::config() const
{
  return mStorage;
}

QVector<std::shared_ptr<NodeSaveInfo>> Flow::getNodes() const
{
  QVector<std::shared_ptr<NodeSaveInfo>> out;
  out.reserve(mStorage->getnodes().size());
  for (const auto& f : mStorage->getnodes())
    out.push_back(std::static_pointer_cast<NodeSaveInfo>(f));

  return out;
}

QVector<TransitionItem*> Flow::transitions() const
{
  return mTransitions;
}

void Flow::addTransition(TransitionItem* transition)
{
  // Make sure the source node holds the transition info
  if (transition->destination() && (id() != transition->destination()->id()))
  {
    bool found = false;
    for (const auto& t : mStorage->gettransitions())
    {
      if (t->getid() == transition->id())
      {
        found = true;
        break;
      }
    }

    if (!found)
      mStorage->addTransition(transition->storage());

    for (auto& t : transitions())
    {
      // If I am the source of this transition
      // Check whether we have another transition with me as destination
      auto src1 = transition->source()->id();
      auto dst1 = transition->destination()->id();
      auto src2 = t->source()->id();
      auto dst2 = t->destination()->id();
      if (((src1 == dst2) && (src2 == dst1)))
      {
        transition->setEdge(TransitionItem::Edge::FORWARD);
        t->setEdge(TransitionItem::Edge::BACKWARD);
      }
    }
  }

  bool found = false;
  for (const auto& t : transitions())
  {
    if (t->id() == transition->id())
    {
      found = true;
      break;
    }
  }

  if (!found)
    mTransitions.push_back(transition);
}

void Flow::removeTransition(TransitionItem* transition)
{
  mStorage->removeTransition(transition->storage());
  mTransitions.removeIf([transition](TransitionItem* item) { return item->id() == transition->id(); });
}
