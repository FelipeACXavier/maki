#pragma once

#include "canvas.h"
#include "document/node_info.h"

class StructureCanvas : public Canvas
{
  Q_OBJECT
public:
  StructureCanvas(const QString& canvasId, std::shared_ptr<EdgeRouter> router, QObject* parent = nullptr);

  Types::LibraryTypes type() const override;

  QString requestCreateNode(NodeSaveInfo info, NodeCreation creation) override;
  void requestRemoveNode(NodeSaveInfo info) override;

  void addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info) override;
  void suggestCapability(NodeItem* node) override;
};
