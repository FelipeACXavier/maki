#include "flow_from_selection.h"

#include <QTabWidget>

#include "../behaviour_canvas.h"
#include "../canvas.h"
#include "../canvas_view.h"
#include "../config_table.h"
#include "keys.h"

FlowFromSelectionCommand::FlowFromSelectionCommand(Canvas* canvas, const QString& flowId, const QString& flowOwner, const FlowSaveInfo& flowInfo,
                                                   const NodeSaveInfo& startNode, const NodeSaveInfo& endNode, const TransitionSaveInfo& startTransition,
                                                   const TransitionSaveInfo& endTransition, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mFlowId(flowId)
    , mFlowOwner(flowOwner)
    , mFlowInfo(flowInfo)
    , mStartNode(startNode)
    , mEndNode(endNode)
    , mStartTransition(startTransition)
    , mEndTransition(endTransition)
{
}

void FlowFromSelectionCommand::undo()
{
  LOG_TRACE("Undo BatchRemoveCommand");
}

void FlowFromSelectionCommand::redo()
{
  LOG_DEBUG("STARTING FLOW FROM SELECTION COMMAND");
  if (!mCanvas)
  {
    LOG_WARNING("Could not convert selection to flow: canvas is null");
    return;
  }

  if (mFlowOwner.isEmpty())
  {
    LOG_WARNING("Could not find task node");
    return;
  }

  auto* newCanvas = qobject_cast<BehaviourCanvas*>(mCanvas->requestNewFlow(mFlowOwner));
  if (!newCanvas)
  {
    LOG_WARNING("Could not create a new canvas");
    return;
  }

  PFlow newFlow = newCanvas->flow();
  if (!newFlow)
  {
    LOG_WARNING("Could not create new flow");
    return;
  }

  // --------------------------------------------------------------------------
  // Build the new flow contents.
  auto updateInfo = mFlowInfo;
  for (const auto& node : newFlow->getNodes())
  {
    if (!node)
      continue;

    if (node->getnodeId() == ConfigKeys::START_NODE)
    {
      updateInfo.addNode(node);

      auto transition = std::make_shared<TransitionSaveInfo>();
      transition->setId(QUuid::createUuid().toString());
      transition->setSrcId(node->getid());
      transition->setDstId(mStartTransition.getdstId());
      transition->setSrcPort(Types::Port::OUT);
      transition->setDstPort(Types::Port::IN);
      transition->setSrcShift({0, 0});
      transition->setDstShift({0, 0});

      updateInfo.addTransition(transition);
    }
    else if (node->getnodeId() == ConfigKeys::SUCCESS_NODE)
    {
      updateInfo.addNode(node);

      auto transition = std::make_shared<TransitionSaveInfo>();
      transition->setId(QUuid::createUuid().toString());
      transition->setSrcId(mEndTransition.getsrcId());
      transition->setDstId(node->getid());
      transition->setSrcPort(Types::Port::OUT);
      transition->setDstPort(Types::Port::IN);
      transition->setSrcShift({0, 0});
      transition->setDstShift({0, 0});

      updateInfo.addTransition(transition);
    }
  }

  // Remove any existing transitions
  auto toDelete = newFlow->transitions();
  for (const auto* transition : toDelete)
    newCanvas->removeTransition(transition->saveInfo());

  // --------------------------------------------------------------------------
  // Remove the original boundary transitions first.
  mCanvas->removeTransition(mStartTransition);
  mCanvas->removeTransition(mEndTransition);

  // --------------------------------------------------------------------------
  // Remove internal transitions from the old flow.
  for (const auto& transition : mFlowInfo.gettransitions())
  {
    auto transitionInfo = std::dynamic_pointer_cast<TransitionSaveInfo>(transition);
    if (!transitionInfo)
      continue;

    mCanvas->removeTransition(*transitionInfo);
  }

  // --------------------------------------------------------------------------
  // Remove the selected nodes from the old flow.
  QPointF oldCenter{0, 0};
  for (const auto& node : mFlowInfo.getnodes())
  {
    auto nodeInfo = std::dynamic_pointer_cast<NodeSaveInfo>(node);
    if (!nodeInfo)
      continue;

    oldCenter += node->getposition();
    mCanvas->removeNode(*nodeInfo);
  }
  oldCenter /= mFlowInfo.getnodes().size();

  // --------------------------------------------------------------------------
  // Create the FlowCall replacing the selection.
  const auto configuration = ConfigurationTable::instance().get(ConfigKeys::FLOW_CALL_NODE);
  if (!configuration)
  {
    LOG_WARNING("Could not find FlowCall configuration");
    return;
  }

  NodeSaveInfo flowCall(*configuration);
  flowCall.setId(QUuid::createUuid().toString());
  flowCall.setPosition(oldCenter);
  mCanvas->createNode(flowCall, Canvas::NodeCreation::Inserting);
  auto newNode = mCanvas->findNodeWithId(flowCall.getid());
  if (!newNode)
    return;

  maki::RecordValue record;
  maki::setRecordValue(record, ConfigKeys::COMPONENT, maki::Value::createString(mFlowOwner));
  maki::setRecordValue(record, ConfigKeys::ARGUMENTS, maki::Value::createList({}));
  maki::setRecordValue(record, ConfigKeys::FLOW, maki::Value::createString(newFlow->name()));
  maki::setRecordValue(record, ConfigKeys::COMPONENT_ID, maki::Value::createString(mFlowOwner));
  maki::setRecordValue(record, ConfigKeys::COMPONENT_TYPE, maki::Value::createString(ConfigKeys::TASK_NODE));
  maki::setRecordValue(record, ConfigKeys::CALL_ID, maki::Value::createString(newFlow->id()));
  newNode->setProperty(ConfigKeys::TASK, maki::Value::createRecord(record));

  // --------------------------------------------------------------------------
  // Rewire original boundary:
  {
    TransitionSaveInfo transition = mStartTransition;
    transition.setId(QUuid::createUuid().toString());
    transition.setDstId(flowCall.getid());
    transition.setDstPort(Types::Port::IN);
    transition.setDstShift({0, 0});
    mCanvas->createTransition(transition);
  }
  {
    TransitionSaveInfo transition = mEndTransition;
    transition.setId(QUuid::createUuid().toString());
    transition.setSrcId(flowCall.getid());
    transition.setSrcPort(Types::Port::OUT);
    transition.setSrcShift({0, 0});
    mCanvas->createTransition(transition);
  }

  // Populate the newly created flow
  for (const auto& inode : updateInfo.getnodes())
    if (auto nodeInfo = std::dynamic_pointer_cast<NodeSaveInfo>(inode); nodeInfo)
      newCanvas->createNode(*nodeInfo, Canvas::NodeCreation::Inserting);

  for (const auto& itransition : updateInfo.gettransitions())
    if (auto transitionInfo = std::dynamic_pointer_cast<TransitionSaveInfo>(itransition); transitionInfo)
      newCanvas->createTransition(*transitionInfo);

  LOG_TRACE("Redo FlowFromSelectionCommand");
}
