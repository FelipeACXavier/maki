#include "elements/behaviour/within_node.h"

#include <QGraphicsScene>

WithinNode::WithinNode(const QString& id,
                       std::shared_ptr<NodeSaveInfo> info,
                       const QPointF& initialPosition,
                       std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : SubflowHostNode(id, info, initialPosition, nodeConfig, parent)
{
}

WithinNode::~WithinNode()
{
  // Normal deletion goes through Canvas::removeNode, which calls detachOwnedSubflowBlocks()
  // first. This only runs if the Within node is destroyed some other way.
  destroyDetachedBlocks(detachOwnedSubflowBlocks());
}

QVector<NodeItem*> WithinNode::detachOwnedSubflowBlocks()
{
  QVector<NodeItem*> blocks;

  // Break the Do↔Else stack link; it does not feed boundingRect(), so no
  // geometry notification is needed here.
  if (mDoBlock)
    mDoBlock->setStackFollower(nullptr);

  // prepareForDeletion() is the caller's job, once the blocks are out of the
  // scene: it shrinks their boundingRect, which is only safe when unindexed.
  if (mDoBlock)
  {
    blocks.append(mDoBlock);
    mDoBlock = nullptr;
  }

  if (mElseBlock)
  {
    blocks.append(mElseBlock);
    mElseBlock = nullptr;
  }

  return blocks;
}

bool WithinNode::subflowsCollapsed() const
{
  return (mDoBlock && mDoBlock->isCollapsed()) || (mElseBlock && mElseBlock->isCollapsed());
}

void WithinNode::setSubflowsCollapsed(bool collapsed)
{
  if (collapsed)
  {
    // Hide the follower first so it does not restack against a collapsing Do.
    if (mElseBlock)
      mElseBlock->setCollapsed(true);
    if (mDoBlock)
      mDoBlock->setCollapsed(true);
  }
  else
  {
    // Expand Do first so Else can stack under the restored geometry.
    if (mDoBlock)
      mDoBlock->setCollapsed(false);
    if (mElseBlock)
      mElseBlock->setCollapsed(false);
    // Push Else down only if it overlaps Do; keep a larger remembered gap.
    if (mElseBlock && mDoBlock)
      mElseBlock->ensureStackedBelow(mDoBlock);
  }

  SubflowCollapseUi::writePersisted(this, collapsed);
  update();
}

void WithinNode::toggleSubflowsCollapsed()
{
  setSubflowsCollapsed(!subflowsCollapsed());
}

void WithinNode::ensureSubflowBlocks()
{
  if (mDoBlock && mElseBlock)
    return;

  if (!scene())
    return;

  if (!mDoBlock)
  {
    mDoBlock = SubflowBlock::createAttached(this, SubflowBlock::Role::Do);
    if (mDoBlock)
      mDoBlock->syncToOwnerPosition();
  }

  if (!mElseBlock)
  {
    mElseBlock = SubflowBlock::createAttached(this, SubflowBlock::Role::Else);
    if (mElseBlock && mDoBlock)
    {
      mElseBlock->setConnectorAbove(mDoBlock);
      mDoBlock->setStackFollower(mElseBlock);
      mElseBlock->syncBelow(mDoBlock);
    }
  }

  if (SubflowCollapseUi::readPersisted(this))
    setSubflowsCollapsed(true);
}

void WithinNode::updatePosition(const QPointF& position)
{
  const QPointF delta = position - mLastPosition;
  BehaviourNode::updatePosition(position);
  if (delta.isNull())
    return;

  // Move the do block; the else block follows via stack-follower sync.
  if (mDoBlock)
    mDoBlock->translateBy(delta);
  else if (mElseBlock)
    mElseBlock->translateBy(delta);
}

QVariant WithinNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemSceneHasChanged && scene())
    ensureSubflowBlocks();

  return BehaviourNode::itemChange(change, value);
}
