#pragma once

#include "elements/node.h"
#include "empty_slot.h"

/**
 * @brief Structural system-view node (Tasks, capabilities, timers, etc.).
 */
class TaskNode : public NodeItem
{
public:
  TaskNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
           QGraphicsItem* parent = nullptr);

  ~TaskNode() override;

  VoidResult start() override;

  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info) override;
  void childRemoved(NodeItem* child) override;

  void settingsChanged(const AppearanceSettings& appearance) override;
  void swapCapabilityOrder(NodeItem* a, NodeItem* b);

protected:
  void initializeNodeSize() override;

private:
  /** True while the pointer is over the Task empty-slot (add-capability) control. */
  EmptySlot* mEmptySlot = nullptr;
  int mColumnCount = 2;

  struct SlotLayout
  {
    qreal diameter = 0.0;
    qreal horizontalGap = 0.0;
    qreal verticalGap = 0.0;
    qreal labelHeight = 0.0;
    int columnCount = 0;
    int rowCount = 0;
    QRectF contentRect = {};
  };

  SlotLayout slotLayout(const QSizeF& size, int slotCount) const;
  QVector<QPointF> capabilitySlotCenters(const SlotLayout& layout, int slotCount) const;
  qreal capabilityLabelHeight(qreal slotDiameter) const;

  void relayoutCapabilitySlots();
  QSizeF minimumSizeForCapabilitySlots(const SlotLayout& layout, int slotCount) const;
};