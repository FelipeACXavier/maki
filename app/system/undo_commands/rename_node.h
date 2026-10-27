#pragma once

#include <QString>
#include <QUndoCommand>

#include "../canvas.h"

/**
 * @brief Command to rename a node on the canvas.
 */
class RenameNodeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs a RenameNodeCommand object.
   *
   * @param canvas Pointer to the canvas containing the node.
   * @param nodeId Unique identifier of the node.
   * @param oldName Original name of the node.
   * @param newName New name of the node.
   * @param parent Parent undo command.
   */
  RenameNodeCommand(Canvas* canvas, const QString& nodeId, const QString& oldName, const QString& newName, QUndoCommand* parent = nullptr);

  /**
   * @brief Restores the original node name.
   */
  void undo() override;

  /**
   * @brief Applies the new node name.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const QString mNodeId;
  const QString mOldName;
  const QString mNewName;
};
