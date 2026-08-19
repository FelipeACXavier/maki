#include "behaviour_canvas.h"

#include "canvas_view.h"
#include "config_table.h"
#include "elements/flow.h"
#include "logging.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(flow->id(), configTable, router, parent)
    , mFlow(flow)
{
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
    if (t->source()->id() == node->id())
      ++index;

  LOG_INFO("canAddTransition: {} < {}", index, node->config()->transitions.size());
  return node->config()->transitions.isEmpty() || index <= node->config()->transitions.size();
}

TransitionConfig BehaviourCanvas::nextTransition(NodeItem* node) const
{
  int index = 0;
  for (const auto& t : mFlow->transitions())
    if (t->source()->id() == node->id())
      ++index;

  LOG_INFO("nextTransition: {} >= {}", index, node->config()->transitions.size());
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
    if (transition->source()->id() != nodeId && transition->destination()->id() != nodeId)
      continue;

    mFlow->removeTransition(transition);
    removeItem(transition);
    itemsToRemove.append(transition);
  }

  return itemsToRemove;
}

void BehaviourCanvas::addTransition(TransitionItem* transition)
{
  LOG_INFO("Adding transition: {} {}", transition->getName(), transition->getEvent());
  mFlow->addTransition(transition);
}

void BehaviourCanvas::removeTransition(TransitionItem* transition)
{
  mFlow->removeTransition(transition);
}

void BehaviourCanvas::onNodeMoved(const NodeItem* node)
{
  if (!node)
    return;

  for (const auto& transition : mFlow->transitions())
    if (transition->source()->id() == node->id() || transition->destination()->id() == node->id())
      transition->updatePath();
}
