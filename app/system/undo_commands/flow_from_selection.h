#pragma once

#include <QUndoCommand>

#include "flow_info.h"
#include "node_info.h"
#include "transition_info.h"

class Canvas;

class FlowFromSelectionCommand : public QUndoCommand
{
public:
  FlowFromSelectionCommand(Canvas* canvas, const QString& flowId, const QString& flowOwner, const FlowSaveInfo& flowInfo, const NodeSaveInfo& startNode,
                           const NodeSaveInfo& endNode, const TransitionSaveInfo& startTransition, const TransitionSaveInfo& endTransition,
                           QUndoCommand* parent = nullptr);

  void undo() override;
  void redo() override;

private:
  Canvas* mCanvas = nullptr;
  const QString mFlowId;
  const QString mFlowOwner;
  const FlowSaveInfo mFlowInfo;
  const NodeSaveInfo mStartNode;
  const NodeSaveInfo mEndNode;
  const TransitionSaveInfo mStartTransition;
  const TransitionSaveInfo mEndTransition;
};
