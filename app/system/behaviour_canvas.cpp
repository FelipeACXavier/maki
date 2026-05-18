#include "behaviour_canvas.h"

#include <qcoreapplication.h>
#include <qdir.h>

#include "elements/flow.h"
#include "logging.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(flow->id(), configTable, parent)
    , mFlow(flow)
{
}

Types::LibraryTypes BehaviourCanvas::type() const
{
  return Types::LibraryTypes::BEHAVIOUR;
}

void BehaviourCanvas::cleanFlow()
{
  if (mFlow)
    delete mFlow;
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

bool BehaviourCanvas::canAddTransition(NodeItem* node) const
{
  int index = 0;
  for (const auto& t : mFlow->transitions())
  {
    if (t->source()->id() == node->id())
      ++index;
  }

  LOG_INFO("canAddTransition: %d < %d", index, node->config()->transitions.size());
  return node->config()->transitions.isEmpty() || index <= node->config()->transitions.size();
}

TransitionConfig BehaviourCanvas::nextTransition(NodeItem* node) const
{
  int index = 0;
  for (const auto& t : mFlow->transitions())
  {
    if (t->source()->id() == node->id())
      ++index;
  }

  LOG_INFO("nextTransition: %d >= %d", index, node->config()->transitions.size());
  if (node->config()->transitions.isEmpty() || index >= node->config()->transitions.size())
    return TransitionConfig();

  return node->config()->transitions.at(index);
}

QVector<QGraphicsItem*> BehaviourCanvas::cleanTransitionsOfNode(const QString& nodeId)
{
  QVector<QGraphicsItem*> itemsToRemove = {};
  auto toDelete = mFlow->transitions();
  for (TransitionItem* transition : toDelete)
  {
    if (transition->source()->id() != nodeId &&
        transition->destination()->id() != nodeId)
      continue;

    mFlow->removeTransition(transition);
    removeItem(transition);
    itemsToRemove.append(transition);
  }

  return itemsToRemove;
}

void BehaviourCanvas::addTransition(TransitionItem* transition)
{
  LOG_INFO("Adding transition: %s %s", qPrintable(transition->getName()), qPrintable(transition->getEvent()));
  mFlow->addTransition(transition);
}

void BehaviourCanvas::removeTransition(TransitionItem* transition)
{
  mFlow->removeTransition(transition);
}

void BehaviourCanvas::onNodeMoved(const QString& nodeId)
{
  for (const auto& transition : mFlow->transitions())
  {
    if (transition->source()->id() == nodeId || transition->destination()->id() == nodeId)
      transition->updatePath();
  }
}