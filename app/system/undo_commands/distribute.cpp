#include "distribute.h"

#include "logging.h"
#include "system/canvas.h"

DistributeCommand::DistributeCommand(Canvas* canvas, const QList<Types::AlignmentNode>& items, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mItems(items)
{
  setText(QObject::tr("Distribute nodes"));
}

void DistributeCommand::undo()
{
  if (!mCanvas)
    return;

  for (const auto& item : mItems)
    mCanvas->moveNodeTo(item.id, item.pos);

  LOG_TRACE("Undo AlignCommand");
}

void DistributeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->distributeNodes(mItems);
  LOG_TRACE("Redo AlignCommand");
}
