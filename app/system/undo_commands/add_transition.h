#pragma once

#include <QUndoCommand>

#include "save_info.h"

class Canvas; /// Forward declaration of Canvas class.
class QPointF; /// Forward declaration of QPointF class.

/**
 * @brief Command to add a transition in the canvas.
 */
class AddTransitionCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs an AddTransitionCommand object.
   *
   * @param canvas Pointer to the canvas where the transition will be added.
   * @param desc Description of the transition to be added.
   * @param parent Parent command for undo/redo functionality.
   */
  AddTransitionCommand(Canvas* canvas, const TransitionSaveInfo& desc, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the addition of the transition.
   */
  void undo() override;

  /**
   * @brief Redoes the addition of the transition.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr; /// Pointer to the canvas where the transition is added.
  const TransitionSaveInfo mDesc; /// Description of the transition to be added.
};
