#pragma once

#include <QUndoCommand>

#include "elements/save_info.h"

class Canvas;
class QPointF;

class RemoveTransitionCommand : public QUndoCommand
{
public:
  RemoveTransitionCommand(Canvas* canvas, const TransitionSaveInfo& desc, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const TransitionSaveInfo mDesc;
};
