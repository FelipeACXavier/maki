#pragma once

#include <QUndoCommand>

#include "save_info.h"
#include "types.h"

class Canvas; /// Pointer to the canvas on which the command operates.
class QPointF; /// Position type used for alignment calculations.

/**
 * @brief Command class for aligning items on a canvas.
 */
class AlignCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs an AlignCommand object.
   *
   * @param canvas Pointer to the canvas where the alignment will occur.
   * @param items List of alignment nodes to be aligned.
   * @param mode Alignment mode (e.g., horizontal, vertical).
   * @param direction Direction of alignment (e.g., left, right).
   * @param parent Parent undo command for grouping.
   */
  AlignCommand(Canvas* canvas, const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the alignment operation.
   */
  void undo() override;

  /**
   * @brief Redoes the alignment operation.
   */
  void redo() override;

private:
  Canvas* mCanvas; /// Pointer to the canvas on which the command operates. /// @note Not owned by this class.
  const QList<Types::AlignmentNode> mItems; /// List of alignment nodes to be aligned. /// @note Not owned by this class.
  const Types::AlignmentMode mMode; /// Alignment mode (e.g., horizontal, vertical).
  const Types::AlignmentDirection mDirection; /// Direction of alignment (e.g., left, right).
};
