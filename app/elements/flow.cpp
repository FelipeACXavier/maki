#include "flow.h"

#include "connection.h"
#include "node.h"

Flow::Flow(const QString& name)
    : mId(QUuid::createUuid().toString())
    , mName(name)
{
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

void Flow::addNode(NodeItem* node)
{
  mInfo.behaviouralNodes.append(node->saveInfo());
}

void Flow::removeNode(NodeItem* node)
{
  mInfo.behaviouralNodes.removeIf([node](NodeSaveInfo item) {
    return item.id == node->id();
  });
}

void Flow::addConnection(ConnectionItem* connection)
{
  mInfo.connections.append(connection->saveInfo());
}

void Flow::removeConnection(ConnectionItem* connection)
{
  mInfo.connections.removeIf([connection](ConnectionSaveInfo item) {
    return item.id == connection->id();
  });
}

void Flow::updateFlow(NodeItem* node)
{
  for (auto& info : mInfo.behaviouralNodes)
  {
    if (info.id != node->id())
      continue;

    info = node->saveInfo();
    return;
  }
}

QVector<NodeSaveInfo> Flow::getNodes() const
{
  return mInfo.behaviouralNodes;
}

QVector<ConnectionSaveInfo> Flow::getConnections() const
{
  return mInfo.connections;
}
