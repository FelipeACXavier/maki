#pragma once

#include <QUndoCommand>

#include "elements/save_info.h"

class Canvas;
class QPointF;

class RemoveNodeCommand : public QUndoCommand
{
public:
  RemoveNodeCommand(Canvas* canvas, const NodeSaveInfo& desc, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const NodeSaveInfo mDesc;
};
