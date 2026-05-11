#include "node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QSvgRenderer>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include <QUndoStack>
#include <QUuid>

#include "app_configs.h"
#include "app_paths.h"
#include "flow.h"
#include "port.h"
#include "logging.h"
#include "style_helpers.h"
#include "subtask_connector.h"
#include "system/structure_canvas.h"
#include "system/canvas.h"
#include "system/undo_commands/move_node.h"
#include "system/undo_commands/resize_node.h"
#include "theme.h"

namespace
{
constexpr qreal kTaskCornerRadius = 28.0;
constexpr qreal kTaskInnerPadding = 6.0;
constexpr qreal kTaskSlotDiameterFactor = 0.30;  // relative to min(width,height)
constexpr qreal kTaskSlotTopY = 0.30;
constexpr qreal kTaskSlotBottomY = 0.70;
constexpr qreal kTaskSlotLeftX = 0.30;
constexpr qreal kTaskSlotRightX = 0.70;
constexpr qreal kTaskAspectWidth = 324.0;
constexpr qreal kTaskAspectHeight = 300.0;
constexpr qreal kTaskAspectRatio = kTaskAspectWidth / kTaskAspectHeight;

/** Horizontal row of slot centers (count in [1,3]). Centers use full node width `size`. */
QVector<QPointF> taskSlotCentersHorizontalRow(const QSizeF& size, int count)
{
  QVector<QPointF> out;
  if (count <= 0)
    return out;

  const qreal w = size.width();
  const qreal h = size.height();
  const qreal slotD = qMin(w, h) * kTaskSlotDiameterFactor;
  constexpr qreal rowYMiddleNorm = 0.5;
  const qreal y = h * rowYMiddleNorm;

  if (count == 1)
  {
    out.append(QPointF(w * 0.5, y));
    return out;
  }

  constexpr qreal horizontalMargin = 16.0;
  const qreal leftX = horizontalMargin + slotD * 0.5;
  const qreal rightX = w - horizontalMargin - slotD * 0.5;
  const qreal span = rightX - leftX;
  if (count == 2)
  {
    const qreal anchorW = qMin(w, kTaskAspectWidth);
    const qreal colStep = (kTaskSlotRightX - kTaskSlotLeftX) * anchorW;
    const qreal cx0 = (w - colStep) * 0.5;
    out.append(QPointF(cx0, y));
    out.append(QPointF(cx0 + colStep, y));
    return out;
  }
  // count == 3
  const qreal step = span * 0.5;
  out.append(QPointF(leftX, y));
  out.append(QPointF(leftX + step, y));
  out.append(QPointF(rightX, y));
  return out;
}

/** Slot centers in node local coordinates (same basis as boundingRect: full node 0..w x 0..h). */
QVector<QPointF> taskSlotCenters(const QSizeF& size, int count)
{
  QVector<QPointF> out;
  if (count <= 0)
    return out;

  const qreal w = size.width();
  const qreal h = size.height();

  // Single horizontal row while fewer than 4 slots (incl. placeholder).
  if (count < 4)
    return taskSlotCentersHorizontalRow(size, count);

  // Two-row grid, row-major: fill the top row left-to-right, then the bottom row.
  // numCols = ceil(count / 2). Column pitch matches classic 2-column layout on anchorW; grid is centered in w.
  const qreal anchorW = qMin(w, kTaskAspectWidth);
  const qreal colStep = (kTaskSlotRightX - kTaskSlotLeftX) * anchorW;
  const qreal topY = kTaskSlotTopY * h;
  const qreal botY = kTaskSlotBottomY * h;

  const int numCols = (count + 1) / 2;
  const qreal totalGrid = (numCols > 1) ? static_cast<qreal>(numCols - 1) * colStep : 0.0;
  const qreal leftX = (numCols > 1) ? (w - totalGrid) * 0.5 : w * 0.5;
  out.reserve(count);
  for (int i = 0; i < count; ++i)
  {
    const int row = i / numCols;
    const int col = i % numCols;
    const qreal x = leftX + col * colStep;
    const qreal y = (row == 0) ? topY : botY;
    out.append(QPointF(x, y));
  }
  return out;
}

void renderSvgInEllipse(QPainter* painter, const QString& svgPath, const QPointF& center, qreal diameter)
{
  if (svgPath.isEmpty())
    return;
  QSvgRenderer renderer(svgPath);
  if (!renderer.isValid())
    return;
  QRectF viewBox = renderer.viewBoxF();
  if (!viewBox.isValid() || viewBox.isEmpty())
    viewBox = QRectF(0, 0, 1, 1);

  QRectF drawingBounds(center.x() - diameter * 0.5, center.y() - diameter * 0.5, diameter, diameter);
  constexpr qreal padding = 2.0;
  QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
  const qreal sx = contentRect.width() / viewBox.width();
  const qreal sy = contentRect.height() / viewBox.height();
  const qreal scale = qMin(sx, sy);
  const QSizeF scaledSize(viewBox.width() * scale, viewBox.height() * scale);
  const QRectF targetRect(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0,
                          contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
                          scaledSize.width(),
                          scaledSize.height());
  renderer.render(painter, targetRect);
}

QSizeF taskAspectSizeFromWidth(qreal width)
{
  return QSizeF(width, width / kTaskAspectRatio);
}
}  // namespace

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->getnodeId(), nodeConfig, parent)
    , mStorage(info)
    , mParentNode(nullptr)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->getScale() : 1.0)
    , mSize(mStorage->getSize())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  mStorage->setId(this->id());
  mStorage->setNodeId(this->nodeId());

  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Task"))
  {
    mSize = taskAspectSizeFromWidth(mSize.width());
    mStorage->setSize(mSize);
  }

  // Children are created by the canvas, so we must make sure that there is no trailing children information
  mStorage->clearChildren();

  for (const auto& property : config()->properties)
  {
    if (!mStorage->getproperties().contains(property.id))
      mStorage->addProperty(property.id, property.defaultValue);
  }

  for (const auto& event : config()->events)
  {
    bool found = false;
    for (const auto& flow : mStorage->getflows())
    {
      if (flow->getname() != event.name)
        continue;

      found = true;
      break;
    }

    if (found)
      continue;

    mStorage->addFlow(std::make_shared<FlowSaveInfo>(event));
  }

  // node svg replaces icon if set
  const bool structuralCapability =
      config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type != QStringLiteral("Task");
  if (config()->body.nodeSvg.isEmpty() && !mStorage->getIcon().isEmpty() && !structuralCapability)
    setIcon(mStorage->getIcon(), config()->body.iconColor);

  qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabel(getProperty("name").toString(), labelSize);

  if (config()->libraryType == Types::LibraryTypes::BEHAVIOUR)
  {
    if (config()->hasInPort)
      mInPort = new PortItem(PortItem::In, this);
    if (config()->hasOutPort)
      mOutPort = new PortItem(PortItem::Out, this);
    updatePortPositions();
  }

  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
  mLastPosition = pos();

  LOG_DEBUG("%s created at: (%f, %f) with size (%f, %f) and scale %f", qPrintable(id()), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale());
}

NodeItem::~NodeItem()
{
  destroySubtaskConnector();
}

int NodeItem::type() const
{
  return Type;
}

Types::LibraryTypes NodeItem::function() const
{
  return config()->libraryType;
}

QString NodeItem::nodeName() const
{
  auto name = getProperty("name");
  return name.toString();
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return nodeId();
}

qreal NodeItem::baseScale() const
{
  return mStorage->getScale();
}

VoidResult NodeItem::start()
{
  return NodeBase::start();
}

QRectF NodeItem::boundingRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  auto color = getProperty("color");
  auto background = color.isValid() ? QColor::fromString(color.toString()) : config()->body.backgroundColor;

  if (rendersAsInsetCapability())
  {
    Q_UNUSED(style);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
    painter->setPen(pen);
    painter->setBrush(QBrush(background));
    const QRectF r = boundingRect().adjusted(2, 2, -2, -2);
    painter->drawEllipse(r);
    if (!config()->body.iconPath.isEmpty())
    {
      const QString iconAbsPath = AppPaths::icon(config()->body.iconPath);
      renderSvgInEllipse(painter, iconAbsPath, r.center(), qMin(r.width(), r.height()));
    }
    return;
  }

  if (isTaskContainer())
  {
    Q_UNUSED(style);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.5 / baseScale());
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const QRectF bodyRect = boundingRect().adjusted(kTaskInnerPadding,
                                                    kTaskInnerPadding,
                                                    -kTaskInnerPadding,
                                                    -kTaskInnerPadding);
    painter->drawRoundedRect(bodyRect, kTaskCornerRadius, kTaskCornerRadius);

    const qreal bbW = boundingRect().width();
    const qreal bbH = boundingRect().height();
    const qreal slotDiameter = qMin(bbW, bbH) * kTaskSlotDiameterFactor;
    const qreal slotRadius = slotDiameter * 0.5;
    const int n = static_cast<int>(structuralCapabilityChildren().size());
    const QVector<QPointF> centers = taskSlotCenters(boundingRect().size(), n + 1);
    if (!centers.isEmpty())
    {
      const QPointF placeholderCenter = centers.last();
      if (mHoverPreviewActive && mHoverPreviewColor.isValid())
      {
        const QPen capPen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
        painter->setPen(capPen);
        painter->setBrush(QBrush(mHoverPreviewColor));
        painter->drawEllipse(placeholderCenter, slotRadius, slotRadius);
        renderSvgInEllipse(painter, mHoverPreviewIcon, placeholderCenter, slotDiameter);
      }
      else
      {
        QPen dashPen(Qt::black, 1.5 / baseScale(), Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(dashPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(placeholderCenter, slotRadius, slotRadius);
      }
    }
    return;
  }

  const QPen outlinePen =
      isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());
  const bool isStructuralCapability =
      function() == Types::LibraryTypes::STRUCTURAL && !isTaskContainer();

  NodeBase::paintNode(boundingRect(), background, outlinePen, painter);

  if (isStructuralCapability && !config()->body.iconPath.isEmpty())
  {
    const QRectF r = boundingRect().adjusted(2, 2, -2, -2);
    renderSvgInEllipse(painter, AppPaths::icon(config()->body.iconPath), r.center(), qMin(r.width(), r.height()));
  }
}

QPainterPath NodeItem::shape() const
{
  if (rendersAsInsetCapability())
  {
    QPainterPath path;
    path.addEllipse(boundingRect().adjusted(2, 2, -2, -2));
    return path;
  }
  return NodeBase::nodeShape(boundingRect());
}

QVector<PropertyConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QMap<QString, QVariant> NodeItem::properties() const
{
  return mStorage->getproperties();
}

QVector<std::shared_ptr<IProperty>> NodeItem::fields() const
{
  return mStorage->getfields();
}

QVector<std::shared_ptr<IFlow>> NodeItem::events() const
{
  return mStorage->getflows();
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

QVariant NodeItem::getProperty(const QString& key) const
{
  if (!mStorage)
    return QVariant();

  return mStorage->getProperty(key);
}

void NodeItem::setProperty(const QString& key, QVariant value)
{
  LOG_DEBUG("[%s] Setting property %s of node: %s", qPrintable(id()), qPrintable(key), qPrintable(nodeId()));
  if (!mStorage)
    return;

  // TODO(felaze): We should check for success here
  mStorage->addProperty(key, value);

  if (key == "name")
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

void NodeItem::setEvent(int index, const FlowConfig& event)
{
  // if (!mStorage)
  //   return;

  // if (index < mStorage->events.size())
  //   mStorage->events[index] = event;
  // else
  //   mStorage->events.push_back(event);
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(std::make_shared<PropertyInfo>(property)));

  return VoidResult();
}

VoidResult NodeItem::setField(const QString& key, std::shared_ptr<PropertyInfo> property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(property));

  return VoidResult();
}

PropertyInfo NodeItem::getField(const QString& key) const
{
  if (!mStorage)
    return PropertyInfo();

  return mStorage->getField(key);
}

void NodeItem::removeField(const QString& key)
{
  if (!mStorage)
    return;

  mStorage->removeField(key);
}

QVector<NodeItem*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addParent(NodeItem* parent)
{
  if (!parent)
    return;

  mParentNode = parent;
  mStorage->setParentId(parent->id());
  setZValue(parent->zValue() + 2);

  if (function() == Types::LibraryTypes::STRUCTURAL && parent->isTaskContainer())
  {
    // subtasks drags are redirected to the root task in mouseMoveEvent so the
    // whole tree moves as a unit.
    setFlag(ItemIsMovable, false);
    return;
  }

  fitInsideParent(20);
}

void NodeItem::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (!info)
    return;

  mStorage->addChild(info);
  mChildrenNodes.push_back(node);
}

void NodeItem::childRemoved(NodeItem* child)
{
  mStorage->removeChild(child->id());
  mChildrenNodes.removeAll(child);

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
  {
    if (child->isTaskContainer())
      layoutSubtasks();
    else
      relayoutCapabilitySlots();
  }
}

bool NodeItem::isTaskContainer() const
{
  return config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Task");
}

bool NodeItem::isStructuralSubtask() const
{
  return isTaskContainer() && mParentNode && mParentNode->isTaskContainer();
}

bool NodeItem::rendersAsInsetCapability() const
{
  return function() == Types::LibraryTypes::STRUCTURAL && mParentNode && mParentNode->isTaskContainer() && !isTaskContainer();
}

QVector<NodeItem*> NodeItem::structuralSubtaskChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (c->isTaskContainer())
      out.append(c);
  }
  return out;
}

QVector<NodeItem*> NodeItem::structuralCapabilityChildren() const
{
  QVector<NodeItem*> out;
  for (NodeItem* c : mChildrenNodes)
  {
    if (!c->isTaskContainer())
      out.append(c);
  }
  return out;
}

NodeItem* NodeItem::rootStructuralTask() const
{
  NodeItem* root = const_cast<NodeItem*>(this);
  NodeItem* parent = mParentNode;
  while (parent && parent->isTaskContainer())
  {
    root = parent;
    parent = parent->parentNode();
  }
  return root;
}

void NodeItem::layoutSubtasks()
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  QVector<NodeItem*> subs = structuralSubtaskChildren();
  constexpr qreal trunkDrop = 24.0;
  constexpr qreal rowGap = 48.0;
  constexpr qreal trunkMinInset = 28.0;
  constexpr qreal trunkWidthFraction = 0.30;
  constexpr qreal subtaskBranchLength = 86.0;

  const QRectF taskBodySceneRect = mapRectToScene(boundingRect());
  const qreal trunkX = taskBodySceneRect.left() + qMax(trunkMinInset, taskBodySceneRect.width() * trunkWidthFraction);
  const qreal baseY = taskBodySceneRect.bottom() + trunkDrop;
  const QSizeF subtaskTargetSize(boundingRect().width() * (2.0 / 3.0),
                                 boundingRect().height() * (2.0 / 3.0));

  for (int i = 0; i < subs.size(); ++i)
  {
    NodeItem* st = subs[i];
    st->applySize(subtaskTargetSize);
    const QSizeF sz = st->boundingRect().size();
    const qreal x = trunkX + subtaskBranchLength;
    const qreal y = baseY + i * (sz.height() + rowGap);
    st->updatePosition(QPointF(x, y));
  }

  syncSubtaskConnector();
}

void NodeItem::relayoutCapabilitySlots()
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  QVector<NodeItem*> caps = structuralCapabilityChildren();
  const int n = caps.size();

  constexpr qreal margin = 16.0;

  qreal W = mSize.width();
  qreal H = mSize.height();

  const int slotCount = n + 1;
  QVector<QPointF> centers = taskSlotCenters(QSizeF(W, H), slotCount);
  qreal slotDiam = qMin(W, H) * kTaskSlotDiameterFactor;
  qreal reqW = W;
  qreal reqH = H;
  if (!isStructuralSubtask())
  {
    reqW = qMax(static_cast<qreal>(config()->body.width), W);
    if (slotCount >= 4)
    {
      const int numCols = (slotCount + 1) / 2;
      const qreal anchorW = qMin(W, kTaskAspectWidth);
      const qreal colStep = (kTaskSlotRightX - kTaskSlotLeftX) * anchorW;
      const qreal totalGrid = (numCols > 1) ? static_cast<qreal>(numCols - 1) * colStep : 0.0;
      reqW = qMax(reqW, totalGrid + slotDiam + 2.0 * margin);
    }
    else
    {
      for (const QPointF& c : centers)
        reqW = qMax(reqW, c.x() + slotDiam / 2.0 + margin);
    }
    reqH = qMax(static_cast<qreal>(config()->body.height), H);
  }

  if (!isStructuralSubtask() && (reqW > W + 0.5 || reqH > H + 0.5))
  {
    applySize(QSizeF(reqW, reqH));
    W = mSize.width();
    H = mSize.height();
    centers = taskSlotCenters(QSizeF(W, H), slotCount);
    slotDiam = qMin(W, H) * kTaskSlotDiameterFactor;
  }

  for (int i = 0; i < n; ++i)
  {
    const QPointF centerLocal = centers[i];
    const QPointF centerScene = mapToScene(centerLocal);
    NodeItem* cap = caps[i];
    cap->applySize(QSizeF(slotDiam, slotDiam));
    const QSizeF cs = cap->boundingRect().size();
    cap->updatePosition(centerScene - QPointF(cs.width() / 2.0, cs.height() / 2.0));
  }

  syncSubtaskConnector();
}

void NodeItem::ensureSubtaskConnector(StructureCanvas* canvas)
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL || !canvas)
    return;

  if (mSubtaskConnector)
  {
    mSubtaskConnector->syncGeometry();
    return;
  }

  QGraphicsScene* sc = scene();
  if (!sc)
    return;

  mSubtaskConnector = new SubtaskConnector(this, canvas);
  sc->addItem(mSubtaskConnector);
  mSubtaskConnector->syncGeometry();
}

void NodeItem::destroySubtaskConnector()
{
  if (!mSubtaskConnector)
    return;

  if (mSubtaskConnector->scene())
    mSubtaskConnector->scene()->removeItem(mSubtaskConnector);

  delete mSubtaskConnector;
  mSubtaskConnector = nullptr;
}

void NodeItem::syncSubtaskConnector()
{
  if (mSubtaskConnector)
    mSubtaskConnector->syncGeometry();
}

NodeItem* NodeItem::parentNode() const
{
  return mParentNode;
}

QString NodeItem::behaviour() const
{
  return config()->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return config()->help;
}

QRectF NodeItem::parentInnerSceneRect(qreal padding) const
{
  if (!parentNode())
    return {};

  QRectF r = parentNode()->mapRectToScene(parentNode()->boundingRect());
  return r.adjusted(padding, padding, -padding, -padding);
}

// Apply a new logical size to this node in one place
void NodeItem::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);

  // Same scale logic as before
  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);

  setLabelSize(newFontSize, mSize);
  updatePortPositions();
  update();

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void NodeItem::setHoverPreview(const QString& iconPath, const QColor& color, bool active)
{
  if (!isTaskContainer() || function() != Types::LibraryTypes::STRUCTURAL)
    return;

  mHoverPreviewActive = active;
  if (active)
  {
    mHoverPreviewIcon = iconPath;
    mHoverPreviewColor = color;
  }
  else
  {
    mHoverPreviewIcon.clear();
    mHoverPreviewColor = QColor();
  }
  update();
}

// Clamp this node's position so its scene rect stays inside `inner`
QPointF NodeItem::clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const
{
  QPointF posScene = pos();
  QPointF offset = childSceneRect.topLeft() - posScene;  // pos() → top-left

  QPointF newPos = posScene;

  const qreal minX = inner.left() - offset.x();
  const qreal maxX = inner.right() - (childSceneRect.width() + offset.x());
  const qreal minY = inner.top() - offset.y();
  const qreal maxY = inner.bottom() - (childSceneRect.height() + offset.y());

  newPos.setX(std::clamp(newPos.x(), minX, maxX));
  newPos.setY(std::clamp(newPos.y(), minY, maxY));

  return newPos;
}

QSizeF NodeItem::clampSize(qreal width, qreal height) const
{
  if (isStructuralSubtask())
    return QSizeF(width, height);

  if (!parentNode())
    return QSizeF(width, height);

  QRectF inner = parentInnerSceneRect(10);
  if (!inner.isValid())
    return QSizeF(width, height);

  // child pos
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF currentTopLeft = childSceneRect.topLeft();

  // Max width/height keeping bottom-right within inner
  qreal maxWidth = inner.right() - currentTopLeft.x();
  qreal maxHeight = inner.bottom() - currentTopLeft.y();

  qreal clampedW = qMin(width, qMax<qreal>(0.0, maxWidth));
  qreal clampedH = qMin(height, qMax<qreal>(0.0, maxHeight));

  return QSizeF(clampedW, clampedH);
}

void NodeItem::fitInsideParent(qreal padding)
{
  if (isStructuralSubtask())
    return;

  if (rendersAsInsetCapability())
    return;

  QRectF inner = parentInnerSceneRect(padding);
  if (!inner.isValid())
    return;

  // 1) Clamp size so we're not bigger than the inner rect
  QSizeF currentSize = mSize;
  qreal maxW = inner.width();
  qreal maxH = inner.height();

  maxW = qMax(maxW, Config::MINIMUM_NODE_SIZE);
  maxH = qMax(maxH, Config::MINIMUM_NODE_SIZE);

  qreal newW = qMin(currentSize.width(), maxW);
  qreal newH = qMin(currentSize.height(), maxH);

  if (newW != currentSize.width() || newH != currentSize.height())
    applySize(QSizeF(newW, newH));

  // 2) Clamp position so we're fully inside `inner`
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF newPos = clampPosInside(inner, childSceneRect);
  updatePosition(newPos);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing && (event->modifiers() & Qt::ShiftModifier))
  {
    QPointF delta = event->pos() - mResizeStartMousePos;

    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.width() + delta.x());
    qreal newHeight = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.height() + delta.y());

    QSizeF clampedSize = clampSize(newWidth, newHeight);

    applySize(clampedSize);

    if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && isTaskContainer())
    {
      relayoutCapabilitySlots();
      layoutSubtasks();
    }
    else
    {
      for (auto* child : children())
      {
        auto* nodeChild = static_cast<NodeItem*>(child);
        nodeChild->fitInsideParent(10);
      }
    }
  }
  else
  {
    if (isStructuralSubtask())
    {
      NodeItem* root = rootStructuralTask();
      if (root && root != this)
      {
        const QPointF delta = event->scenePos() - mTreeDragStartScenePos;
        root->updatePosition(mTreeDragRootStartPos + delta);
      }
      event->accept();
      return;
    }

    QGraphicsItem::mouseMoveEvent(event);
  }
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && event->modifiers() & Qt::ShiftModifier)
  {
    mIsResizing = true;
    mResizeStartMousePos = event->pos();
    mResizeStartSize = mSize;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::SizeFDiagCursor);
    event->accept();
  }
  else
  {
    mDragStartPos = pos();
    if (isStructuralSubtask())
    {
      NodeItem* root = rootStructuralTask();
      mTreeDragRootStartPos = root ? root->pos() : QPointF();
      mTreeDragStartScenePos = event->scenePos();
    }
    QGraphicsItem::mousePressEvent(event);
  }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing)
  {
    mIsResizing = false;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::ArrowCursor);

    auto canvas = static_cast<Canvas*>(scene());
    if (canvas)
      canvas->undoStack()->push(new ResizeNodeCommand(canvas, id(), mResizeStartSize, mSize));
  }
  else if (isStructuralSubtask())
  {
    auto canvas = static_cast<Canvas*>(scene());
    NodeItem* root = rootStructuralTask();
    if (canvas && root && root->pos() != mTreeDragRootStartPos)
      canvas->undoStack()->push(new MoveNodeCommand(canvas, root->id(), mTreeDragRootStartPos, root->pos()));
  }
  else if (pos() != mDragStartPos)
  {
    auto canvas = static_cast<Canvas*>(scene());
    if (canvas)
      canvas->undoStack()->push(new MoveNodeCommand(canvas, id(), mDragStartPos, pos()));
  }

  QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    if (NodeItem* parent = parentNode())
    {
      // Structural subtasks are not clamped inside the parent rect: they live
      // below the parent task. The user-drag redirect to the root is handled
      // explicitly in mouseMoveEvent so this path must remain a no-op for them.
      if (isStructuralSubtask())
        return QGraphicsItem::itemChange(change, value);

      QPointF newPos = value.toPointF();  // proposed new pos in scene coords

      // auto parent = parentNode();
      // if (parent == nullptr)
      //   return QGraphicsItem::itemChange(change, value);

      QRectF parentRect = parent->boundingRect();
      parentRect = parentRect.adjusted(10, 10, -10, -10);
      parentRect.translate(parent->pos());

      // Child rect in its own coords
      QRectF childLocalRect = boundingRect();

      // Compute allowed range so childSceneRect stays inside parentRect
      const qreal minX = parentRect.left();
      const qreal maxX = parentRect.right() - childLocalRect.width();
      const qreal minY = parentRect.top();
      const qreal maxY = parentRect.bottom() - childLocalRect.height();

      // Clamp
      newPos.setX(std::clamp(newPos.x(), minX, maxX));
      newPos.setY(std::clamp(newPos.y(), minY, maxY));

      return newPos;  // this replaces the proposed position
    }
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    updatePosition(value.toPointF());
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& newPosition)
{
  prepareGeometryChange();
  setPos(newPosition);

  QPointF delta = newPosition - mLastPosition;
  for (auto* child : children())
  {
    auto childNode = static_cast<NodeItem*>(child);
    childNode->updatePosition(childNode->pos() + delta);
  }

  mLastPosition = newPosition;

  updateExtrasPosition();
  mStorage->setPosition(pos() + boundingRect().center());

  if (isTaskContainer() && function() == Types::LibraryTypes::STRUCTURAL)
    syncSubtaskConnector();
}

void NodeItem::updateExtrasPosition()
{
  updatePortPositions();
  for (auto& transition : transitions())
    transition->updatePath();

  updateLabelPosition();
}

void NodeItem::updatePortPositions()
{
  const qreal w = boundingRect().width();
  const qreal h = boundingRect().height();
  if (mInPort)
    mInPort->setPos(-PortItem::kSize - PortItem::kGap, (h - PortItem::kSize) / 2.0);
  if (mOutPort)
    mOutPort->setPos(w + PortItem::kGap, (h - PortItem::kSize) / 2.0);
}

// Slots
void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

NodeSaveInfo NodeItem::saveInfo() const
{
  return *mStorage;
}

QVector<TransitionItem*> NodeItem::transitions() const
{
  return mTransitions;
}

void NodeItem::addTransition(TransitionItem* transition)
{
  // Make sure the source node holds the transition info
  if (transition->destination() && (id() != transition->destination()->id()))
  {
    bool found = false;
    for (const auto& t : mStorage->gettransitions())
    {
      if (t->getid() == transition->id())
      {
        found = true;
        break;
      }
    }

    if (!found)
      mStorage->addTransition(transition->storage());

    for (auto& t : transitions())
    {
      // If I am the source of this transition
      // Check whether we have another transition with me as destination
      auto src1 = transition->source()->id();
      auto dst1 = transition->destination()->id();
      auto src2 = t->source()->id();
      auto dst2 = t->destination()->id();
      if (((src1 == dst2) && (src2 == dst1)))
      {
        transition->setEdge(TransitionItem::Edge::FORWARD);
        t->setEdge(TransitionItem::Edge::BACKWARD);
      }
    }
  }

  bool found = false;
  for (const auto& t : transitions())
  {
    if (t->id() == transition->id())
    {
      found = true;
      break;
    }
  }

  if (!found)
    mTransitions.push_back(transition);

  prepareGeometryChange();
  updateExtrasPosition();
}

void NodeItem::removeTransition(TransitionItem* transition)
{
  mStorage->removeTransition(transition->storage());
  mTransitions.removeIf([transition](TransitionItem* item) {
    return item->id() == transition->id();
  });
}

QPointF NodeItem::edgePointToward(const QPointF& targetScenePos, bool fromOutgoingPort) const
{
  if (fromOutgoingPort && mOutPort)
    return mOutPort->anchorScenePos();
  if (!fromOutgoingPort && mInPort)
    return mInPort->anchorScenePos();

  QPointF center = sceneBoundingRect().center();
  QPointF dir = targetScenePos - center;

  if (dir.manhattanLength() < 0.001)
    return center;  // avoid divide by zero

  // Normalise and scale
  dir /= std::hypot(dir.x(), dir.y());
  qreal radius = boundingRect().width() / 2.0;
  return center + dir * radius;
}

bool NodeItem::canAddTransition() const
{
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  return config()->transitions.isEmpty() || index < config()->transitions.size();
}

TransitionConfig NodeItem::nextTransition() const
{
  // Only count the transitions coming from this
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  if (config()->transitions.isEmpty() || index >= config()->transitions.size())
    return TransitionConfig();

  return config()->transitions.at(index);
}

QVector<TransitionConfig> NodeItem::configTransitions() const
{
  return config()->transitions;
}

QVector<Flow*> NodeItem::flows() const
{
  return mFlows;
}

Flow* NodeItem::createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info)
{
  std::shared_ptr<FlowSaveInfo> flowConfig = info;
  bool found = false;
  if (info != nullptr)
  {
    // Clean up
    for (const auto& f : mStorage->getflows())
    {
      if (f->getid() != info->getid())
        continue;

      flowConfig = std::dynamic_pointer_cast<FlowSaveInfo>(f);
      found = true;
      break;
    }
  }
  else
  {
    flowConfig = std::make_shared<FlowSaveInfo>();
  }

  if (!found)
  {
    // Set this as owner of the flow
    flowConfig->setOwner(id());
    mStorage->addFlow(flowConfig);
  }

  Flow* flow = new Flow(flowName, flowConfig);
  mFlows.push_back(flow);

  if (flowAdded)
    flowAdded(flow, this);

  return flow;
}

Flow* NodeItem::getFlow(const QString& flowId) const
{
  for (const auto& flow : mFlows)
  {
    if (flow->id() == flowId)
      return flow;
  }

  return nullptr;
}

void NodeItem::deleteFlow(const QString& flowId)
{
  mStorage->removeFlow(flowId);
  mFlows.removeIf([flowId](Flow* flow) { return flow->id() == flowId; });
}

void NodeItem::updateFlow()
{
}
