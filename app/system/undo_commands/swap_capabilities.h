#pragma once

#include <QString>
#include <QUndoCommand>

class Canvas;

class SwapCapabilitiesCommand : public QUndoCommand
{
public:
  SwapCapabilitiesCommand(Canvas* canvas, const QString& parentId, const QString& aId, const QString& bId,
                        QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas;
  QString mParentId;
  QString mAId;
  QString mBId;
};
