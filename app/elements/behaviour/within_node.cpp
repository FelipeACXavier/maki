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
}

WithinNode::~WithinNode()
{
  // Normal deletion goes through Canvas::removeNode, which calls detachOwnedSubflowBlocks()
  // first. This only runs if the Within node is destroyed some other way.
  for (NodeItem* block : detachOwnedSubflowBlocks())
  {
    if (block->scene())
      block->scene()->removeItem(block);
    delete block;
  }
}

QVector<NodeItem*> WithinNode::detachOwnedSubflowBlocks()
{
  QVector<NodeItem*> blocks;

  if (mDoBlock)
  {
    mDoBlock->setStackFollower(nullptr);
    mDoBlock->setConnectorAbove(nullptr);
    mDoBlock->setOwnerNode(nullptr);
    blocks.append(mDoBlock);
    mDoBlock = nullptr;
  }

  if (mElseBlock)
  {
    mElseBlock->setStackFollower(nullptr);
    mElseBlock->setConnectorAbove(nullptr);
    mElseBlock->setOwnerNode(nullptr);
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
