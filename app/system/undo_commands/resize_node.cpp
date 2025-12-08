#include "resize_node.h"

#include <QString>

#include "logging.h"
#include "system/canvas.h"

ResizeNodeCommand::ResizeNodeCommand(Canvas* canvas, const QString& nodeId, const QSizeF& previousSize, const QSizeF& currentSize, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNodeId(nodeId)
    , mPreviousSize(previousSize)
    , mCurrentSize(currentSize)
{
  setText(QObject::tr("Move node"));
}

void ResizeNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->setNodeSize(mNodeId, mPreviousSize);
  LOG_DEBUG("Undo ResizeNodeCommand: %.2f %.2f", mCurrentSize.width(), mCurrentSize.height());
}

void ResizeNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->setNodeSize(mNodeId, mCurrentSize);
  LOG_DEBUG("Redo ResizeNodeCommand: %.2f %.2f", mCurrentSize.width(), mCurrentSize.height());
}
