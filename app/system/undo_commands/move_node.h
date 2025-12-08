#pragma once

#include <QPointF>
#include <QUndoCommand>

class Canvas;
class QString;

class MoveNodeCommand : public QUndoCommand
{
public:
  MoveNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& previousPosition, const QPointF& currentPosition, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const QString mNodeId;
  const QPointF mPreviousPosition;
  const QPointF mCurrentPosition;
};
