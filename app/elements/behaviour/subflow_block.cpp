#include "elements/behaviour/subflow_block.h"

#include <QApplication>
#include <QFont>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSet>
#include <QStyleOptionGraphicsItem>
#include <QUuid>
#include <algorithm>
#include <cmath>

#include "app_configs.h"
#include "config.h"
#include "elements/port.h"
#include "elements/transition.h"
#include "keys.h"
#include "style_helpers.h"
#include "system/canvas.h"
#include "types.h"

namespace
{
constexpr qreal kInnerPadding = 24.0;
constexpr qreal kPortLaneWidth = 36.0;
/** Wide enough for "Loop [n] iterations every [n] ms" title controls. */
constexpr qreal kMinBlockWidth = 320.0;
constexpr qreal kMinBlockHeight = 100.0;
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

/** Infinity glyph shown in the Loop iterations field when the value is 0. */
const QChar kInfinityGlyph(0x221E);

QLineEdit* makeIntEdit(QWidget* parent, int width, const QString& tooltip)
{
  auto* edit = new QLineEdit(parent);
  edit->setFixedWidth(width);
  edit->setAlignment(Qt::AlignCenter);
  edit->setValidator(new QIntValidator(0, 2'000'000'000, edit));
  edit->setToolTip(tooltip);
  return edit;
}

QLineEdit* makeIterationsEdit(QWidget* parent, int width, const QString& tooltip)
{
  auto* edit = new QLineEdit(parent);
  edit->setFixedWidth(width);
  edit->setAlignment(Qt::AlignCenter);
  // Digits, or the infinity glyph / "inf" for indefinite loops (stored as 0).
  edit->setValidator(new QRegularExpressionValidator(
      QRegularExpression(QStringLiteral("^(\\d+|") + kInfinityGlyph + QStringLiteral("|[iI][nN][fF])?$")), edit));
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

void syncIterationsEditFromProperty(QLineEdit* edit, NodeItem* owner)
{
  if (!edit || !owner || edit->hasFocus())
    return;

  const QVariant value = owner->getProperty(kIterationsPropertyId);
  if (!value.isValid())
  {
    if (!edit->text().isEmpty())
      edit->setText(QString());
    return;
  }

  const int iterations = value.toInt();
  const QString text = iterations == 0 ? QString(kInfinityGlyph) : QString::number(iterations);
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

bool applyIterationsEditToProperty(QLineEdit* edit, NodeItem* owner)
{
  if (!edit || !owner)
    return false;

  const QString text = edit->text().trimmed();
  if (text == QString(kInfinityGlyph) || text.compare(QStringLiteral("inf"), Qt::CaseInsensitive) == 0)
  {
    owner->setProperty(kIterationsPropertyId, 0);
    edit->setText(QString(kInfinityGlyph));
    return true;
  }

  bool ok = false;
  const int value = text.toInt(&ok);
  if (!ok || value < 0)
    return false;

  owner->setProperty(kIterationsPropertyId, value);
  if (value == 0)
    edit->setText(QString(kInfinityGlyph));
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

qreal SubflowCollapseUi::labelCenterOffsetX()
{
  // Pull the centered label left by half of (gap + chevron) so the pair is balanced.
  return -0.5 * (kGapFromLabel + kButtonSize);
}

QRectF SubflowCollapseUi::arrowRect(const NodeBase& host)
{
  const QRectF text = host.labelTextTightRect();
  QPointF anchor;
  if (!text.isEmpty())
  {
    anchor = QPointF(text.right() + kGapFromLabel, text.center().y() - kButtonSize * 0.5);
  }
  else
  {
    const QRectF body = host.nodeRect();
    const qreal groupWidth = kButtonSize;
    anchor = QPointF(body.center().x() - groupWidth * 0.5, body.bottom() + kGapFromLabel);
  }
  return QRectF(anchor, QSizeF(kButtonSize, kButtonSize));
}

void SubflowCollapseUi::paintArrow(QPainter* painter, const QRectF& rect, bool collapsed)
{
  if (!painter || rect.isEmpty())
    return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(Config::FOREGROUND));

  QPolygonF triangle;
  if (collapsed)
  {
    // Right-pointing triangle: click to expand.
    const qreal inset = rect.height() * 0.15;
    triangle << QPointF(rect.left() + inset, rect.top())
             << QPointF(rect.left() + inset, rect.bottom())
             << QPointF(rect.right() - inset, rect.center().y());
  }
  else
  {
    // Down-pointing triangle: click to collapse.
    const qreal inset = rect.width() * 0.15;
    triangle << QPointF(rect.left(), rect.top() + inset)
             << QPointF(rect.right(), rect.top() + inset)
             << QPointF(rect.center().x(), rect.bottom() - inset);
  }

  painter->drawPolygon(triangle);
  painter->restore();
}

bool SubflowCollapseUi::readPersisted(const NodeItem* host)
{
  if (!host)
    return false;

  const QVariant unified = host->getProperty(QString::fromLatin1(kPropertyKey));
  if (unified.isValid())
    return unified.toBool();

  // Migrate older per-block flags.
  const QVariant loop = host->getProperty(QStringLiteral("loopCollapsed"));
  if (loop.isValid() && loop.toBool())
    return true;
  const QVariant doFlag = host->getProperty(QStringLiteral("doCollapsed"));
  if (doFlag.isValid() && doFlag.toBool())
    return true;
  const QVariant elseFlag = host->getProperty(QStringLiteral("elseCollapsed"));
  if (elseFlag.isValid() && elseFlag.toBool())
    return true;

  return false;
}

void SubflowCollapseUi::writePersisted(NodeItem* host, bool collapsed)
{
  if (host)
    host->setProperty(QString::fromLatin1(kPropertyKey), collapsed);
}

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
  config->hasOutPort = false;
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
  // Avoid DeviceCoordinateCache — it has interacted badly with subflow teardown/paint.
  setCacheMode(QGraphicsItem::NoCache);
  setZValue(owner ? owner->zValue() - 1 : -1);
  setLabelName(QString());
}

void SubflowBlock::setOwnerNode(NodeItem* owner)
{
  mOwner = owner;
  if (owner)
    setZValue(owner->zValue() - 1);
}

void SubflowBlock::destroyTitleUi()
{
  if (!mTitleProxy)
    return;

  if (mTimeoutEdit)
    mTimeoutEdit->blockSignals(true);
  if (mIterationsEdit)
    mIterationsEdit->blockSignals(true);
  if (mRateEdit)
    mRateEdit->blockSignals(true);

  if (QWidget* embedded = mTitleProxy->widget())
  {
    if (QWidget* focus = QApplication::focusWidget())
    {
      if (focus == embedded || embedded->isAncestorOf(focus))
        focus->clearFocus();
    }
    mTitleProxy->setWidget(nullptr);
    embedded->hide();
    delete embedded;
  }

  mTitleProxy->hide();
  delete mTitleProxy;
  mTitleProxy = nullptr;
  mTimeoutEdit = nullptr;
  mIterationsEdit = nullptr;
  mRateEdit = nullptr;
}

void SubflowBlock::prepareForDeletion()
{
  // boundingRect() spans the dashed connector up to mConnectorAbove / mOwner, so
  // dropping those links shrinks it. Without prepareGeometryChange() the scene
  // index would keep leaf entries for the old, larger rect and later dereference
  // this item after it is freed.
  prepareGeometryChange();

  destroyTitleUi();
  mStackFollower = nullptr;
  mConnectorAbove = nullptr;
  mOwner = nullptr;
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

void SubflowBlock::notifyEnclosingSubflow()
{
  // Nested Repeat/Within blocks are not logical children of the outer Loop/Do
  // block, so a size/position change here must explicitly ask the enclosing
  // subflow (the owner's parent) to grow/shrink around us.
  if (!mOwner)
    return;
  if (NodeItem* hostParent = mOwner->parentNode())
    hostParent->expandSubflowToFitChildren();
}

QVariant SubflowBlock::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange && mOwner)
  {
    // Constrain movement to the vertical axis: X stays locked under the owner
    // (left-aligned), and the block is never allowed above the owner (or above
    // its stacked predecessor for Within "else").
    QPointF newPos = value.toPointF();
    newPos.setX(alignedLeftUnderOwner(mSize.width()));

    qreal minTop = sceneBottomBelowItem(mOwner) + gapBelowPredecessor();
    if (mConnectorAbove && mConnectorAbove != mOwner)
      minTop = qMax(minTop, sceneBottomBelowItem(mConnectorAbove) + gapBelowPredecessor());
    newPos.setY(qMax(newPos.y(), minTop));

    return NodeItem::itemChange(change, QVariant(newPos));
  }

  if (change == QGraphicsItem::ItemPositionHasChanged && mSuppressExpand && !mCollapsed)
  {
    // Live-drag of a nested block: keep the enclosing subflow covering us.
    // (mSuppressExpand is set for the duration of a chrome drag.)
    const QVariant result = NodeItem::itemChange(change, value);
    notifyEnclosingSubflow();
    return result;
  }

  return NodeItem::itemChange(change, value);
}

void SubflowBlock::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (mCollapsed)
  {
    event->ignore();
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
  // Manual drag/extend does not go through expandToFitChildren, so notify the
  // enclosing subflow here so nested Loop blocks stay covered by their parent.
  notifyEnclosingSubflow();
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

  mIterationsEdit = makeIterationsEdit(container, 44, QStringLiteral("Number of iterations (∞ / 0 = indefinitely)"));

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
  syncIterationsEditFromProperty(mIterationsEdit, mOwner);
  syncEditFromProperty(mRateEdit, mOwner, kRatePropertyId);
}

void SubflowBlock::applyTimeoutToOwner()
{
  if (!applyEditToProperty(mTimeoutEdit, mOwner, kTimeoutPropertyId))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::applyIterationsToOwner()
{
  if (!applyIterationsEditToProperty(mIterationsEdit, mOwner))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::applyRateToOwner()
{
  if (!applyEditToProperty(mRateEdit, mOwner, kRatePropertyId))
    syncTitleFieldsFromOwner();
}

void SubflowBlock::initializeNodeSize()
{
  if (mSize.width() <= 0.0 || mSize.height() <= 0.0)
    mSize = QSizeF(kMinBlockWidth, kMinBlockHeight);
  mStorage->setSize(mSize);
}

void SubflowBlock::syncToOwnerPosition()
{
  if (!mOwner)
    return;

  mSuppressExpand = true;
  const QPointF newTopLeft(alignedLeftUnderOwner(mSize.width()),
                           sceneBottomBelowItem(mOwner) + gapBelowPredecessor());
  updatePosition(newTopLeft);
  mSuppressExpand = false;
  notifyStackFollower();
}

void SubflowBlock::syncBelow(NodeItem* above)
{
  if (!above || !mOwner)
    return;

  mSuppressExpand = true;
  const QPointF newTopLeft(alignedLeftUnderOwner(mSize.width()),
                           sceneBottomBelowItem(above) + gapBelowPredecessor());
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

  auto fitRect = [&](const QRectF& sceneRect) {
    right = qMax(right, sceneRect.right() + kInnerPadding + kPortLaneWidth);
    bottom = qMax(bottom, sceneRect.bottom() + kInnerPadding);
  };

  // Gather all descendant nodes so we can also enclose the subflow blocks owned
  // by nested Repeat / Within nodes (those blocks are separate scene items, not
  // logical children, so they would otherwise fall outside this border).
  QVector<NodeItem*> stack = children();
  QSet<const NodeItem*> descendants;
  while (!stack.isEmpty())
  {
    NodeItem* child = stack.takeLast();
    if (!child)
      continue;
    descendants.insert(child);
    fitRect(child->sceneBoundingRect());
    stack.append(child->children());
  }

  if (scene())
  {
    for (QGraphicsItem* item : scene()->items())
    {
      if (!item || item->type() != NodeItem::Type)
        continue;
      auto* node = static_cast<NodeItem*>(item);
      if (node == this || !node->isSubflowContainer())
        continue;
      if (descendants.contains(node->subflowHost()))
        fitRect(node->sceneBoundingRect());
    }
  }

  const qreal newWidth = qMax(kMinBlockWidth, right - left);
  const qreal newHeight = qMax(kMinBlockHeight, bottom - top);

  setBlockGeometry(QPointF(left, top), QSizeF(newWidth, newHeight));

  mSuppressExpand = false;

  // Auto-fit growth must also ripple outward to any enclosing subflow.
  notifyEnclosingSubflow();
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
  if (!mOwner)
    return;

  // Refitting during host teardown paints into a half-destroyed scene.
  if (auto* canvas = dynamic_cast<Canvas*>(scene()))
  {
    if (canvas->isBulkRemoving())
      return;
  }

  expandToFitChildren();
}

QPainterPath SubflowBlock::shape() const
{
  if (mCollapsed)
    return {};

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
  if (mCollapsed)
    return rect;

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
  if (mCollapsed)
    return;

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

  if (mCollapsed)
    return;

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
}

void SubflowBlock::paintTitle(QPainter* painter) const
{
  QFont font = boldTitleFont(painter->font());
  painter->setFont(font);
  painter->setPen(QPen(Config::FOREGROUND));

  const QRectF titleRect(kTitleLeft, kTitleTop, nodeRect().width() - 2.0 * kTitleLeft, font.pointSizeF() * 2.0);
  painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, titleTextForRole(mRole));
}

qreal SubflowBlock::gapBelowPredecessor() const
{
  return kGapBelowOwner;
}

qreal SubflowBlock::alignedLeftUnderOwner(qreal width) const
{
  Q_UNUSED(width);
  if (!mOwner)
    return pos().x();

  return mOwner->mapRectToScene(mOwner->nodeRect()).left();
}

void SubflowBlock::setContentsVisible(bool visible)
{
  // Logical children are independent scene items (not graphics children), so
  // their visibility — and that of any nested descendants — is toggled here.
  QVector<NodeItem*> stack = children();
  QSet<const NodeItem*> affected;
  QVector<NodeItem*> nodesToToggle;

  while (!stack.isEmpty())
  {
    NodeItem* node = stack.takeLast();
    if (!node || affected.contains(node))
      continue;
    affected.insert(node);
    nodesToToggle.append(node);
    stack.append(node->children());
  }

  for (NodeItem* node : nodesToToggle)
    node->setVisible(visible);

  if (!scene())
    return;

  // Nested Repeat/Within subflow blocks are owned scene items, not logical
  // children — hide/show them too, and recurse into their contents.
  const QList<QGraphicsItem*> items = scene()->items();
  for (QGraphicsItem* item : items)
  {
    if (!item || item->type() != NodeItem::Type)
      continue;

    auto* node = static_cast<NodeItem*>(item);
    if (node == this || !node->isSubflowContainer())
      continue;
    if (!affected.contains(node->subflowHost()))
      continue;

    auto* nested = static_cast<SubflowBlock*>(node);
    nested->setVisible(visible);
    affected.insert(nested);

    if (!visible)
    {
      // Force-hide nested contents regardless of the nested block's own collapse.
      nested->setContentsVisible(false);
    }
    else if (!nested->isCollapsed())
    {
      // Restore contents only when the nested block itself is expanded.
      nested->setContentsVisible(true);
    }
  }

  // Transitions touching a hidden descendant / nested block would otherwise dangle.
  for (QGraphicsItem* item : items)
  {
    if (!item || item->type() != Types::TRANSITION)
      continue;

    auto* transition = static_cast<TransitionItem*>(item);
    if (affected.contains(transition->source()) || affected.contains(transition->destination()))
      transition->setVisible(visible);
  }
}

void SubflowBlock::setCollapsed(bool collapsed)
{
  if (collapsed == mCollapsed)
    return;

  // Must precede every boundingRect()-affecting change below so the scene index
  // drops this item using its current rect.
  prepareGeometryChange();

  mCollapsed = collapsed;

  // Inline title widgets only belong to the expanded block chrome.
  if (mTitleProxy)
    mTitleProxy->setVisible(!collapsed);
  setFlag(QGraphicsItem::ItemIsMovable, !collapsed);

  if (collapsed)
  {
    setContentsVisible(false);
    setVisible(false);
  }
  else
  {
    setVisible(true);
    setContentsVisible(true);
    // Restore the normal dashed-connector gap under the host / predecessor.
    if (mConnectorAbove && mConnectorAbove != mOwner)
      syncBelow(mConnectorAbove);
    else
      syncToOwnerPosition();
    // Regrow around the (now visible) children; also re-stacks any follower.
    expandToFitChildren();
  }

  notifyEnclosingSubflow();
  update();
}
