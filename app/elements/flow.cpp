#include "flow.h"

#include <qdir.h>

#include "logging.h"
#include "node.h"
#include "system/behaviour_canvas.h"
#include "transition_info.h"

Flow::Flow(const QString& name, std::shared_ptr<FlowSaveInfo> storage)
    : mId((!storage->getid().isEmpty() && !storage->getid().isNull()) ? storage->getid() : QUuid::createUuid().toString())
    , mName(name)
    , mStorage(storage)
{
  mStorage->setId(this->id());
  mStorage->setName(this->name());
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
  if (!transition)
    return;

  const auto srcId = transition->sourceId();
  const auto dstId = transition->destinationId();

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

  found = false;
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
}

void Flow::removeTransition(TransitionItem* transition)
{
  mStorage->removeTransition(transition->storage());
  deleteTransition(transition);
}

void Flow::deleteTransition(TransitionItem* transition)
{
  LOG_INFO("Removing transition: %s", qPrintable(transition->id()));
  mTransitions.removeIf([transition](TransitionItem* item) { return item->id() == transition->id(); });
}
