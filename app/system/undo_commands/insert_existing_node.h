#pragma once

#include <QUndoCommand>

#include "transition_info.h"

class Canvas;

class InsertExistingNodeCommand : public QUndoCommand
{
public:
  InsertExistingNodeCommand(Canvas* canvas, const QString& nodeId, const QPointF& oldCenter, const QPointF& newCenter, const QString& oldParent,
                            const QString& newParent, const TransitionSaveInfo& originalTransition, const TransitionSaveInfo& incomingTransition,
                            const TransitionSaveInfo& outgoingTransition, const QString& originalSubflow, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;

  QString mNodeId;
  QString mOldParentId;
  QString mNewParentId;

  QPointF mOldCenter;
  QPointF mNewCenter;

  TransitionSaveInfo mOriginalTransition;
  TransitionSaveInfo mIncomingTransition;
  TransitionSaveInfo mOutgoingTransition;

  QString mOriginalSubflow;
};
