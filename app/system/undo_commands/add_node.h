#pragma once

#include <QUndoCommand>

#include "save_info.h"

class Canvas;
class QPointF;

class AddNodeCommand : public QUndoCommand
{
public:
  AddNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const NodeSaveInfo mDesc;
};
