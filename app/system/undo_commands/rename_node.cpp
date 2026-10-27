#include "rename_node.h"

#include "config.h"
#include "elements/node.h"
#include "keys.h"

RenameNodeCommand::RenameNodeCommand(Canvas* canvas, const QString& nodeId, const QString& oldName, const QString& newName, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mNodeId(nodeId)
    , mOldName(oldName)
    , mNewName(newName)
{
  setText(QObject::tr("Rename node '%1' to '%2'").arg(oldName, newName));
}

void RenameNodeCommand::undo()
{
  if (!mCanvas)
    return;

  auto* node = mCanvas->findNodeWithId(mNodeId);
  if (!node)
    return;

  node->setProperty(ConfigKeys::NAME, maki::Value::createString(mOldName));
}

void RenameNodeCommand::redo()
{
  if (!mCanvas)
    return;

  auto* node = mCanvas->findNodeWithId(mNodeId);
  if (!node)
    return;

  node->setProperty(ConfigKeys::NAME, maki::Value::createString(mNewName));
}
