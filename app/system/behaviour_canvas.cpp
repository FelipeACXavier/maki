#include "behaviour_canvas.h"

#include <QUndoStack>

#include "canvas_view.h"
#include "elements/flow.h"
#include "keys.h"
#include "logging.h"
#include "undo_commands/insert_existing_node.h"
#include "undo_commands/insert_node.h"

static constexpr qreal MIN_NODE_SPACING = 40.0;

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(flow->id(), router, parent)
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
    auto src = addInitialNode(ConfigKeys::START_NODE, startPos);
    auto dst = addInitialNode(ConfigKeys::SUCCESS_NODE, endPos);

    if (!(src && dst))
      return;

    auto initial = std::make_shared<TransitionSaveInfo>();
    initial->setId(QUuid::createUuid().toString());

    initial->setSrcId(src->getid());
    initial->setDstId(dst->getid());

    initial->setSrcPoint(src->getposition());
    initial->setDstPoint(dst->getposition());
    initial->setSrcShift({0, 0});
    initial->setDstShift({0, 0});

    mFlow->config()->addTransition(initial);
  }
}

std::shared_ptr<const NodeSaveInfo> BehaviourCanvas::addInitialNode(const QString& nodeType, const QPointF& position)
{
  auto config = getNodeConfig(nodeType);
  if (config == nullptr)
    return nullptr;

  auto info = std::make_shared<NodeSaveInfo>(*config);
  info->setId(QUuid::createUuid().toString());
  info->setPosition(position);
  mFlow->config()->addNode(info);

  return info;
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

void BehaviourCanvas::nodeStarted(NodeItem* node)
{
  if (node && node->nodeId() == ConfigKeys::REPEAT_NODE)
  {
    if (!node->children().isEmpty())
      return;

    TransitionSaveInfo initial;

    initial.setId(QUuid::createUuid().toString());

    initial.setSrcId(node->id());
    initial.setDstId(node->id());

    initial.setSrcPort(Types::Port::IN);
    initial.setDstPort(Types::Port::OUT);

    if (auto* inPort = node->getPort(Types::Port::IN))
      initial.setSrcPoint(inPort->anchorScenePos());

    if (auto* outPort = node->getPort(Types::Port::OUT))
      initial.setDstPoint(outPort->anchorScenePos());

    initial.setSrcShift({0, 0});
    initial.setDstShift({0, 0});

    createTransition(initial);
  }
  else if (node && node->nodeId() == ConfigKeys::WITHIN_NODE)
  {
    if (!node->children().isEmpty())
      return;
    {
      TransitionSaveInfo initial;
      initial.setId(QUuid::createUuid().toString());
      initial.setSrcId(node->id());
      initial.setDstId(node->id());
      initial.setSrcPort(Types::Port::IN);
      initial.setDstPort(Types::Port::OUT);
      if (auto* inPort = node->getPort(Types::Port::IN, "do"))
      {
        initial.setSrcPoint(inPort->anchorScenePos());
        initial.setSrcPortSubFlow(inPort->subFlow());
      }
      if (auto* outPort = node->getPort(Types::Port::OUT))
        initial.setDstPoint(outPort->anchorScenePos());
      initial.setSrcShift({0, 0});
      initial.setDstShift({0, 0});
      createTransition(initial);
    }
    {
      TransitionSaveInfo initial;
      initial.setId(QUuid::createUuid().toString());
      initial.setSrcId(node->id());
      initial.setDstId(node->id());
      initial.setSrcPort(Types::Port::IN);
      initial.setDstPort(Types::Port::OUT);
      if (auto* inPort = node->getPort(Types::Port::IN, "else"))
      {
        initial.setSrcPoint(inPort->anchorScenePos());
        initial.setSrcPortSubFlow(inPort->subFlow());
      }
      if (auto* outPort = node->getPort(Types::Port::OUT))
        initial.setDstPoint(outPort->anchorScenePos());
      initial.setSrcShift({0, 0});
      initial.setDstShift({0, 0});
      createTransition(initial);
    }
  }

  Canvas::nodeStarted(node);
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

bool BehaviourCanvas::canAddTransition(NodeItem* node, PortItem* port) const
{
  if (!node || !port)
    return false;

  // Expanded sub-flow input marker may start the internal flow.
  if (port->isMarker())
    return true;

  int index = 0;
  for (const auto& t : mFlow->transitions())
  {
    if (t->source()->id() != node->id())
      continue;

    if ((port->isAbort() || port->isError()) && t->getEvent() == port->defaultTransitionEvent())
      return false;

    ++index;
  }

  // LOG_TRACE("canAddTransition: {} <= {}", index, node->config()->transitions.size());
  return node->config()->transitions.isEmpty() || index <= node->config()->transitions.size();
}

TransitionConfig BehaviourCanvas::nextTransition(NodeItem* node) const
{
  int index = 0;
  for (const auto& t : mFlow->transitions())
    if (t->source()->id() == node->id())
      ++index;

  // LOG_TRACE("nextTransition: {} >= {}", index, node->config()->transitions.size());
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
    itemsToRemove.append(transition);
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
  // LOG_TRACE("Adding transition: {} {}", transition->getName(), transition->getEvent());
  mFlow->addTransition(transition);
}

void BehaviourCanvas::removeTransition(TransitionItem* transition)
{
  if (transition != nullptr)
  {
    // LOG_TRACE("Removing transition: {}", transition->id());
    mFlow->removeTransition(transition);
  }
}

void BehaviourCanvas::onSubFlowCollapsed(NodeItem* owner, const QString& subflowId, bool collapsed)
{
  if (!owner || !mFlow)
    return;

  for (auto* transition : mFlow->transitions())
  {
    if (!transition)
      continue;

    auto* source = transition->source();
    auto* destination = transition->destination();
    if (!source || !destination)
      continue;

    const auto storage = transition->storage();
    if (!storage)
      continue;

    // Nodes that semantically live inside this particular subflow.
    const bool sourceDescendant = isDescendantOf(source, owner, subflowId);
    const bool destinationDescendant = isDescendantOf(destination, owner, subflowId);

    // Special case:
    //
    // Some subflows have an internal owner -> owner transition, e.g.
    //
    //   Repeat.body.IN -> Repeat.body.OUT
    //
    // or:
    //
    //   Within.do.IN -> Within.body.OUT
    //
    // These transitions belong to the expanded subflow even though neither
    // endpoint is represented by a child NodeItem.
    const bool ownerInternalTransition =
        source == owner && destination == owner && (storage->srcPortSubFlow() == subflowId || storage->dstPortSubFlow() == subflowId);

    const bool belongsToSubFlow = sourceDescendant || destinationDescendant || ownerInternalTransition;
    if (!belongsToSubFlow)
      continue;

    LOG_DEBUG("Subflow '{}': {}[{}] -> {}[{}] | descendant={} {} ownerInternal={} collapsed={}", subflowId, source->nodeId(), storage->srcPortSubFlow(),
              destination->nodeId(), storage->dstPortSubFlow(), sourceDescendant, destinationDescendant, ownerInternalTransition, collapsed);

    transition->setVisible(!collapsed);
  }

  onNodeMoved(owner, true);
}

QList<NodeItem*> BehaviourCanvas::getNeighboursOf(const NodeItem* node, int depth) const
{
  QList<NodeItem*> result;

  if (!node)
    return result;

  QSet<const NodeItem*> visited;
  QList<QPair<const NodeItem*, int>> pending;

  visited.insert(node);
  pending.append({node, 0});

  while (!pending.isEmpty())
  {
    const auto [current, currentDepth] = pending.takeFirst();

    if (current != node)
      result.append(const_cast<NodeItem*>(current));

    // depth < 0 means unlimited traversal.
    if (depth >= 0 && currentDepth >= depth)
      continue;

    for (auto* transition : mFlow->transitions())
    {
      if (!transition)
        continue;

      NodeItem* neighbour = nullptr;

      if (transition->source() == current)
        neighbour = transition->destination();
      else if (transition->destination() == current)
        neighbour = transition->source();

      if (!neighbour)
        continue;

      if (visited.contains(neighbour))
        continue;

      if (isDescendantOf(neighbour, node))
        continue;

      if (!sameLayoutScope(node, neighbour))
        continue;

      visited.insert(neighbour);
      pending.append({neighbour, currentDepth + 1});
    }
  }

  result.append(const_cast<NodeItem*>(node));
  return result;
}

void BehaviourCanvas::onNodeGeometryChanged(NodeItem* node, const QRectF& oldRect)
{
  if (!node)
    return;

  // Calculate how far the node moved (we know that nodes only expand to the right and bottom)
  const QRectF currentRect = node->sceneAlignRect();
  const qreal oldRight = oldRect.right();
  const qreal newRight = currentRect.right();
  const qreal deltaX = newRight - oldRight;

  LOG_INFO("Geometry change: oldRight={} newRight={} delta={}", oldRight, newRight, deltaX);
  if (!qFuzzyIsNull(deltaX))
    ensureMinimumSpacing(node->id());

  for (const auto& transition : mFlow->transitions())
    if (transition->source()->id() == node->id() || transition->destination()->id() == node->id())
      transition->updatePath();
}

void BehaviourCanvas::onNodeMoved(NodeItem* node, bool done)
{
  if (!node || !mFlow)
    return;

  QList<TransitionItem*> transitions;

  for (auto* transition : mFlow->transitions())
  {
    if (!transition)
      continue;

    auto* source = transition->source();
    auto* destination = transition->destination();

    if (!source || !destination)
      continue;

    const bool sourceAffected = source == node || isDescendantOf(source, node);
    const bool destinationAffected = destination == node || isDescendantOf(destination, node);
    if (!sourceAffected && !destinationAffected)
      continue;

    transition->updatePath();
    transitions.append(transition);
  }

  Canvas::onNodeMoved(node, done);

  if (done)
    autoRoute(transitions);
}

bool BehaviourCanvas::sameLayoutScope(const NodeItem* lhs, const NodeItem* rhs) const
{
  if (!lhs || !rhs)
    return false;

  return lhs->parentNode() == rhs->parentNode() && lhs->parentSubFlow() == rhs->parentSubFlow();
}

void BehaviourCanvas::pushDownstreamNodes(NodeItem* source, qreal deltaX)
{
  if (!source || qFuzzyIsNull(deltaX))
    return;

  QSet<NodeItem*> visited;
  QList<NodeItem*> pending;

  visited.insert(source);
  pending.append(source);

  while (!pending.isEmpty())
  {
    auto* current = pending.takeFirst();

    for (auto* transition : mFlow->transitions())
    {
      if (!transition)
        continue;

      if (transition->source() != current)
        continue;

      auto* destination = transition->destination();

      if (!destination)
        continue;

      if (visited.contains(destination))
        continue;

      if (destination == source)
        continue;

      if (isDescendantOf(source, destination))
        continue;

      // Check scope BEFORE inserting it into visited.
      if (!sameLayoutScope(source, destination))
        continue;

      visited.insert(destination);
      pending.append(destination);
    }
  }

  visited.remove(source);

  for (auto* node : visited)
  {
    if (!node)
      continue;

    node->updatePosition(node->pos() + QPointF(deltaX, 0), Config::NodeMove::Relayout);
  }
}

void BehaviourCanvas::ensureMinimumSpacing(const QString& nodeId)
{
  auto node = findNodeWithId(nodeId);
  if (!node)
    return;

  const auto neighbours = getNeighboursOf(node, 1);
  const QRectF nodeRect = node->sceneAlignRect();

  for (auto* neighbour : neighbours)
  {
    if (!neighbour || neighbour == node)
      continue;

    const QRectF neighbourRect = neighbour->sceneAlignRect();

    // ----------------------------------------------------------------------
    // Neighbour is on the right.
    // ----------------------------------------------------------------------
    if (neighbourRect.center().x() >= nodeRect.center().x())
    {
      const qreal currentGap = neighbourRect.left() - nodeRect.right();

      if (currentGap >= MIN_NODE_SPACING)
        continue;

      const qreal deltaX = MIN_NODE_SPACING - currentGap;
      pushDownstreamNodes(node, deltaX);
    }
    // ----------------------------------------------------------------------
    // Neighbour is on the left.
    // ----------------------------------------------------------------------
    else
    {
      const qreal currentGap = nodeRect.left() - neighbourRect.right();

      if (currentGap >= MIN_NODE_SPACING)
        continue;

      const qreal deltaX = MIN_NODE_SPACING - currentGap;

      // For now I'd move only the left neighbour.
      //
      // If you want to preserve everything before it too, we can implement
      // pushUpstreamNodes() exactly like pushDownstreamNodes().
      neighbour->updatePosition(neighbour->pos() - QPointF(deltaX, 0), Config::NodeMove::Relayout);
    }
  }

  alignNodeGroup(neighbours, Types::AlignmentMode::VERTICAL, Types::AlignmentDirection::CENTER);
  // const auto groups = groupNodesByParent(neighbours);
  // for (auto group : groups)
  //   distributeNodeGroupHorizontally(group);
}

// =======================================================================
// Insertion stuff
NodeItem* BehaviourCanvas::insertionParentForTransition(const TransitionItem* transition) const
{
  if (!transition)
    return nullptr;

  auto* source = transition->source();
  auto* destination = transition->destination();

  if (!source || !destination)
    return nullptr;

  const auto info = transition->saveInfo();

  auto* sourceParent = source->parentNode();
  auto* destinationParent = destination->parentNode();

  const bool destinationIsSubFlowExit = info.dstPort() == Types::Port::OUT || info.dstPort() == Types::Port::ERROR || info.dstPort() == Types::Port::ABORT;
  LOG_DEBUG("Transition parent: {} {} {} {}, port types: {}, {}", source->nodeId(), destination->nodeId(), sourceParent != nullptr,
            destinationParent != nullptr, (int)info.srcPort(), (int)info.dstPort());
  // --------------------------------------------------------------------------
  // Empty subflow pass-through:
  //
  // Repeat.IN ------> Repeat.OUT
  // Repeat.IN ------> Repeat.ERROR
  // Repeat.IN ------> Repeat.ABORT
  //
  // Source and destination are the same container node, but the transition
  // connects two different boundary ports.
  if (source == destination && info.srcPort() == Types::Port::IN && destinationIsSubFlowExit)
    return source;

  // --------------------------------------------------------------------------
  // Normal transition between two nodes inside the same subflow:
  //
  // Repeat
  //   A ------> B
  //
  // The inserted node belongs to Repeat too.
  if (sourceParent && destinationParent && sourceParent == destinationParent)
    return sourceParent;

  // --------------------------------------------------------------------------
  // Subflow entry:
  //
  // Repeat.IN ------> A
  //
  // A is a child of Repeat, while Repeat itself is the source.
  if (destinationParent && source == destinationParent && info.srcPort() == Types::Port::IN)
    return destinationParent;

  // --------------------------------------------------------------------------
  // Subflow exit:
  //
  // A ------> Repeat.OUT
  // A ------> Repeat.ERROR
  // A ------> Repeat.ABORT
  //
  // A is a child of Repeat and the destination is Repeat itself.
  if (sourceParent && destination == sourceParent && destinationIsSubFlowExit)
    return sourceParent;

  // This is a transition outside a subflow, or one crossing unrelated
  // containers.
  return nullptr;
}

QPointF BehaviourCanvas::transitionPortAnchor(NodeItem* node, Types::Port port, const QPointF& fallback) const
{
  if (!node || port == Types::Port::UNKNOWN)
    return fallback;

  if (auto* item = node->getPort(port))
    return item->anchorScenePos();

  return fallback;
}

bool BehaviourCanvas::insertDroppedNodeOnTransition(TransitionItem* transition, NodeSaveInfo info)
{
  if (!transition)
    return false;

  NodeItem* source = transition->source();
  NodeItem* destination = transition->destination();

  if (!source || !destination)
    return false;

  const TransitionSaveInfo originalTransition = transition->saveInfo();

  // Determine whether this transition belongs to a subflow.
  NodeItem* insertionParent = insertionParentForTransition(transition);
  const auto srcCenter = source->mapRectToScene(source->boundingRect()).center();
  const auto dstCenter = destination->mapRectToScene(destination->boundingRect()).center();
  const QPointF insertCenter = QPointF(info.getposition().x(), (srcCenter.y() + dstCenter.y()) * 0.5);

  // The user dropped directly on the transition, so use that position.
  const QString insertedNodeId = QUuid::createUuid().toString();

  info.setId(insertedNodeId);
  info.setPosition(insertCenter);

  if (insertionParent)
  {
    info.setParentId(insertionParent->id());
    // Since every subflow has their own input port, the source port determines the
    // parent subflow
    info.setParentSubFlow(originalTransition.srcPortSubFlow());
  }
  else
  {
    info.setParentId("");
    info.setParentSubFlow(Constants::MAIN_SUB_FLOW);
  }

  const QPointF srcPoint = transitionPortAnchor(source, originalTransition.srcPort(), originalTransition.srcPoint());
  const QPointF dstPoint = transitionPortAnchor(destination, originalTransition.dstPort(), originalTransition.dstPoint());

  // ==========================================================================
  // Original source -> inserted node
  //
  // Preserve the original source port.
  //
  // This matters for:
  //   OUT   -> inserted
  //   ERROR -> inserted
  //   ABORT -> inserted
  //   Repeat.IN -> inserted
  // ==========================================================================
  TransitionSaveInfo incoming;

  incoming.setId(QUuid::createUuid().toString());

  incoming.setEvent(originalTransition.getevent());
  incoming.setLabel(originalTransition.getlabel());

  incoming.setSrcPort(originalTransition.srcPort());
  incoming.setSrcPortSubFlow(originalTransition.srcPortSubFlow());
  incoming.setDstPort(Types::Port::IN);

  incoming.setSrcId(source->id());
  incoming.setDstId(insertedNodeId);

  incoming.setSrcPoint(srcPoint);
  incoming.setDstPoint(insertCenter);

  incoming.setSrcShift({0, 0});
  incoming.setDstShift({0, 0});

  // ==========================================================================
  // Inserted node -> original destination
  //
  // Preserve the original destination port.
  //
  // This matters especially for:
  //   inserted -> Repeat.OUT
  //   inserted -> Repeat.ERROR
  //   inserted -> Repeat.ABORT
  // ==========================================================================
  TransitionConfig outConfig;

  if (const auto config = getNodeConfig(info.getnodeId()); config && !config->transitions.isEmpty())
    outConfig = config->transitions.front();

  TransitionSaveInfo outgoing;

  outgoing.setId(QUuid::createUuid().toString());

  outgoing.setEvent(outConfig.event);
  outgoing.setLabel(outConfig.label);

  outgoing.setSrcPort(Types::Port::OUT);
  outgoing.setDstPort(originalTransition.dstPort());
  outgoing.setDstPortSubFlow(originalTransition.dstPortSubFlow());

  outgoing.setSrcId(insertedNodeId);
  outgoing.setDstId(destination->id());

  outgoing.setSrcPoint(insertCenter);
  outgoing.setDstPoint(dstPoint);

  outgoing.setSrcShift({0, 0});
  outgoing.setDstShift({0, 0});

  LOG_DEBUG("Inserting node: {} ({}) on transition {} with parent {} and subflows: {} {}", info.getnodeId(), info.getid(), transition->id(), info.getparentId(),
            originalTransition.srcPortSubFlow(), originalTransition.dstPortSubFlow());
  mUndoStack->push(new InsertNodeCommand(this, info, originalTransition, incoming, outgoing));

  return true;
}

bool BehaviourCanvas::insertNodeOnTransition(TransitionItem* transition, NodeItem* node)
{
  if (!transition || !node)
    return false;

  NodeItem* source = transition->source();
  NodeItem* destination = transition->destination();

  if (!source || !destination || source == destination || node == source || node == destination)
    return false;

  const TransitionSaveInfo originalTransition = transition->saveInfo();
  NodeItem* insertionParent = insertionParentForTransition(transition);
  const QPointF originalNodeCenter = node->centerPosition();

  // The node is already being dragged over the transition, so retaining its
  // current X and placing it at the transition's vertical position works well.
  const QPointF pathCenter =
      transition->path().isEmpty() ? (originalTransition.srcPoint() + originalTransition.dstPoint()) / 2.0 : transition->path().pointAtPercent(0.5);

  const QPointF insertCenter{originalNodeCenter.x(), pathCenter.y()};
  const QPointF srcPoint = transitionPortAnchor(source, originalTransition.srcPort(), originalTransition.srcPoint());
  const QPointF dstPoint = transitionPortAnchor(destination, originalTransition.dstPort(), originalTransition.dstPoint());

  // ==========================================================================
  // Original source -> inserted node
  // ==========================================================================
  TransitionSaveInfo incoming;

  incoming.setId(QUuid::createUuid().toString());

  incoming.setEvent(originalTransition.getevent());
  incoming.setLabel(originalTransition.getlabel());

  incoming.setSrcPort(originalTransition.srcPort());
  incoming.setSrcPortSubFlow(originalTransition.srcPortSubFlow());
  incoming.setDstPort(Types::Port::IN);

  incoming.setSrcId(source->id());
  incoming.setDstId(node->id());

  incoming.setSrcPoint(srcPoint);
  incoming.setDstPoint(insertCenter);

  incoming.setSrcShift({0, 0});
  incoming.setDstShift({0, 0});

  // ==========================================================================
  // Inserted node -> original destination
  // ==========================================================================
  TransitionConfig outConfig;

  if (const auto config = getNodeConfig(node->nodeId()); config && !config->transitions.isEmpty())
    outConfig = config->transitions.front();

  TransitionSaveInfo outgoing;

  outgoing.setId(QUuid::createUuid().toString());

  outgoing.setEvent(outConfig.event);
  outgoing.setLabel(outConfig.label);

  outgoing.setSrcPort(Types::Port::OUT);
  outgoing.setDstPort(originalTransition.dstPort());
  outgoing.setDstPortSubFlow(originalTransition.dstPortSubFlow());

  outgoing.setSrcId(node->id());
  outgoing.setDstId(destination->id());

  outgoing.setSrcPoint(insertCenter);
  outgoing.setDstPoint(dstPoint);

  outgoing.setSrcShift({0, 0});
  outgoing.setDstShift({0, 0});

  const QString oldParentId = node->parentNode() ? node->parentNode()->id() : QString{};
  const QString newParentId = insertionParent ? insertionParent->id() : QString{};
  const QString oldSubflow = insertionParent ? insertionParent->parentSubFlow() : Constants::MAIN_SUB_FLOW;

  LOG_DEBUG("Inserting node: {} ({}) in transition between: {} {}", node->nodeId(), oldSubflow, source->nodeId(), destination->nodeId());
  mUndoStack->push(new InsertExistingNodeCommand(this, node->id(), originalNodeCenter, insertCenter, oldParentId, newParentId, originalTransition, incoming,
                                                 outgoing, oldSubflow));

  return true;
}
