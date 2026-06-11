#pragma once

#include <QUndoCommand>

#include "save_info.h"

class Canvas; /// Forward declaration of Canvas class.
class QPointF; /// Forward declaration of QPointF class.

/**
 * @brief Command to remove a transition in the canvas.
 */
class RemoveTransitionCommand : public QUndoCommand
{
public:
  /**
   * @brief Constructs a new RemoveTransitionCommand object.
   *
   * @param canvas Pointer to the canvas where the transition is located.
   * @param desc Transition save information.
   * @param parent Parent undo command.
   */
  RemoveTransitionCommand(Canvas* canvas, const TransitionSaveInfo& desc, QUndoCommand* parent = nullptr);

  /**
   * @brief Undoes the removal of the transition.
   */
  void undo() override;

  /**
   * @brief Redoes the removal of the transition.
   */
  void redo() override;

private:
  Canvas* mCanvas = nullptr; /// Pointer to the canvas where the transition is located.
  const TransitionSaveInfo mDesc; /// Transition save information.
};
