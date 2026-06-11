#include "align.h"

#include "canvas.h"
#include "logging.h"

AlignCommand::AlignCommand(Canvas* canvas, const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mItems(items)
    , mMode(mode)
    , mDirection(direction)
{
  setText(QObject::tr("Align"));
}

void AlignCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->alignNodes(mItems, mMode, mDirection, true);
  LOG_TRACE("Undo AlignCommand");
}

void AlignCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->alignNodes(mItems, mMode, mDirection, false);
  LOG_TRACE("Redo AlignCommand");
}
