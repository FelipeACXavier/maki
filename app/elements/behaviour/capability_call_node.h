#pragma once

#include "elements/behaviour/behaviour_node.h"
#include "elements/structure/empty_slot.h"

class CapabilityCallNode : public BehaviourNode
{
public:
  CapabilityCallNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                     QGraphicsItem* parent = nullptr);

  virtual ~CapabilityCallNode();

  VoidResult start() override;

  void setProperty(const QString& key, const maki::Value& value) override;

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

private:
  /** True while the pointer is over the Task empty-slot (add-capability) control. */
  EmptySlot* mEmptySlot = nullptr;
  QSvgRenderer* mCapabilityRenderer = nullptr;
  QString mCapabilityIconPath = "";

  bool ensureRenderer();
};