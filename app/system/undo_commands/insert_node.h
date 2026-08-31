#pragma once

#include <QUndoCommand>

#include "node_info.h"
#include "transition_info.h"

class Canvas;

class InsertNodeCommand : public QUndoCommand
{
public:
  InsertNodeCommand(Canvas* canvas, const NodeSaveInfo& node, const TransitionSaveInfo& originalTransition,
                    const TransitionSaveInfo& incomingTransition, const TransitionSaveInfo& outgoingTransition, QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;

  NodeSaveInfo mNode;

  TransitionSaveInfo mOriginalTransition;
  TransitionSaveInfo mIncomingTransition;
  TransitionSaveInfo mOutgoingTransition;
};