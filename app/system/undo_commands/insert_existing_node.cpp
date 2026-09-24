#include "insert_existing_node.h"

#include "../canvas.h"
#include "elements/node.h"

InsertExistingNodeCommand::InsertExistingNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& oldCenter, const QPointF& newCenter,
                                                     const QString& oldParent, const QString& newParent, const TransitionSaveInfo& originalTransition,
                                                     const TransitionSaveInfo& incomingTransition, const TransitionSaveInfo& outgoingTransition,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNodeId(nodeId)
    , mOldParentId(oldParent)
    , mNewParentId(newParent)
    , mOldCenter(oldCenter)
    , mNewCenter(newCenter)
    , mOriginalTransition(originalTransition)
    , mIncomingTransition(incomingTransition)
    , mOutgoingTransition(outgoingTransition)
{
  setText(QObject::tr("Insert node on transition"));
}

void InsertExistingNodeCommand::redo()
{
  if (!mCanvas)
    return;

  auto* node = mCanvas->findNodeWithId(mNodeId);
  if (!node)
    return;

  mCanvas->removeTransition(mOriginalTransition);

  node->setCenterPosition(mNewCenter);

  mCanvas->createTransition(mIncomingTransition);
  mCanvas->createTransition(mOutgoingTransition);
  mCanvas->reparentNode(mNodeId, mOldParentId);
}

void InsertExistingNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->reparentNode(mNodeId, mNewParentId);
  mCanvas->removeTransition(mIncomingTransition);
  mCanvas->removeTransition(mOutgoingTransition);
  mCanvas->createTransition(mOriginalTransition);

  auto* node = mCanvas->findNodeWithId(mNodeId);
  if (node)
    node->setCenterPosition(mOldCenter);
}
