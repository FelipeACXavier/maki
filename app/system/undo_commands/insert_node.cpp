#include "insert_node.h"

#include "canvas/canvas.h"
#include "logging.h"

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
  mCanvas->requestRemoveTransition(mIncomingTransition);
  mCanvas->requestRemoveTransition(mOutgoingTransition);

  // Now the inserted node can safely disappear.
  mCanvas->requestRemoveNode(mNode);

  // source and destination still exist, so the original edge can be restored.
  mCanvas->requestCreateTransition(mOriginalTransition);

  LOG_TRACE("Undo InsertNodeCommand");
}

void InsertNodeCommand::redo()
{
  if (!mCanvas)
    return;

  // The original edge must disappear before its replacement is created.
  mCanvas->requestRemoveTransition(mOriginalTransition);

  // Both replacement transitions depend on the inserted node existing.
  mCanvas->requestCreateNode(mNode, Canvas::NodeCreation::Inserting);

  mCanvas->requestCreateTransition(mIncomingTransition);
  mCanvas->requestCreateTransition(mOutgoingTransition);
  SaveInfo::instance().publish(DocumentEvent{
    .type = DocumentEvent::Type::BatchCreationDone,
    .data =
      BatchEvent{
        .canvasId = mCanvas->id(),
        .createdNodes = {mNode.getid()},
        .createdTransitions = {mIncomingTransition.getid(), mOutgoingTransition.getid()},
      },
  });
  LOG_TRACE("Redo InsertNodeCommand");
}
