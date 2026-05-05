#pragma once

#include <QPointF>
#include <QUndoCommand>

class Canvas;
class QString;

/**
 * @brief Command to move a node on the canvas.
 */
class MoveNodeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs a MoveNodeCommand object.
   *
   * @param canvas Pointer to the canvas where the node is located.
   * @param nodeId ID of the node to be moved.
   * @param previousPosition The previous position of the node.
   * @param currentPosition The new position of the node.
   * @param parent Pointer to the parent command, if any.
   */
  MoveNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& previousPosition, const QPointF& currentPosition, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the move operation.
   */
  void undo() override;

  /**
   * @brief Redoes the move operation.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr;  /// Pointer to the canvas where the node is located.
  const QString mNodeId;      /// ID of the node to be moved.
  const QPointF mPreviousPosition;  /// The previous position of the node.
  const QPointF mCurrentPosition;   /// The new position of the node.
};
