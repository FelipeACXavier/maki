#include "insert_node.h"

#include "logging.h"
#include "system/canvas.h"

InsertNodeCommand::InsertNodeCommand(Canvas* canvas, const NodeSaveInfo& node, const TransitionSaveInfo& originalTransition,
                                     const TransitionSaveInfo& incomingTransition, const TransitionSaveInfo& outgoingTransition, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNode(node)
    , mOriginalTransition(originalTransition)
    , mIncomingTransition(incomingTransition)
    , mOutgoingTransition(outgoingTransition)
{
  setText(QObject::tr("Insert node"));
}

void InsertNodeCommand::undo()
{
  if (!mCanvas)
    return;

  // Remove transitions while all their endpoints still exist.
  mCanvas->removeTransition(mIncomingTransition);
  mCanvas->removeTransition(mOutgoingTransition);

  // Now the inserted node can safely disappear.
  mCanvas->removeNode(mNode);

  // source and destination still exist, so the original edge can be restored.
  mCanvas->createTransition(mOriginalTransition);

  LOG_TRACE("Undo InsertNodeCommand");
}

void InsertNodeCommand::redo()
{
  if (!mCanvas)
    return;

  // The original edge must disappear before its replacement is created.
  mCanvas->removeTransition(mOriginalTransition);

  // Both replacement transitions depend on the inserted node existing.
  mCanvas->createNode(mNode);

  mCanvas->createTransition(mIncomingTransition);
  mCanvas->createTransition(mOutgoingTransition);

  LOG_TRACE("Redo InsertNodeCommand");
}