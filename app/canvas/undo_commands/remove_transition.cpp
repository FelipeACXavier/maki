#include "remove_transition.h"

#include "canvas.h"
#include "logging.h"

RemoveTransitionCommand::RemoveTransitionCommand(Canvas* canvas, const TransitionSaveInfo& desc, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
{
  setText(QObject::tr("Remove transition"));
}

void RemoveTransitionCommand::undo()
{
  if (!mCanvas)
    return;

  // mCanvas->createNode(mDesc);
  LOG_DEBUG("Undo RemoveTransitionCommand: %s", qPrintable(mDesc.getlabel()));
}

void RemoveTransitionCommand::redo()
{
  if (!mCanvas)
    return;

  // mCanvas->onRemoveNode(mDesc.id);
  LOG_DEBUG("Redo RemoveTransitionCommand: %s", qPrintable(mDesc.getlabel()));
}
