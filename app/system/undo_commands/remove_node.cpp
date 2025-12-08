#include "remove_node.h"

#include "logging.h"
#include "system/canvas.h"

RemoveNodeCommand::RemoveNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
{
  setText(QObject::tr("Remove node %1").arg(desc.nodeId));
}

void RemoveNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->createNode(mDesc);
  LOG_DEBUG("Undo RemoveNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}

void RemoveNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->onRemoveNode(mDesc.id);
  LOG_DEBUG("Redo RemoveNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}
