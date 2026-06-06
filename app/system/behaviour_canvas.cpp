#include "behaviour_canvas.h"

#include <qcoreapplication.h>
#include <qhashfunctions.h>

#include "canvas_view.h"
#include "config_table.h"
#include "elements/flow.h"
#include "logging.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(flow->id(), configTable, router, parent)
    , mFlow(flow)
{
}

BehaviourCanvas::~BehaviourCanvas()
{
  auto toDelete = mFlow->transitions();
  for (TransitionItem* transition : toDelete)
  {
    mFlow->deleteTransition(transition);
    removeItem(transition);
  }
}

void BehaviourCanvas::setupInitialNodes()
{
  // Add start and end nodes on creation
  if (mFlow && mFlow->getNodes().isEmpty())
  {
    const QRectF visible = parentView()->mapToScene(parentView()->viewport()->rect()).boundingRect();
    const qreal y = visible.center().y();
    const QPointF startPos{visible.left(), y};
    const QPointF endPos{visible.left() + visible.width(), y};
    addInitialNode("Koda::Start", startPos);
    addInitialNode("Koda::Success", endPos);
  }
}

void BehaviourCanvas::addInitialNode(const QString& nodeType, const QPointF& position)
{
  auto config = mConfigTable->get(nodeType);
  if (config == nullptr)
    return;

  auto info = std::make_shared<NodeSaveInfo>(*config);
  info->setPosition(position);
  mFlow->config()->addNode(info);
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
  LOG_INFO("Clean transitions with %d", toDelete.size());
  for (TransitionItem* transition : toDelete)
  {
    LOG_INFO("Cleaning? %s -> %s %s", qPrintable(nodeId), qPrintable(transition->source()->id()));
    if (transition->source()->id() != nodeId &&
        transition->destination()->id() != nodeId)
      continue;

    mFlow->deleteTransition(transition);
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

void BehaviourCanvas::onCleanChanged(bool state)
{
  if (mFlow)
    emit cleanChanged(id(), mFlow->name(), state);
}

void BehaviourCanvas::onCleanChanged(bool state)
{
  if (mFlow)
    emit cleanChanged(id(), mFlow->name(), state);
}
