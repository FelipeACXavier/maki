#pragma once

#include <optional>

#include "elements/behaviour/behaviour_node.h"

class NodeConfig;
class QGraphicsProxyWidget;
class QLineEdit;
class QPainter;

/**
 * Shared collapse-chevron UI for Repeat / Within hosts.
 * One arrow sits beside the host label and toggles all owned subflow blocks together.
 */
namespace SubflowCollapseUi
{
constexpr qreal kButtonSize = 12.0;
constexpr qreal kGapFromLabel = 4.0;
constexpr const char* kPropertyKey = "subflowCollapsed";

/** Shift so label text + chevron stay centered under the host. */
qreal labelCenterOffsetX();
QRectF arrowRect(const NodeBase& host);
void paintArrow(QPainter* painter, const QRectF& rect, bool collapsed);
bool readPersisted(const NodeItem* host);
void writePersisted(NodeItem* host, bool collapsed);
}  // namespace SubflowCollapseUi

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

  /** Collapse hides this block and its contents entirely (chevron lives on the host). */
  void setCollapsed(bool collapsed);
  bool isCollapsed() const { return mCollapsed; }

  NodeItem* ownerNode() const { return mOwner; }
  void setOwnerNode(NodeItem* owner);

  /**
   * Drop owner/stack links and destroy embedded title widgets without scheduling
   * repaints. Must be called before the block leaves the scene / is deleted.
   */
  void prepareForDeletion();

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
  void destroyTitleUi();
  void syncTitleFieldsFromOwner();
  void applyTimeoutToOwner();
  void applyIterationsToOwner();
  void applyRateToOwner();
  std::optional<std::pair<QPointF, QPointF>> connectorSceneEndpoints() const;
  void paintConnector(QPainter* painter) const;
  void paintTitle(QPainter* painter) const;

  qreal gapBelowPredecessor() const;
  /** Top-left X under the owner (left-aligned with the host body). */
  qreal alignedLeftUnderOwner(qreal width) const;
  void setContentsVisible(bool visible);

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
