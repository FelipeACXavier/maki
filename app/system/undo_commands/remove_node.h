#pragma once

#include <QUndoCommand>

#include "save_info.h"

class Canvas; /// Forward declaration of Canvas class.
class QPointF; /// Forward declaration of QPointF class.

/**
 * @brief Command to remove a node from the canvas.
 */
class RemoveNodeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs a RemoveNodeCommand object.
   *
   * @param canvas Pointer to the canvas where the node is located.
   * @param desc Description of the node to be removed.
   * @param parent Parent command for undo/redo functionality.
   */
  RemoveNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the removal of a node from the canvas.
   */
  void undo() override;

  /**
   * @brief Redoes the removal of a node from the canvas.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr; /// Pointer to the canvas where the node is located.
  const NodeSaveInfo mDesc; /// Description of the node to be removed.
};
