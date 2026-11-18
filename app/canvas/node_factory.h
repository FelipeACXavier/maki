#pragma once

#include "canvas/elements/node.h"
#include "types.h"

class NodeFactory
{
public:
  static NodeItem* create(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                          Types::LibraryTypes canvasType, QGraphicsItem* parent = nullptr);
};
