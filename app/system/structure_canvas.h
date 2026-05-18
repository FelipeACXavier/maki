#pragma once

#include "canvas.h"
#include "node_info.h"

class StructureCanvas : public Canvas
{
  Q_OBJECT
public:
  StructureCanvas(std::shared_ptr<SaveInfo> storage, const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;

  void addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info) override;

private:
  std::shared_ptr<SaveInfo> mStorage;

  void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding) override;
};
