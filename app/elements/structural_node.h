#pragma once

#include "elements/node.h"

class StructureCanvas;

namespace structural_layout
{
void layoutNonLayeredTidyTree(NodeItem* root);
}

/**
 * @brief Structural system-view node (Tasks, capabilities, timers, etc.).
 */
class StructuralNode : public NodeItem
{
public:
  StructuralNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  ~StructuralNode() override;

  VoidResult start() override;

  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  void addParent(NodeItem* node) override;
  void childRemoved(NodeItem* child) override;

  bool isTaskContainer() const override;
  bool isStructuralSubtask() const override;
  bool rendersAsInsetCapability() const override;

  QVector<NodeItem*> structuralSubtaskChildren() const override;
  QVector<NodeItem*> structuralCapabilityChildren() const override;

  void layoutSubtasks() override;
  void relayoutCapabilitySlots() override;
  void swapCapabilityOrder(NodeItem* a, NodeItem* b) override;
  NodeItem* capabilityAtScenePos(const QPointF& scenePos, NodeItem* exclude = nullptr) const override;

  QRectF placeholderSlotSceneRect() const override;
  bool placeholderSlotContainsScenePoint(const QPointF& scenePos) const override;

  void ensureSubtaskConnector(StructureCanvas* canvas) override;
  void destroySubtaskConnector() override;
  void syncSubtaskConnector() override;

  void applySize(const QSizeF& size) override;
  void setHoverPreview(const QString& iconPath, const QColor& color, bool active) override;
  void updatePosition(const QPointF& position) override;

  void applyStructuralLayoutTopLeft(const QPointF& topLeftScene) override;
  void finalizeStructuralPackedPositions(const QPointF& subtreeSceneTopLeft) override;

protected:
  void initializeNodeSize() override;
  void configurePorts() override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

  NodeItem* rootStructuralTask() const override;

private:
  void setEmptySlotHovered(bool hovered);

  bool mIsResizing{false};
  QPointF mResizeStartMousePos{0, 0};
  QSizeF mResizeStartSize{0, 0};

  int mCapDragStartIndex = -1;
  QPointF mTreeDragRootStartPos{0, 0};
  QPointF mTreeDragStartScenePos{0, 0};

  class SubtaskConnector* mSubtaskConnector = nullptr;

  bool mHoverPreviewActive = false;
  QString mHoverPreviewIcon;
  QColor mHoverPreviewColor;

  /** True while the pointer is over the Task empty-slot (add-capability) control. */
  bool mEmptySlotHovered = false;
};
