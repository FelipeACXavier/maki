#include "batch_remove.h"

#include "logging.h"
#include "system/canvas.h"

BatchRemoveCommand::BatchRemoveCommand(Canvas* canvas, const QVector<NodeSaveInfo>& nodes, const QVector<TransitionSaveInfo>& transitions,
                                       QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNodes(nodes)
    , mTransitions(transitions)
{
  setText(QObject::tr("Batch remove"));
}

void BatchRemoveCommand::undo()
{
  if (!mCanvas)
    return;

  for (auto& node : mNodes)
    mCanvas->createNode(node);

  for (auto& transition : mTransitions)
    mCanvas->createTransition(transition);

  LOG_TRACE("Undo BatchRemoveCommand");
}

void BatchRemoveCommand::redo()
{
  if (!mCanvas)
    return;

  for (auto& transition : mTransitions)
    mCanvas->removeTransition(transition);

  for (auto& node : mNodes)
    mCanvas->removeNode(node);

  LOG_TRACE("Redo BatchRemoveCommand");
}
