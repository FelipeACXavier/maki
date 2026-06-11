#include "add_transition.h"

#include "canvas.h"
#include "logging.h"

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

  // mCanvas->onRemoveNode(mDesc.id);
  LOG_DEBUG("Undo AddTransitionCommand: %s", qPrintable(mDesc.getlabel()));
}

void AddTransitionCommand::redo()
{
  if (!mCanvas)
    return;

  // mCanvas->createNode(mDesc);
  LOG_DEBUG("Redo AddTransitionCommand: %s", qPrintable(mDesc.getlabel()));
}
