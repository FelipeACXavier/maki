#include "add_node.h"

#include "logging.h"
#include "system/canvas.h"

AddNodeCommand::AddNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
{
  setText(QObject::tr("Add node"));
}

void AddNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->removeNode(mDesc);
  LOG_TRACE("Undo AddNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}

void AddNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->createNode(mDesc);
  LOG_TRACE("Redo AddNodeCommand: %s %s", qPrintable(mDesc.nodeId), qPrintable(mDesc.id));
}
