#include "remove_node.h"

#include "logging.h"
#include "system/canvas.h"

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
  LOG_TRACE("Undo RemoveNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}

void RemoveNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->removeNode(mDesc, false);
  LOG_TRACE("Redo RemoveNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}
