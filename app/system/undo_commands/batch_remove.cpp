#include "batch_remove.h"

#include "canvas/canvas.h"
#include "logging.h"

BatchRemoveCommand::BatchRemoveCommand(Canvas* canvas, const QVector<NodeSaveInfo>& nodes, const QVector<TransitionSaveInfo>& transitions, QUndoCommand* parent)
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

  for (const auto& node : mNodes)
    mCanvas->requestCreateNode(node, Canvas::NodeCreation::Populating);

  for (const auto& transition : mTransitions)
    mCanvas->requestCreateTransition(transition);

  LOG_TRACE("Undo BatchRemoveCommand");
}

void BatchRemoveCommand::redo()
{
  if (!mCanvas)
    return;

  for (const auto& transition : mTransitions)
    mCanvas->requestRemoveTransition(transition);

  for (const auto& node : mNodes)
    mCanvas->requestRemoveNode(node);

  LOG_TRACE("Redo BatchRemoveCommand");
}
