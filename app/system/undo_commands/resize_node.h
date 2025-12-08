#pragma once

#include <QSizeF>
#include <QUndoCommand>

class Canvas;
class QString;

class ResizeNodeCommand : public QUndoCommand
{
public:
  ResizeNodeCommand(Canvas* canvas, const QString& nodeId, const QSizeF& previousSize, const QSizeF& currentSize, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const QString mNodeId;
  const QSizeF mPreviousSize;
  const QSizeF mCurrentSize;
};
