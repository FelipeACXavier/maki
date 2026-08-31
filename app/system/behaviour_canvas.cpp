#include "behaviour_canvas.h"

#include <QUndoStack>

#include "canvas_view.h"
#include "config_table.h"
#include "elements/flow.h"
#include "logging.h"
#include "undo_commands/insert_node.h"

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

  LOG_TRACE("canAddTransition: {} < {}", index, node->config()->transitions.size());
  return node->config()->transitions.isEmpty() || index <= node->config()->transitions.size();
}

TransitionConfig BehaviourCanvas::nextTransition(NodeItem* node) const
{
  int index = 0;
  for (const auto& t : mFlow->transitions())
    if (t->source()->id() == node->id())
      ++index;

  LOG_TRACE("nextTransition: {} >= {}", index, node->config()->transitions.size());
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
  }

  return itemsToRemove;
}

QVector<TransitionSaveInfo> BehaviourCanvas::transitionsOfNode(const QString& nodeId)
{
  QVector<TransitionSaveInfo> info = {};

  for (TransitionItem* transition : mFlow->transitions())
    if (transition->source()->id() == nodeId || transition->destination()->id() == nodeId)
      info.append(transition->saveInfo());

  return info;
}

void BehaviourCanvas::addTransition(TransitionItem* transition)
{
  LOG_TRACE("Adding transition: {} {}", transition->getName(), transition->getEvent());
  mFlow->addTransition(transition);
}

void BehaviourCanvas::removeTransition(TransitionItem* transition)
{
  if (transition != nullptr)
  {
    LOG_TRACE("Removing transition: {}", transition->id());
    mFlow->removeTransition(transition);
  }
}

void BehaviourCanvas::onNodeMoved(const NodeItem* node)
{
  if (!node)
    return;

  for (const auto& transition : mFlow->transitions())
    if (transition->source()->id() == node->id() || transition->destination()->id() == node->id())
      transition->updatePath();
}

bool BehaviourCanvas::insertDroppedNodeOnTransition(TransitionItem* transition, std::shared_ptr<NodeSaveInfo> info)
{
  if (!transition || !info)
    return false;

  NodeItem* source = transition->source();
  NodeItem* destination = transition->destination();
  if (!source || !destination || source == destination)
    return false;

  // The node already adjust the node size + label during creation, so we can just use the boundingRect
  const auto srcCenter = source->mapRectToScene(source->boundingRect()).center();
  const auto dstCenter = destination->mapRectToScene(destination->boundingRect()).center();
  const auto insertCenter = (srcCenter + dstCenter) * 0.5;

  auto originalTransition = transition->saveInfo();

  // Update the info before inserting
  const QString insertedNodeId = QUuid::createUuid().toString();
  info->setId(insertedNodeId);
  info->setPosition(insertCenter);

  // --------------------------------------------------------------------------
  // Source -> inserted node
  TransitionSaveInfo incoming;
  incoming.setId(QUuid::createUuid().toString());
  incoming.setEvent(originalTransition.getevent());
  incoming.setLabel(originalTransition.getlabel());

  incoming.setSrcId(source->id());
  incoming.setDstId(insertedNodeId);

  incoming.setSrcPoint(source->sceneNodeRect().center());
  incoming.setDstPoint(insertCenter);
  incoming.setSrcShift({0, 0});
  incoming.setDstShift({0, 0});

  // --------------------------------------------------------------------------
  // Inserted node -> destination
  const TransitionConfig outConfig =
      mConfigTable->get(info->getnodeId())->transitions.isEmpty() ? TransitionConfig{} : mConfigTable->get(info->getnodeId())->transitions.front();

  TransitionSaveInfo outgoing;
  outgoing.setId(QUuid::createUuid().toString());
  outgoing.setEvent(outConfig.event);
  outgoing.setLabel(outConfig.label);

  outgoing.setSrcId(insertedNodeId);
  outgoing.setDstId(destination->id());

  outgoing.setSrcPoint(insertCenter);
  outgoing.setDstPoint(destination->sceneNodeRect().center());
  outgoing.setSrcShift({0, 0});
  outgoing.setDstShift({0, 0});

  mUndoStack->push(new InsertNodeCommand(this, *info, originalTransition, incoming, outgoing));

  return true;
}
