#pragma once

#include "elements/node.h"

/**
 * @brief Structural system-view node (Tasks, capabilities, timers, etc.).
 */
class CapabilityNode : public NodeItem
{
public:
  CapabilityNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                 QGraphicsItem* parent = nullptr);

  ~CapabilityNode() override;

  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  void addParent(NodeItem* node) override;

  bool rendersAsInsetCapability() const;
};