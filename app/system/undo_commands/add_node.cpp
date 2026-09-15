#include "add_node.h"

#include "logging.h"
#include "system/canvas.h"

AddNodeCommand::AddNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, Canvas::NodeCreation creation, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mDesc(desc)
    , mCreation(creation)
{
  setText(QObject::tr("Add node"));
}

void AddNodeCommand::undo()
{
  if (!mCanvas)
    return;

  mCanvas->removeNode(mDesc);
  LOG_TRACE("Undo AddNodeCommand: {} {}", mDesc.getnodeId(), mDesc.getid());
}

void AddNodeCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->createNode(mDesc, mCreation);
  LOG_TRACE("Redo AddNodeCommand: {} {}", mDesc.getnodeId(), mDesc.getid());
}
