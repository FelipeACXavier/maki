#pragma once

#include <QUndoCommand>

#include "save_info.h"
#include "types.h"

class Canvas;
class QPointF;

class AlignCommand : public QUndoCommand
{
public:
  AlignCommand(Canvas* canvas, const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const QList<Types::AlignmentNode> mItems;
  const Types::AlignmentMode mMode;
  const Types::AlignmentDirection mDirection;
};
