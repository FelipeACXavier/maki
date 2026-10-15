#include "flow.h"

#include "logging.h"
#include "macros.h"
#include "node.h"
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

void Flow::removeNode(const NodeItem* node)
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
  const auto& nodes = mStorage->getnodes();
  QVector<std::shared_ptr<NodeSaveInfo>> out;
  out.reserve(nodes.size());
  maki::transform(nodes, out, [](const auto& node) { return std::static_pointer_cast<NodeSaveInfo>(node); });
  return out;
}

QVector<TransitionItem*> Flow::transitions() const
{
  return mTransitions;
}

// cppcheck-suppress constParameterPointer
void Flow::addTransition(TransitionItem* transition)
{
  if (!transition)
    return;

  const auto transitionId = transition->id();
  if (!maki::anyOf(mStorage->gettransitions(), [&transitionId](const auto& t) { return t->getid() == transitionId; }))
    mStorage->addTransition(transition->storage());

  if (!maki::anyOf(transitions(), [&transitionId](const auto& t) { return t->id() == transitionId; }))
    mTransitions.push_back(transition);
}

void Flow::removeTransition(TransitionItem* transition)
{
  if (!transition)
  {
    LOG_DEBUG("Tried removing unknown transition");
    return;
  }

  mStorage->removeTransition(transition->storage());
  mTransitions.removeIf([transition](const TransitionItem* item) { return item->id() == transition->id(); });
}
