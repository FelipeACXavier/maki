#pragma once

#include <QUndoCommand>

#include "types.h"

class Canvas;   /// Pointer to the canvas on which the command operates.
class QPointF;  /// Position type used for alignment calculations.

/**
 * @brief Command class for aligning items on a canvas.
 */
class DistributeCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs an AlignCommand object.
   *
   * @param canvas Pointer to the canvas where the alignment will occur.
   * @param items List of alignment nodes to be aligned.
   * @param parent Parent undo command for grouping.
   */
  DistributeCommand(Canvas* canvas, const QList<Types::AlignmentNode>& items, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the distribution operation.
   */
  void undo() override;

  /**
   * @brief Redoes the distribution operation.
   */
  void redo() override;

private:
  Canvas* mCanvas;                           /// Pointer to the canvas on which the command operates. /// @note Not owned by this class.
  const QList<Types::AlignmentNode> mItems;  /// List of alignment nodes to be aligned. /// @note Not owned by this class.
};
