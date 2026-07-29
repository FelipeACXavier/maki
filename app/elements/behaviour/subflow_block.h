#pragma once

#include <optional>

#include "elements/behaviour/behaviour_node.h"

class NodeConfig;
class QGraphicsProxyWidget;
class QLineEdit;

/**
 * @brief Bordered container below a Repeat / Within node for inline subflow editing.
 *
 * Not part of the flow palette; created programmatically and kept out of Flow persistence.
 * Canvas/NodeItem interact via isSubflowContainer / subflowHost / expandSubflowToFitChildren
 * rather than concrete casts.
 */
class SubflowBlock : public BehaviourNode
{
public:
  enum class Role
  {
    Loop,  ///< Repeat body
    Do,    ///< Within "do" branch
    Else   ///< Within "else" / timeout branch
  };

  static constexpr qreal kDefaultWidth = 320.0;
  static constexpr qreal kDefaultHeight = 100.0;

  static std::shared_ptr<NodeConfig> synthesizedConfig();

  /** Create, add to the owner's scene, start, and attach to @p owner. */
  static SubflowBlock* createAttached(NodeItem* owner, Role role);

  SubflowBlock(const QString& id,
               std::shared_ptr<NodeSaveInfo> info,
               const QPointF& initialPosition,
               std::shared_ptr<NodeConfig> nodeConfig,
               NodeItem* owner,
               Role role = Role::Loop,
               QGraphicsItem* parent = nullptr);

  bool isSubflowContainer() const override { return true; }
  NodeItem* subflowHost() const override { return mOwner; }
  void expandSubflowToFitChildren() override { expandToFitChildren(); }
  bool isCollapsedSubflow() const override { return mCollapsed; }

  Role role() const { return mRole; }

  /** Collapse hides the block entirely and shows only a label + expand arrow under the host. */
  void setCollapsed(bool collapsed);
  bool isCollapsed() const { return mCollapsed; }
  void toggleCollapsed() { setCollapsed(!mCollapsed); }

  /** Re-apply the collapsed flag persisted on the owning Repeat / Within node. */
  void applyPersistedCollapsedState();

  NodeItem* ownerNode() const { return mOwner; }
  void setOwnerNode(NodeItem* owner);

  /** Item the dashed connector attaches to above this block (defaults to owner). */
  void setConnectorAbove(NodeItem* above);
  NodeItem* connectorAbove() const { return mConnectorAbove ? mConnectorAbove : mOwner; }

  /** When this block moves/resizes, keep @p follower stacked directly below it. */
  void setStackFollower(SubflowBlock* follower);

  void syncToOwnerPosition();
  void syncBelow(NodeItem* above);
  void translateBy(const QPointF& delta);
  void expandToFitChildren();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;
  QPainterPath shape() const override;
  QRectF boundingRect() const override;

  void applySize(const QSizeF& size) override;
  void childRemoved(NodeItem* child) override;
  VoidResult start() override;

protected:
  void initializeNodeSize() override;

  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  void setBlockGeometry(const QPointF& topLeft, const QSizeF& size);
  void notifyStackFollower();
  /** Ask the enclosing subflow (owner's parent) to refit around this block. */
  void notifyEnclosingSubflow();
  void ensureTitleUi();
  void ensureDoTitleUi();
  void ensureLoopTitleUi();
  void syncTitleFieldsFromOwner();
  void applyTimeoutToOwner();
  void applyIterationsToOwner();
  void applyRateToOwner();
  std::optional<std::pair<QPointF, QPointF>> connectorSceneEndpoints() const;
  void paintConnector(QPainter* painter) const;
  void paintTitle(QPainter* painter) const;

  QRectF collapseButtonRect() const;
  QRectF collapsedCaptionRect() const;
  QSizeF collapsedChromeSize() const;
  qreal gapBelowPredecessor() const;
  /** Top-left X under the owner: left-aligned when expanded, centred when collapsed. */
  qreal alignedLeftUnderOwner(qreal width) const;
  void applyCollapsedChrome();
  void paintCollapseButton(QPainter* painter) const;
  void paintCollapsedChrome(QPainter* painter) const;
  void setContentsVisible(bool visible);
  QString collapsePropertyKey() const;
  void persistCollapsedState();

  NodeItem* mOwner = nullptr;
  NodeItem* mConnectorAbove = nullptr;
  SubflowBlock* mStackFollower = nullptr;
  Role mRole = Role::Loop;
  bool mSuppressExpand = false;
  bool mCollapsed = false;

  QGraphicsProxyWidget* mTitleProxy = nullptr;
  QLineEdit* mTimeoutEdit = nullptr;
  QLineEdit* mIterationsEdit = nullptr;
  QLineEdit* mRateEdit = nullptr;
};
