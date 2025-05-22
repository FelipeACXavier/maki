#include "flow.h"

#include "node.h"

Flow::Flow(const QString& name, std::shared_ptr<FlowSaveInfo> storage)
    : mId((!storage->id.isEmpty() && !storage->id.isNull()) ? storage->id : QUuid::createUuid().toString())
    , mName(name)
    , mStorage(storage)
{
  mStorage->id = this->id();
  mStorage->name = this->name();

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

void Flow::removeNode(NodeItem* node)
{
  mStorage->nodes.removeIf([node](std::shared_ptr<NodeSaveInfo> item) {
    return item->id == node->id();
  });
}

void Flow::updateFlow(NodeItem* /* node */, std::shared_ptr<NodeSaveInfo> storage)
{
  // for (auto& info : mInfo.behaviouralNodes)
  // {
  //   if (info.id != node->id())
  //     continue;

  //   info = node->saveInfo();
  //   return;
  // }

  // Add the node info directly to our shared knowledge
  mStorage->nodes.push_back(storage);
}

QVector<std::shared_ptr<NodeSaveInfo>> Flow::getNodes() const
{
  return mStorage->nodes;
}
