#include "elements/behaviour/subflow_block.h"

#include <QFont>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSet>
#include <QStyleOptionGraphicsItem>
#include <QUuid>
#include <algorithm>

#include "app_configs.h"
#include "config.h"
#include "elements/port.h"
#include "elements/transition.h"
#include "keys.h"
#include "style_helpers.h"
#include "types.h"

namespace
{
constexpr qreal kInnerPadding = 24.0;
constexpr qreal kPortLaneWidth = 36.0;
/** Wide enough for "Loop [n] iterations every [n] ms" title controls. */
constexpr qreal kMinBlockWidth = 320.0;
constexpr qreal kMinBlockHeight = 100.0;
/** Height of the header strip left visible when the block is collapsed. */
constexpr qreal kCollapsedHeight = 44.0;
/** Size of the collapse/expand chevron in item coordinates. */
constexpr qreal kCollapseButtonSize = 16.0;
constexpr qreal kCollapseButtonMargin = 8.0;
constexpr qreal kCornerRadius = 10.0;
/** Visible dashed-line gap below the owner label (or body if unlabeled). */
constexpr qreal kGapBelowOwner = 40.0;
constexpr qreal kTitleLeft = 10.0;
constexpr qreal kTitleTop = 6.0;
const QColor kConnectorColor(0xb0, 0xb0, 0xb0);
const QString kTimeoutPropertyId = QStringLiteral("timeout");
const QString kIterationsPropertyId = QStringLiteral("iterations");
const QString kRatePropertyId = QStringLiteral("rate");

const char* kTitleWidgetStyle =
    "QLabel { color: #404040; background: transparent; }"
    "QLineEdit { background: white; border: 1px solid #a0a0a0; border-radius: 2px; padding: 1px 3px; }";

QLineEdit* makeIntEdit(QWidget* parent, int width, const QString& tooltip)
{
  auto* edit = new QLineEdit(parent);
  edit->setFixedWidth(width);
  edit->setAlignment(Qt::AlignCenter);
  edit->setValidator(new QIntValidator(0, 2'000'000'000, edit));
  edit->setToolTip(tooltip);
  return edit;
}

void syncEditFromProperty(QLineEdit* edit, NodeItem* owner, const QString& propertyId)
{
  if (!edit || !owner || edit->hasFocus())
    return;

  const QVariant value = owner->getProperty(propertyId);
  const QString text = value.isValid() ? QString::number(value.toInt()) : QString();
  if (edit->text() != text)
    edit->setText(text);
}

bool applyEditToProperty(QLineEdit* edit, NodeItem* owner, const QString& propertyId)
{
  if (!edit || !owner)
    return false;

  bool ok = false;
  const int value = edit->text().trimmed().toInt(&ok);
  if (!ok)
    return false;

  owner->setProperty(propertyId, value);
  return true;
}

qreal sceneBottomBelowItem(NodeItem* item)
{
  if (!item)
    return 0.0;

  // Prefer the name label bottom so placement leaves room for the connector.
  const QRectF labelLocal = item->labelBoundingRect();
  if (!labelLocal.isEmpty())
    return item->mapRectToScene(labelLocal).bottom();

  return item->mapRectToScene(item->nodeRect()).bottom();
}
/** Minimum grab strip width in screen pixels (converted to item coords at paint/drag time). */
constexpr qreal kMinBorderGrabScreenPx = 18.0;
/** Fallback grab width in item coordinates when scale is unavailable. */
constexpr qreal kMinBorderGrabScene = 16.0;

bool isBlockChromeHit(const QRectF& outer, const QPointF& localPos, qreal grabWidth, qreal bodyInset)
{
  if (!outer.contains(localPos))
    return false;

  // The inner canvas stays non-draggable so nodes inside can be moved freely.
  const QRectF body = outer.adjusted(bodyInset, bodyInset, -bodyInset, -bodyInset);
  if (body.contains(localPos))
    return false;

  const qreal distLeft = localPos.x() - outer.left();
  const qreal distRight = outer.right() - localPos.x();
  const qreal distTop = localPos.y() - outer.top();
  const qreal distBottom = outer.bottom() - localPos.y();
  const qreal edgeDist = qMin(qMin(distLeft, distRight), qMin(distTop, distBottom));
  return edgeDist <= grabWidth;
}

qreal borderGrabWidth(const QGraphicsItem* item, const QRectF& outer)
{
  const QTransform transform = item->sceneTransform();
  const qreal scale = qSqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
  if (scale <= 0.0)
    return kMinBorderGrabScene;

  const qreal grab = qMax(kMinBorderGrabScene, kMinBorderGrabScreenPx / scale);
  const qreal maxGrab = qMin(outer.width(), outer.height()) * 0.45;
  return qMin(grab, maxGrab);
}

QString titleTextForRole(SubflowBlock::Role role)
{
  switch (role)
  {
    case SubflowBlock::Role::Do:
      return QStringLiteral("Do within");
    case SubflowBlock::Role::Else:
      return QStringLiteral("On timeout");
    case SubflowBlock::Role::Loop:
    default:
      return QStringLiteral("Loop");
  }
}

QFont boldTitleFont(const QFont& base)
{
  QFont font = base;
  font.setBold(true);
  font.setPointSizeF(qMax(8.0, kInnerPadding * 0.4));
  return font;
}
}  // namespace

std::shared_ptr<NodeConfig> SubflowBlock::synthesizedConfig()
{
  static std::shared_ptr<NodeConfig> config;
  if (config)
    return config;

  config = std::make_shared<NodeConfig>();
  config->type = QStringLiteral("Koda::SubflowBlock");
  config->libraryType = Types::LibraryTypes::BEHAVIOUR;
  config->body.width = static_cast<int>(kMinBlockWidth);
  config->body.height = static_cast<int>(kMinBlockHeight);
  config->body.borderRadius = static_cast<int>(kCornerRadius);
  config->hasInPort = false;
  config->hasOutPort = true;
  return config;
}

SubflowBlock* SubflowBlock::createAttached(NodeItem* owner, Role role)
{
  if (!owner || !owner->scene())
    return nullptr;

  auto info = std::make_shared<NodeSaveInfo>();
  info->setId(QUuid::createUuid().toString());
  info->setNodeId(synthesizedConfig()->type);
  info->setSize(QSizeF(kDefaultWidth, kDefaultHeight));

  QString name = QStringLiteral("Subflow");
  if (role == Role::Do)
    name = QStringLiteral("Do");
  else if (role == Role::Else)
    name = QStringLiteral("Else");
  info->addProperty(ConfigKeys::NAME, name);

  auto* block = new SubflowBlock(info->getid(), info, owner->pos(), synthesizedConfig(), owner, role);
  owner->scene()->addItem(block);
  block->start();
  return block;
}

SubflowBlock::SubflowBlock(const QString& id,
                           std::shared_ptr<NodeSaveInfo> info,
                           const QPointF& initialPosition,
                           std::shared_ptr<NodeConfig> nodeConfig,
                           NodeItem* owner,
                           Role role,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
    , mOwner(owner)
    , mRole(role)
{
  setAcceptDrops(true);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setZValue(owner ? owner->zValue() - 1 : -1);
  setLabelName(QString());
}

void SubflowBlock::setOwnerNode(NodeItem* owner)
{
  mOwner = owner;
  if (owner)
    setZValue(owner->zValue() - 1);
}

void SubflowBlock::setConnectorAbove(NodeItem* above)
{
  mConnectorAbove = above;
  prepareGeometryChange();
  update();
}

void SubflowBlock::setStackFollower(SubflowBlock* follower)
{
  mStackFollower = follower;
}

void SubflowBlock::notifyStackFollower()
{
  if (mStackFollower)
    mStackFollower->syncBelow(this);
}

QVariant SubflowBlock::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange && mOwner)
  {
    // Constrain movement to the vertical axis: X stays locked to the owner's
    // left edge, and the block is never allowed above the owner (or above its
    // stacked predecessor for Within "else").
    QPointF newPos = value.toPointF();
    const QRectF ownerScene = mOwner->mapRectToScene(mOwner->nodeRect());
    newPos.setX(ownerScene.left());

    qreal minTop = sceneBottomBelowItem(mOwner) + kGapBelowOwner;
    if (mConnectorAbove && mConnectorAbove != mOwner)
      minTop = qMax(minTop, sceneBottomBelowItem(mConnectorAbove) + kGapBelowOwner);
    newPos.setY(qMax(newPos.y(), minTop));

    return NodeItem::itemChange(change, QVariant(newPos));
  }

  return NodeItem::itemChange(change, value);
}

void SubflowBlock::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (collapseButtonRect().contains(event->pos()))
  {
    toggleCollapsed();
    event->accept();
    return;
  }

  if (!isBlockChromeHit(nodeRect(), event->pos(), borderGrabWidth(this, nodeRect()), kInnerPadding))
  {
    event->ignore();
    return;
  }

  mDragStartPos = pos();
  // While the user drags the block, its children move with it; suppress the
  // fit-to-children pass so those cascaded moves do not fight the drag.
  mSuppressExpand = true;
  QGraphicsItem::mousePressEvent(event);
}

void SubflowBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  mSuppressExpand = false;
  notifyStackFollower();
  QGraphicsItem::mouseReleaseEvent(event);
}

VoidResult SubflowBlock::start()
{
  const VoidResult result = BehaviourNode::start();
  ensureTitleUi();
  return result;
}

void SubflowBlock::ensureTitleUi()
{
  if (mTitleProxy)
    return;

  if (mRole == Role::Do)
    ensureDoTitleUi();
  else if (mRole == Role::Loop)
    ensureLoopTitleUi();

  // Title row (esp. Loop) needs more width than the old empty default.
  if ((mRole == Role::Do || mRole == Role::Loop) && mSize.width() < kMinBlockWidth)
    expandToFitChildren();
}

void SubflowBlock::ensureDoTitleUi()
{
  auto* container = new QWidget;
  container->setAttribute(Qt::WA_TranslucentBackground);
  container->setStyleSheet(QString::fromUtf8(kTitleWidgetStyle));

  auto* layout = new QHBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  const QFont titleFont = boldTitleFont(container->font());

  auto* prefix = new QLabel(QStringLiteral("Do within"), container);
  prefix->setFont(titleFont);

  mTimeoutEdit = makeIntEdit(container, 52, QStringLiteral("Timeout in milliseconds"));

  auto* suffix = new QLabel(QStringLiteral("ms"), container);
  suffix->setFont(titleFont);

  layout->addWidget(prefix);
  layout->addWidget(mTimeoutEdit);
  layout->addWidget(suffix);
  layout->addStretch();
  container->adjustSize();

  mTitleProxy = new QGraphicsProxyWidget(this);
  mTitleProxy->setWidget(container);
  mTitleProxy->setPos(kTitleLeft, kTitleTop);
  mTitleProxy->setZValue(10);

  syncTitleFieldsFromOwner();
  QObject::connect(mTimeoutEdit, &QLineEdit::editingFinished, mTimeoutEdit, [this]() { applyTimeoutToOwner(); });
}

void SubflowBlock::ensureLoopTitleUi()
{
  auto* container = new QWidget;
  container->setAttribute(Qt::WA_TranslucentBackground);
  container->setStyleSheet(QString::fromUtf8(kTitleWidgetStyle));

  auto* layout = new QHBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  const QFont titleFont = boldTitleFont(container->font());

  auto* loopLabel = new QLabel(QStringLiteral("Loop"), container);
  loopLabel->setFont(titleFont);

  mIterationsEdit = makeIntEdit(container, 44, QStringLiteral("Number of iterations (0 = indefinitely)"));

  auto* iterationsLabel = new QLabel(QStringLiteral("iterations every"), container);
  iterationsLabel->setFont(titleFont);

  mRateEdit = makeIntEdit(container, 52, QStringLiteral("Delay between iterations in milliseconds"));

  auto* msLabel = new QLabel(QStringLiteral("ms"), container);
  msLabel->setFont(titleFont);

  layout->addWidget(loopLabel);
  layout->addWidget(mIterationsEdit);
  layout->addWidget(iterationsLabel);
  layout->addWidget(mRateEdit);
  layout->addWidget(msLabel);
  layout->addStretch();
  container->adjustSize();

  mTitleProxy = new QGraphicsProxyWidget(this);
  mTitleProxy->setWidget(container);
  mTitleProxy->setPos(kTitleLeft, kTitleTop);
  mTitleProxy->setZValue(10);

  syncTitleFieldsFromOwner();
  QObject::connect(mIterationsEdit, &QLineEdit::editingFinished, mIterationsEdit, [this]() { applyIterationsToOwner(); });
  QObject::connect(mRateEdit, &QLineEdit::editingFinished, mRateEdit, [this]() { applyRateToOwner(); });
}

void SubflowBlock::syncTitleFieldsFromOwner()
{
  if (!mOwner)
    return;

  syncEditFromProperty(mTimeoutEdit, mOwner, kTimeoutPropertyId);
  syncEditFromProperty(mIterationsEdit, mOwner, kIterationsPropertyId);
  syncEditFromProperty(mRateEdit, mOwner, kRatePropertyId);
}

void SubflowBlock::applyTimeoutToOwner()
{
  if (!applyEditToProperty(mTimeoutEdit, mOwner, kTimeoutPropertyId))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::applyIterationsToOwner()
{
  if (!applyEditToProperty(mIterationsEdit, mOwner, kIterationsPropertyId))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::applyRateToOwner()
{
  if (!applyEditToProperty(mRateEdit, mOwner, kRatePropertyId))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::configurePorts()
{
  if (config()->hasOutPort)
    mOutPort = new PortItem(PortItem::Out, this);
  updatePortPositions();
}

void SubflowBlock::initializeNodeSize()
{
  if (mSize.width() <= 0.0 || mSize.height() <= 0.0)
    mSize = QSizeF(kMinBlockWidth, kMinBlockHeight);
  mStorage->setSize(mSize);
}

void SubflowBlock::updatePortPositions()
{
  if (!mOutPort)
    return;

  const qreal midY = nodeRect().center().y();
  mOutPort->setPos(kInnerPadding * 0.5, midY - PortItem::kSize * 0.5);
}

void SubflowBlock::syncToOwnerPosition()
{
  if (!mOwner)
    return;

  mSuppressExpand = true;
  const QRectF ownerScene = mOwner->mapRectToScene(mOwner->nodeRect());
  const QPointF newTopLeft(ownerScene.left(), sceneBottomBelowItem(mOwner) + kGapBelowOwner);
  updatePosition(newTopLeft);
  mSuppressExpand = false;
  notifyStackFollower();
}

void SubflowBlock::syncBelow(NodeItem* above)
{
  if (!above || !mOwner)
    return;

  mSuppressExpand = true;
  const QRectF ownerScene = mOwner->mapRectToScene(mOwner->nodeRect());
  const QPointF newTopLeft(ownerScene.left(), sceneBottomBelowItem(above) + kGapBelowOwner);
  updatePosition(newTopLeft);
  mSuppressExpand = false;
  notifyStackFollower();
}

void SubflowBlock::translateBy(const QPointF& delta)
{
  if (delta.isNull())
    return;

  mSuppressExpand = true;
  updatePosition(pos() + delta);
  mSuppressExpand = false;
  notifyStackFollower();
}

void SubflowBlock::setBlockGeometry(const QPointF& topLeft, const QSizeF& size)
{
  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);
  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  // Resizing must not drag the logical children along: they keep their scene
  // positions while the border grows to enclose them.
  mSuppressChildCascade = true;
  setPos(topLeft);
  mSuppressChildCascade = false;

  mLastPosition = topLeft;
  mStorage->setPosition(pos() + boundingRect().center());
  updatePortPositions();
  update();
  notifyStackFollower();
}

void SubflowBlock::expandToFitChildren()
{
  if (mSuppressExpand || mCollapsed)
    return;

  // Guard against reentrancy: repositioning the block emits move notifications
  // that would otherwise route back here and recurse.
  mSuppressExpand = true;

  // The block is left-aligned to the owner and only grows to the right and
  // downwards. Its top edge is preserved so the user can freely move it vertically.
  qreal left = pos().x();
  const qreal top = pos().y();
  if (mOwner)
    left = mOwner->mapRectToScene(mOwner->nodeRect()).left();

  // Right/bottom are fitted exactly to the children (plus padding and the port
  // lanes), so the block both grows and shrinks as nodes move.
  qreal right = left + kMinBlockWidth;
  qreal bottom = top + kInnerPadding + PortItem::kSize;

  for (NodeItem* child : children())
  {
    const QRectF childScene = child->sceneBoundingRect();
    right = qMax(right, childScene.right() + kInnerPadding + kPortLaneWidth);
    bottom = qMax(bottom, childScene.bottom() + kInnerPadding);
  }

  const qreal newWidth = qMax(kMinBlockWidth, right - left);
  const qreal newHeight = qMax(kMinBlockHeight, bottom - top);

  setBlockGeometry(QPointF(left, top), QSizeF(newWidth, newHeight));

  mSuppressExpand = false;
}

void SubflowBlock::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);
  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  const qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabelSize(newFontSize, mSize);
  updatePortPositions();
  update();
}

void SubflowBlock::childRemoved(NodeItem* child)
{
  BehaviourNode::childRemoved(child);
  if (mOwner)
    expandToFitChildren();
}

QPainterPath SubflowBlock::shape() const
{
  // Only the border chrome is hittable so interior nodes/transitions receive
  // clicks and drops. Drop-into-block uses nodeRect via dropTargetContainer.
  QPainterPath outer;
  outer.addRoundedRect(nodeRect(), kCornerRadius, kCornerRadius);

  const QRectF innerRect = nodeRect().adjusted(kInnerPadding, kInnerPadding, -kInnerPadding, -kInnerPadding);
  if (innerRect.width() <= 0.0 || innerRect.height() <= 0.0)
    return outer;

  QPainterPath inner;
  const qreal innerRadius = qMax(0.0, kCornerRadius - kInnerPadding * 0.5);
  inner.addRoundedRect(innerRect, innerRadius, innerRadius);
  return outer.subtracted(inner);
}

std::optional<std::pair<QPointF, QPointF>> SubflowBlock::connectorSceneEndpoints() const
{
  NodeItem* above = connectorAbove();
  if (!above || !mOwner)
    return std::nullopt;

  const QRectF ownerScene = mOwner->mapRectToScene(mOwner->nodeRect());
  const QRectF blockScene = mapRectToScene(nodeRect());
  // Keep a single vertical spine aligned to the owning node's center.
  const qreal x = ownerScene.center().x();

  // From the owner (Repeat/Within), start below the name label so the line
  // does not overlap the text. Between stacked blocks, use the body bottom.
  qreal startY = 0.0;
  if (above == mOwner)
  {
    const QRectF labelLocal = mOwner->labelBoundingRect();
    if (!labelLocal.isEmpty())
      startY = mOwner->mapRectToScene(labelLocal).bottom();
    else
      startY = ownerScene.bottom();
  }
  else
  {
    startY = above->mapRectToScene(above->nodeRect()).bottom();
  }

  if (blockScene.top() >= startY)
    return std::make_pair(QPointF(x, startY), QPointF(x, blockScene.top()));

  return std::nullopt;
}

QRectF SubflowBlock::boundingRect() const
{
  QRectF rect = nodeRect();
  const auto endpoints = connectorSceneEndpoints();
  if (!endpoints)
    return rect;

  const QPointF p1 = mapFromScene(endpoints->first);
  const QPointF p2 = mapFromScene(endpoints->second);
  const qreal margin = 4.0 / baseScale();
  const QRectF connectorRect = QRectF(p1, p2).normalized().adjusted(-margin, -margin, margin, margin);
  return rect.united(connectorRect);
}

void SubflowBlock::paintConnector(QPainter* painter) const
{
  const auto endpoints = connectorSceneEndpoints();
  if (!endpoints)
    return;

  const QPointF p1 = mapFromScene(endpoints->first);
  const QPointF p2 = mapFromScene(endpoints->second);

  QPen dashPen(kConnectorColor, 1.0 / baseScale(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
  painter->setPen(dashPen);
  painter->setBrush(Qt::NoBrush);
  painter->drawLine(p1, p2);
}

void SubflowBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  paintConnector(painter);

  const QPen borderPen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.5 / baseScale());
  painter->setPen(borderPen);
  painter->setBrush(Qt::NoBrush);
  painter->drawRoundedRect(nodeRect().adjusted(0.5, 0.5, -0.5, -0.5), kCornerRadius, kCornerRadius);

  if (isSelected())
  {
    QPen selPen(Config::HIGHLIGHT);
    selPen.setWidthF(2.0);
    selPen.setCosmetic(true);
    painter->setPen(selPen);
    painter->drawRoundedRect(nodeRect().adjusted(0.5, 0.5, -0.5, -0.5), kCornerRadius, kCornerRadius);
  }

  // Keep inline title fields in sync with the properties panel when not editing.
  if (mRole == Role::Do || mRole == Role::Loop)
    syncTitleFieldsFromOwner();
  else
    paintTitle(painter);

  paintCollapseButton(painter);
}

void SubflowBlock::paintTitle(QPainter* painter) const
{
  QFont font = boldTitleFont(painter->font());
  painter->setFont(font);
  painter->setPen(QPen(Config::FOREGROUND));

  const QRectF titleRect(kTitleLeft, kTitleTop, nodeRect().width() - 2.0 * kTitleLeft, font.pointSizeF() * 2.0);
  painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, titleTextForRole(mRole));
}

QRectF SubflowBlock::collapseButtonRect() const
{
  const QRectF outer = nodeRect();
  return QRectF(outer.right() - kCollapseButtonSize - kCollapseButtonMargin,
                outer.top() + kCollapseButtonMargin,
                kCollapseButtonSize,
                kCollapseButtonSize);
}

void SubflowBlock::paintCollapseButton(QPainter* painter) const
{
  const QRectF r = collapseButtonRect();

  painter->save();
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(Config::FOREGROUND));

  QPolygonF triangle;
  if (mCollapsed)
  {
    // Right-pointing chevron: click to expand.
    const qreal inset = r.height() * 0.15;
    triangle << QPointF(r.left() + inset, r.top())
             << QPointF(r.left() + inset, r.bottom())
             << QPointF(r.right() - inset, r.center().y());
  }
  else
  {
    // Down-pointing chevron: click to collapse.
    const qreal inset = r.width() * 0.15;
    triangle << QPointF(r.left(), r.top() + inset)
             << QPointF(r.right(), r.top() + inset)
             << QPointF(r.center().x(), r.bottom() - inset);
  }

  painter->drawPolygon(triangle);
  painter->restore();
}

QString SubflowBlock::collapsePropertyKey() const
{
  switch (mRole)
  {
    case Role::Do:
      return QStringLiteral("doCollapsed");
    case Role::Else:
      return QStringLiteral("elseCollapsed");
    case Role::Loop:
    default:
      return QStringLiteral("loopCollapsed");
  }
}

void SubflowBlock::persistCollapsedState()
{
  if (mOwner)
    mOwner->setProperty(collapsePropertyKey(), mCollapsed);
}

void SubflowBlock::setContentsVisible(bool visible)
{
  // Logical children are independent scene items (not graphics children), so
  // their visibility — and that of any nested descendants — is toggled here.
  QVector<NodeItem*> stack = children();
  QVector<NodeItem*> descendants;
  while (!stack.isEmpty())
  {
    NodeItem* node = stack.takeLast();
    if (!node)
      continue;
    descendants.append(node);
    stack.append(node->children());
  }

  QSet<const NodeItem*> hidden;
  for (NodeItem* node : descendants)
  {
    node->setVisible(visible);
    hidden.insert(node);
  }

  if (!scene())
    return;

  // Transitions touching a hidden descendant would otherwise dangle in mid-air.
  const QList<QGraphicsItem*> items = scene()->items();
  for (QGraphicsItem* item : items)
  {
    if (!item || item->type() != Types::TRANSITION)
      continue;

    auto* transition = static_cast<TransitionItem*>(item);
    if (hidden.contains(transition->source()) || hidden.contains(transition->destination()))
      transition->setVisible(visible);
  }
}

void SubflowBlock::setCollapsed(bool collapsed)
{
  if (collapsed == mCollapsed)
    return;

  mCollapsed = collapsed;

  // The loop/out port only makes sense while the contents are visible.
  if (mOutPort)
    mOutPort->setVisible(!collapsed);

  if (collapsed)
  {
    setContentsVisible(false);

    mSuppressExpand = true;
    qreal left = pos().x();
    if (mOwner)
      left = mOwner->mapRectToScene(mOwner->nodeRect()).left();
    setBlockGeometry(QPointF(left, pos().y()), QSizeF(qMax(mSize.width(), kMinBlockWidth), kCollapsedHeight));
    mSuppressExpand = false;
  }
  else
  {
    setContentsVisible(true);
    // Regrow around the (now visible) children; also re-stacks any follower.
    expandToFitChildren();
  }

  persistCollapsedState();
  update();
}

void SubflowBlock::applyPersistedCollapsedState()
{
  if (!mOwner)
    return;

  const QVariant value = mOwner->getProperty(collapsePropertyKey());
  if (value.isValid() && value.toBool())
    setCollapsed(true);
}
