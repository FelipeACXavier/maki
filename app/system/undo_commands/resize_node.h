#pragma once

#include <QSizeF>
#include <QUndoCommand>

class Canvas;
class QString;

/**
 * @brief Command to resize a node on the canvas.
 */
class ResizeNodeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs a new ResizeNodeCommand object.
   *
   * @param canvas Pointer to the canvas where the node is located.
   * @param nodeId ID of the node to be resized.
   * @param previousSize Previous size of the node.
   * @param currentSize Current size of the node.
   * @param parent Parent undo command.
   */
  ResizeNodeCommand(Canvas* canvas, const QString& nodeId, const QSizeF& previousSize, const QSizeF& currentSize, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the resize operation.
   */
  void undo() override;

  /**
   * @brief Redoes the resize operation.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr;  /// Pointer to the canvas where the node is located.
  const QString mNodeId;      /// ID of the node to be resized.
  const QSizeF mPreviousSize; /// Previous size of the node.
  const QSizeF mCurrentSize;   /// Current size of the node.
};
