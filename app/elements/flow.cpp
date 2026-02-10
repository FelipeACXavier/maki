#include "flow.h"

#include "node.h"

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

QVector<std::shared_ptr<NodeSaveInfo>> Flow::getNodes() const
{
  QVector<std::shared_ptr<NodeSaveInfo>> out;
  out.reserve(mStorage->getnodes().size());
  for (const auto& f : mStorage->getnodes())
    out.push_back(std::static_pointer_cast<NodeSaveInfo>(f));

  return out;
}
