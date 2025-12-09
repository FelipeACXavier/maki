#include "move_node.h"

#include <QString>

#include "logging.h"
#include "system/canvas.h"

MoveNodeCommand::MoveNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& previousPosition, const QPointF& currentPosition, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNodeId(nodeId)
    , mPreviousPosition(previousPosition)
    , mCurrentPosition(currentPosition)
{
  setText(QObject::tr("Move node"));
}

void MoveNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->moveNodeTo(mNodeId, mPreviousPosition);
  LOG_TRACE("Undo MoveNodeCommand: %d %d", mCurrentPosition.x(), mCurrentPosition.y());
}

void MoveNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->moveNodeTo(mNodeId, mCurrentPosition);
  LOG_TRACE("Redo MoveNodeCommand: %d %d", mCurrentPosition.x(), mCurrentPosition.y());
}
