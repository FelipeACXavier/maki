#pragma once

#include "canvas.h"

class NodeItem;

class StructureCanvas : public Canvas
{
public:
  StructureCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;

  void createSubtask(NodeItem* parentTask);

protected:
  void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding) override;
};
