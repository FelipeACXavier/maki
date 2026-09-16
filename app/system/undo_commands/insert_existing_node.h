#pragma once

#include <QUndoCommand>

#include "save_info.h"

class Canvas;

class InsertExistingNodeCommand : public QUndoCommand
{
public:
  InsertExistingNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& oldCenter, const QPointF& newCenter,
                            const TransitionSaveInfo& originalTransition, const TransitionSaveInfo& incomingTransition,
                            const TransitionSaveInfo& outgoingTransition, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;

  QString mNodeId;

  QPointF mOldCenter;
  QPointF mNewCenter;

  TransitionSaveInfo mOriginalTransition;
  TransitionSaveInfo mIncomingTransition;
  TransitionSaveInfo mOutgoingTransition;
};