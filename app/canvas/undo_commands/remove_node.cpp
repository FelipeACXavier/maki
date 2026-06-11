#include "remove_node.h"

#include "canvas.h"
#include "logging.h"

RemoveNodeCommand::RemoveNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
{
  setText(QObject::tr("Remove node"));
}

void RemoveNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->createNode(mDesc);
  LOG_TRACE("Undo RemoveNodeCommand: %s %s", qPrintable(mDesc.getnodeId()), qPrintable(mDesc.getid()));
}

void RemoveNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->removeNode(mDesc);
  LOG_TRACE("Redo RemoveNodeCommand: %s %s", qPrintable(mDesc.getnodeId()), qPrintable(mDesc.getid()));
}
