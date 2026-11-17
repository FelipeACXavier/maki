#include "rename_node.h"

#include "canvas/canvas.h"
#include "keys.h"

RenameNodeCommand::RenameNodeCommand(const QString& nodeId, const QString& oldName, const QString& newName, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mNodeId(nodeId)
    , mOldName(oldName)
    , mNewName(newName)
{
  setText(QObject::tr("Rename node '%1' to '%2'").arg(oldName, newName));
}

void RenameNodeCommand::undo()
{
  const auto node = SaveInfo::instance().getNodeWithId(mNodeId);
  if (!node)
    return;

  node->setProperty(ConfigKeys::NAME, maki::Value::createString(mOldName));
}

void RenameNodeCommand::redo()
{
  const auto node = SaveInfo::instance().getNodeWithId(mNodeId);
  if (!node)
    return;

  node->setProperty(ConfigKeys::NAME, maki::Value::createString(mNewName));
}
