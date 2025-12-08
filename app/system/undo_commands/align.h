#pragma once

#include <QUndoCommand>

#include "elements/save_info.h"

class Canvas;
class QPointF;

class AlignCommand : public QUndoCommand
{
public:
  AlignCommand(Canvas* canvas, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
};
