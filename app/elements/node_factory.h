#pragma once

#include "node.h"

class NodeFactory
{
public:
  static NodeItem* create(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                          QGraphicsItem* parent = nullptr);
};