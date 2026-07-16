#include "behaviour_canvas.h"

#include <qcoreapplication.h>
#include <qhashfunctions.h>

#include "canvas_view.h"
#include "config_table.h"
#include "elements/behaviour/flow_call_node.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "elements/port.h"
#include "elements/transition.h"
#include "logging.h"
#include "style_helpers.h"
#include "transition_info.h"
#include "types.h"
#include "widgets/structure/flow_call_menu.h"
#include "widgets/structure/transition_event_menu.h"

BehaviourCanvas::BehaviourCanvas(Flow* flow, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(flow->id(), configTable, router, parent)
    , mFlow(flow)
    , mStorage(storage)
{
  connect(this, &Canvas::nodeRemoved, this, [this](const QString& nodeId, const QString&) {
    if (mFlowCallMenu && mFlowCallMenu->trackedNodeId() == nodeId)
      mFlowCallMenu->hideMenu();
  });
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

bool BehaviourCanvas::canAddTransition(NodeItem* node, PortItem* port) const
{
  if (!node)
    return false;

  if (port && port->nodeItem() == node)
  {
    if (port->kind() == PortItem::Abort)
    {
      for (const auto& t : mFlow->transitions())
      {
        if (t->source()->id() == node->id() && t->getEvent() == QStringLiteral("on abort"))
          return false;
      }
      return true;
    }

    if (port->kind() == PortItem::Error)
    {
      for (const auto& t : mFlow->transitions())
      {
        if (t->source()->id() == node->id() && t->getEvent() == QStringLiteral("on error"))
          return false;
      }
      return true;
    }
  }

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

  if (mTransitionMenu && mTransitionMenu->isVisible())
  {
    for (const auto& transition : mFlow->transitions())
    {
      if (transition->id() == mTransitionMenu->trackedTransitionId())
      {
        if (transition->source()->id() == nodeId || transition->destination()->id() == nodeId)
          mTransitionMenu->updatePosition(parentView());
        break;
      }
    }
  }

  if (mFlowCallMenu && mFlowCallMenu->isVisible() && mFlowCallMenu->trackedNodeId() == nodeId)
    mFlowCallMenu->updatePosition(parentView());
}

void BehaviourCanvas::ensureTransitionMenu()
{
  if (mTransitionMenu)
    return;

  CanvasView* view = parentView();
  if (!view)
    return;

  mTransitionMenu = new TransitionEventMenu(view);
}

void BehaviourCanvas::ensureFlowCallMenu()
{
  if (mFlowCallMenu)
    return;

  CanvasView* view = parentView();
  if (!view)
    return;

  mFlowCallMenu = new FlowCallMenu(view);
  connect(mFlowCallMenu, &FlowCallMenu::createFlowRequested, this, &Canvas::createFlowRequested);
}

void BehaviourCanvas::navigateToFlowCallTarget(NodeItem* flowCallNode)
{
  QString taskId;
  QString flowName;
  if (!FlowCallMenu::resolveFlowCallTarget(flowCallNode, mStorage.get(), taskId, flowName))
    return;

  emit openFlowCallTarget(taskId, flowName);
}

void BehaviourCanvas::onSelectionChanged()
{
  Canvas::onSelectionChanged();

  ensureTransitionMenu();
  ensureFlowCallMenu();

  TransitionItem* selectedTransition = nullptr;
  NodeItem* selectedFlowCallNode = nullptr;

  for (auto* item : selectedItems())
  {
    if (item->type() == Types::TRANSITION)
    {
      if (selectedTransition || selectedFlowCallNode)
      {
        selectedTransition = nullptr;
        selectedFlowCallNode = nullptr;
        break;
      }

      selectedTransition = static_cast<TransitionItem*>(item);
      continue;
    }

    if (item->type() == NodeItem::Type)
    {
      auto* node = static_cast<NodeItem*>(item);
      if (!dynamic_cast<FlowCallNode*>(node))
        continue;

      if (selectedTransition || selectedFlowCallNode)
      {
        selectedTransition = nullptr;
        selectedFlowCallNode = nullptr;
        break;
      }

      selectedFlowCallNode = node;
    }
  }

  if (selectedTransition && mTransitionMenu)
  {
    mTransitionMenu->showForTransition(selectedTransition, parentView(), mStorage.get());
    if (mFlowCallMenu)
      mFlowCallMenu->hideMenu();
  }
  else if (selectedFlowCallNode && mFlowCallMenu)
  {
    mFlowCallMenu->showForNode(selectedFlowCallNode, parentView(), mStorage.get());
    if (mTransitionMenu)
      mTransitionMenu->hideMenu();
  }
  else
  {
    if (mTransitionMenu)
      mTransitionMenu->hideMenu();
    if (mFlowCallMenu)
      mFlowCallMenu->hideMenu();
  }
}

namespace
{
TransitionItem* connectBehaviourNodes(Canvas* canvas, BehaviourCanvas* behaviourCanvas, NodeItem* source, NodeItem* destination,
                                      const QString& event, const QString& label)
{
  if (!canvas || !behaviourCanvas || !source || !destination)
    return nullptr;

  if (!behaviourCanvas->canAddTransition(source))
    return nullptr;

  auto storage = std::make_shared<TransitionSaveInfo>();
  auto* transition = new TransitionItem(storage);
  transition->setEvent(event);
  transition->setName(label);
  transition->setStart(source->id(), source->edgePointToward(destination->sceneBoundingRect().center(), true), {0, 0});
  transition->setEnd(destination->id(), destination->edgePointToward(source->sceneBoundingRect().center(), false), {0, 0});
  transition->done(source, destination);
  canvas->addItem(transition);
  behaviourCanvas->addTransition(transition);
  return transition;
}
}  // namespace

NodeItem* BehaviourCanvas::insertDroppedNodeOnTransition(TransitionItem* transition, std::shared_ptr<NodeSaveInfo> info)
{
  if (!transition || !info)
    return nullptr;

  NodeItem* source = transition->source();
  NodeItem* destination = transition->destination();
  if (!source || !destination || source == destination)
    return nullptr;

  const QString eventName = transition->getEvent();
  const QString label = transition->getName();

  removeTransition(transition);
  removeItem(transition);
  delete transition;

  const QPointF insertCenter =
      snapToGrid((source->sceneBoundingRect().center() + destination->sceneBoundingRect().center()) * 0.5, Config::GRID_SIZE);

  NodeItem* node = createNode(NodeCreation::Dropping, info, insertCenter, nullptr);
  if (!node)
    return nullptr;

  if (!connectBehaviourNodes(this, this, source, node, eventName, label))
    return nullptr;

  const TransitionConfig outConfig = nextTransition(node);
  if (!connectBehaviourNodes(this, this, node, destination, outConfig.event, outConfig.label))
    return nullptr;

  onNodeMoved(node->id());
  return node;
}

void BehaviourCanvas::onCleanChanged(bool state)
{
  if (mFlow)
    emit cleanChanged(id(), mFlow->name(), state);
}
