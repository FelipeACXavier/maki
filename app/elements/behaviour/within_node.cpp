#include "elements/behaviour/within_node.h"

#include <QGraphicsScene>

#include "elements/behaviour/subflow_block.h"

WithinNode::WithinNode(const QString& id,
                       std::shared_ptr<NodeSaveInfo> info,
                       const QPointF& initialPosition,
                       std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  // Avoid DeviceCoordinateCache interactions with attached SubflowBlock scene items.
  setCacheMode(QGraphicsItem::NoCache);
}

WithinNode::~WithinNode()
{
  // Normal deletion goes through Canvas::removeNode, which calls detachOwnedSubflowBlocks()
  // first. This only runs if the Within node is destroyed some other way.
  for (NodeItem* block : detachOwnedSubflowBlocks())
  {
    if (block->scene())
      block->scene()->removeItem(block);
    if (auto* subflow = dynamic_cast<SubflowBlock*>(block))
      subflow->prepareForDeletion();
    delete block;
  }
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

  // Apply persisted collapsed state after both blocks are stacked so the Else
  // block re-stacks correctly when the Do block reloads collapsed.
  if (mDoBlock)
    mDoBlock->applyPersistedCollapsedState();
  if (mElseBlock)
    mElseBlock->applyPersistedCollapsedState();
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
