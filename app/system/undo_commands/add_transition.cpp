#include "add_transition.h"

#include "logging.h"
#include "system/canvas.h"

AddTransitionCommand::AddTransitionCommand(Canvas* canvas, const TransitionSaveInfo& desc, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
{
  setText(QObject::tr("Add transition"));
}

void AddTransitionCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->removeTransition(mDesc);
  LOG_TRACE("Undo AddTransitionCommand: {}", mDesc.getlabel());
}

void AddTransitionCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->createTransition(mDesc);
  LOG_TRACE("Redo AddTransitionCommand: {}", mDesc.getlabel());
}
