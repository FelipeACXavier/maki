#include "flow.h"

#include "elements/save_info.h"
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

void Flow::updateFlow(NodeItem* node)
{
  mNodes[node->id()] = node->saveInfo();
}

void Flow::getNodes()
{
}
