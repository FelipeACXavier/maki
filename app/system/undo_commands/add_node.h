#pragma once

#include <QUndoCommand>

#include "../canvas.h"
#include "save_info.h"

class QPointF;  /// Forward declaration of QPointF class.

/**
 * @brief Command to add a node to the canvas.
 */
class AddNodeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs an AddNodeCommand object.
   *
   * @param canvas Pointer to the Canvas where the node will be added.
   * @param desc Description of the node to be added.
   * @param parent Parent command for undo/redo functionality.
   */
  AddNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, Canvas::NodeCreation creation, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the addition of a node from the canvas.
   */
  void undo() override;

  /**
   * @brief Redoes the addition of a node to the canvas.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr;  /// Pointer to the Canvas where the node will be added.
  const NodeSaveInfo mDesc;   /// Description of the node to be added.
  const Canvas::NodeCreation mCreation;
};
