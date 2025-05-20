#pragma once

#include "canvas.h"

class Flow;

class BehaviourCanvas : public Canvas
{
public:
  BehaviourCanvas(Flow* flow, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;

private:
  Flow* mFlow;

  void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding) override;
};
