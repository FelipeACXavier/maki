#include "align.h"

#include "logging.h"
#include "system/canvas.h"

AlignCommand::AlignCommand(Canvas* canvas, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
{
  setText(QObject::tr("Align"));
}

void AlignCommand::undo()
{
  if (!mCanvas)
    return;

  LOG_DEBUG("Undo AlignCommand");
}

void AlignCommand::redo()
{
  if (!mCanvas)
    return;

  LOG_DEBUG("Redo AlignCommand");
}
