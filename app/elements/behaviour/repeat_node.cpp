#include "elements/behaviour/repeat_node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

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

bool RepeatNode::subflowsCollapsed() const
{
  return mBlock && mBlock->isCollapsed();
}

void RepeatNode::setSubflowsCollapsed(bool collapsed)
{
  if (mBlock)
    mBlock->setCollapsed(collapsed);
  SubflowCollapseUi::writePersisted(this, collapsed);
  update();
}

void RepeatNode::toggleSubflowsCollapsed()
{
  setSubflowsCollapsed(!subflowsCollapsed());
}

void RepeatNode::ensureSubflowBlock()
{
  if (mBlock)
    return;

  mBlock = SubflowBlock::createAttached(this, SubflowBlock::Role::Loop);
  if (mBlock)
  {
    mBlock->syncToOwnerPosition();
    if (SubflowCollapseUi::readPersisted(this))
      setSubflowsCollapsed(true);
  }
}

void RepeatNode::updatePosition(const QPointF& position)
{
  const QPointF delta = position - mLastPosition;
  BehaviourNode::updatePosition(position);
  if (mBlock && !delta.isNull())
    mBlock->translateBy(delta);
}

qreal RepeatNode::labelCenterOffsetX() const
{
  return SubflowCollapseUi::labelCenterOffsetX();
}

QRectF RepeatNode::boundingRect() const
{
  return BehaviourNode::boundingRect().united(SubflowCollapseUi::arrowRect(*this));
}

QPainterPath RepeatNode::shape() const
{
  QPainterPath path = BehaviourNode::shape();
  path.addRect(SubflowCollapseUi::arrowRect(*this));
  return path;
}

void RepeatNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);
  SubflowCollapseUi::paintArrow(painter, SubflowCollapseUi::arrowRect(*this), subflowsCollapsed());
}

void RepeatNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event && SubflowCollapseUi::arrowRect(*this).contains(event->pos()))
  {
    toggleSubflowsCollapsed();
    event->accept();
    return;
  }
  BehaviourNode::mousePressEvent(event);
}

QVariant RepeatNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemSceneHasChanged && scene())
    ensureSubflowBlock();

  return BehaviourNode::itemChange(change, value);
}
