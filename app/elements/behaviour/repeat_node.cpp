#include "elements/behaviour/repeat_node.h"

#include <QGraphicsScene>

#include "elements/behaviour/subflow_block.h"

RepeatNode::RepeatNode(const QString& id,
                       std::shared_ptr<NodeSaveInfo> info,
                       const QPointF& initialPosition,
                       std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  // Avoid DeviceCoordinateCache interactions with attached SubflowBlock scene items.
  setCacheMode(QGraphicsItem::NoCache);
}

RepeatNode::~RepeatNode()
{
  // Normal deletion goes through Canvas::removeNode, which calls detachOwnedSubflowBlocks()
  // first. This only runs if the Repeat node is destroyed some other way.
  for (NodeItem* block : detachOwnedSubflowBlocks())
  {
    if (block->scene())
      block->scene()->removeItem(block);
    if (auto* subflow = dynamic_cast<SubflowBlock*>(block))
      subflow->prepareForDeletion();
    delete block;
  }
}

QVector<NodeItem*> RepeatNode::detachOwnedSubflowBlocks()
{
  QVector<NodeItem*> blocks;
  if (!mBlock)
    return blocks;

  // prepareForDeletion() is the caller's job, once the block is out of the scene:
  // it shrinks the block's boundingRect, which is only safe when unindexed.
  blocks.append(mBlock);
  mBlock = nullptr;
  return blocks;
}

void RepeatNode::ensureSubflowBlock()
{
  if (mBlock)
    return;

  mBlock = SubflowBlock::createAttached(this, SubflowBlock::Role::Loop);
  if (mBlock)
  {
    mBlock->syncToOwnerPosition();
    mBlock->applyPersistedCollapsedState();
  }
}

void RepeatNode::updatePosition(const QPointF& position)
{
  const QPointF delta = position - mLastPosition;
  BehaviourNode::updatePosition(position);
  if (mBlock && !delta.isNull())
    mBlock->translateBy(delta);
}

QVariant RepeatNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemSceneHasChanged && scene())
    ensureSubflowBlock();

  return BehaviourNode::itemChange(change, value);
}
