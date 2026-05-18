#include "node_base.h"

#include <QGraphicsColorizeEffect>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QTextDocument>
#include <QtGlobal>

#include "app_configs.h"
#include "app_paths.h"
#include "logging.h"
#include "node.h"

const qreal MAX_WIDTH = 80.0;
const qreal MAX_HEIGHT = 80.0;

void renderShapeSvg(QSvgRenderer& renderer, QPainter* painter, const QRectF& drawingBounds)
{
  QRectF viewBox = renderer.viewBoxF();
  if (!viewBox.isValid() || viewBox.isEmpty())
    viewBox = QRectF(0, 0, 1, 1);

  constexpr qreal padding = 2.0;
  QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
  const qreal sx = contentRect.width() / viewBox.width();
  const qreal sy = contentRect.height() / viewBox.height();
  const qreal scale = qMin(sx, sy);
  const QSizeF scaledSize(viewBox.width() * scale, viewBox.height() * scale);
  const QRectF targetRect(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0,
                          contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
                          scaledSize.width(), scaledSize.height());

  renderer.render(painter, targetRect);
}

NodeBase::NodeBase(const QString& id, const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mConfig(nodeConfig)
    , mId(id)
    , mBounds(0, 0, config()->body.width, config()->body.height)
    , mNodeId(nodeId)
{
  setZValue(config()->body.zIndex);

  qreal scaleFactor = computeScaleFactor();
  mScaledBounds = QRectF(0, 0, config()->body.width * scaleFactor, config()->body.height * scaleFactor);
}

NodeBase::~NodeBase()
{
  mNodeSvgRenderer.reset();
  if (mPixmapItem)
    delete mPixmapItem;
}

QString NodeBase::id() const
{
  return mId;
}

int NodeBase::type() const
{
  return Type;
}

QString NodeBase::nodeId() const
{
  return mNodeId;
}

VoidResult NodeBase::start()
{
  return VoidResult();
}

std::shared_ptr<NodeConfig> NodeBase::config() const
{
  return mConfig;
}

QRectF NodeBase::boundingRect() const
{
  return mBounds;
}

QRectF NodeBase::labelBoundingRect() const
{
  if (mLabel != nullptr)
    return mLabel->boundingRect();

  return QRectF();
}

QRectF NodeBase::itemRectIncludingLabel() const
{
  QRectF r = boundingRect();
  if (!mLabel || !mLabel->isVisible())
    return r;
  return r.united(mLabel->mapRectToParent(mLabel->boundingRect()));
}

qreal NodeBase::labelExtentBelowBody() const
{
  if (!mLabel || !mLabel->isVisible())
    return 0.0;
  const QRectF labelInParent = mLabel->mapRectToParent(mLabel->boundingRect());
  return qMax(0.0, labelInParent.bottom() - boundingRect().bottom());
}

QRectF NodeBase::scaledRect() const
{
  return mScaledBounds;
}

QRectF NodeBase::drawingRect(const QRectF& input) const
{
  return input.adjusted(2, 2, -2, -2);
}

void NodeBase::paintNode(const QRectF& bounds, const QColor& background, const QPen& text, QPainter* painter)
{
  painter->setPen(text);
  painter->setBrush(background);
  painter->setRenderHint(QPainter::Antialiasing, false);

  if (mLabel)
    mLabel->setDefaultTextColor(text.color());

  const auto drawingBounds = drawingRect(bounds);

  if (!config()->body.nodeSvg.isEmpty())
  {
    if (!mNodeSvgRenderer)
    {
      const QString path = AppPaths::icon(config()->body.nodeSvg);
      auto renderer = std::make_unique<QSvgRenderer>(path);
      if (renderer->isValid())
        mNodeSvgRenderer = std::move(renderer);
      else
        LOG_WARNING("nodeSvg not found or invalid: %s", qPrintable(config()->body.nodeSvg));
    }

    if (mNodeSvgRenderer)
    {
      painter->setPen(Qt::NoPen);
      painter->setBrush(Qt::NoBrush);
      painter->setRenderHint(QPainter::Antialiasing, true);
      renderShapeSvg(*mNodeSvgRenderer, painter, drawingBounds);
      painter->setRenderHint(QPainter::Antialiasing, false);
    }
    else
    {
      painter->drawRoundedRect(drawingBounds, config()->body.borderRadius, config()->body.borderRadius);
    }

    paintPixmap(painter);
    return;
  }

  if (config()->body.shape == Types::Shape::RECTANGLE)
  {
    painter->drawRect(drawingBounds);
  }
  else if (config()->body.shape == Types::Shape::ELLIPSE)
  {
    painter->drawEllipse(drawingBounds);
  }
  else if (config()->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(drawingBounds.center().x(), drawingBounds.top())     // Top
            << QPointF(drawingBounds.right(), drawingBounds.center().y())   // Right
            << QPointF(drawingBounds.center().x(), drawingBounds.bottom())  // Bottom
            << QPointF(drawingBounds.left(), drawingBounds.center().y());   // Left

    painter->drawPolygon(diamond);
  }

  else
  {
    painter->drawRoundedRect(drawingBounds, 5, 5);
  }

  paintPixmap(painter);
}

QPainterPath NodeBase::nodeShape(const QRectF& bounds) const
{
  QPainterPath path;
  if (!config()->body.nodeSvg.isEmpty())
  {
    path.addRect(bounds);
    return path;
  }

  if (config()->body.shape == Types::Shape::RECTANGLE)
  {
    path.addRect(bounds);
  }
  else if (config()->body.shape == Types::Shape::ELLIPSE)
  {
    path.addEllipse(bounds);
  }
  else if (config()->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(bounds.center().x(), bounds.top())     // Top
            << QPointF(bounds.right(), bounds.center().y())   // Right
            << QPointF(bounds.center().x(), bounds.bottom())  // Bottom
            << QPointF(bounds.left(), bounds.center().y());   // Left

    path.addPolygon(diamond);
  }
  else
  {
    path.addRoundedRect(bounds, config()->body.borderRadius, config()->body.borderRadius);  // 10 is the corner radius
  }

  return path;
}

void NodeBase::paintLabel(QPainter* painter, const QRectF& area) const
{
  if (!mLabel)
    return;

  painter->setPen(Config::FOREGROUND);
  painter->drawText(area, Qt::AlignCenter, mLabel->toPlainText());
}

void NodeBase::paintPixmap(QPainter* painter) const
{
  if (!mPixmapItem)
    return;

  QRectF rect = boundingRect();
  QPointF center = rect.center();
  QPointF topLeft = center - QPointF((qreal)mPixmapItem->pixmap().width() / 2, (qreal)mPixmapItem->pixmap().height() / 2);
  painter->drawPixmap(topLeft, mPixmapItem->pixmap());
}

void NodeBase::setLabel(const QString& name, qreal fontSize)
{
  mLabel = new QGraphicsTextItem(this);
  mLabel->setDefaultTextColor(Config::FOREGROUND);

  setLabelName(name);
  setLabelSize(fontSize, {(double)config()->body.width, (double)config()->body.height});

  updateLabelPosition();
}

void NodeBase::setLabelName(const QString& name)
{
  if (!mLabel)
    return;

  mLabel->setPlainText(name);
  setLabelSize(mLabel->font().pointSizeF(), boundingRect().size());
}

void NodeBase::setLabelSize(qreal fontSize, const QSizeF& boundingSize)
{
  if (!mLabel)
    return;

  // Set the base font size
  QFont font = mLabel->font();
  font.setPointSizeF(qMin(Fonts::MaxSize, fontSize));
  mLabel->setFont(font);

  // Labels can be a bit longer in the pipeline
  if (config()->libraryType == Types::LibraryTypes::PIPELINE)
    mLabel->setTextWidth(2 * boundingSize.width() - (boundingSize.width() * 0.2));
  else
    mLabel->setTextWidth(boundingSize.width() - (boundingSize.width() * 0.2));

  mLabel->document()->adjustSize();

  updateLabelPosition();
}

void NodeBase::updateLabelPosition()
{
  if (!mLabel)
    return;

  QRectF textBounds = mLabel->boundingRect();

  // Calculate centered position
  qreal x = boundingRect().center().x() - (textBounds.width() / 2);
  qreal y = boundingRect().bottom() + 2;  // type() == NodeItem::Type ? boundingRect().bottom() + 2 : boundingRect().center().y() - (textBounds.height() / 2);

  mLabel->setPos(x, y);
}

void NodeBase::toggleLabelVisibility()
{
  if (!mLabel)
    return;

  mLabel->setVisible(!mLabel->isVisible());
}

void NodeBase::setPixmap(const QPixmap& pixmap)
{
  mPixmapItem = new QGraphicsPixmapItem(pixmap);
}

void NodeBase::setIcon(const QString& path, const QColor& iconColor)
{
  mIconPath = path;
  mIconItem = new QGraphicsSvgItem(mIconPath, this);

  QRectF nodeRect = boundingRect();
  QRectF svgRect = mIconItem->boundingRect();

  constexpr qreal padding = 2.0;

  // available space inside the node
  QRectF contentRect = nodeRect.adjusted(padding, padding, -padding, -padding);

  // scale SVG to fit inside contentRect while keeping aspect ratio
  qreal sx = contentRect.width() / svgRect.width();
  qreal sy = contentRect.height() / svgRect.height();
  qreal scale = std::min(sx, sy);

  mIconItem->setScale(scale);

  // after scaling, compute displayed size
  QSizeF scaledSize(svgRect.width() * scale, svgRect.height() * scale);

  // centre inside contentRect
  qreal x = contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0;
  qreal y = contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0;

  mIconItem->setPos(x, y);

  // auto* effect = new QGraphicsColorizeEffect();
  // effect->setColor(iconColor);
  // effect->setStrength(1.0);

  // mIconItem->setGraphicsEffect(effect);
}

qreal NodeBase::computeScaleFactor() const
{
  qreal widthScale = (config()->body.width > MAX_WIDTH)
                         ? MAX_WIDTH / config()->body.width
                         : 1.0;

  qreal heightScale = (config()->body.height > MAX_HEIGHT)
                          ? MAX_HEIGHT / config()->body.height
                          : 1.0;

  return qMin(widthScale, heightScale);  // Use the smallest scale to maintain aspect ratio
}

QPixmap NodeBase::nodePixmap() const
{
  if (!config()->body.nodeSvg.isEmpty())
  {
    if (!mNodeSvgRenderer)
    {
      const QString path = AppPaths::icon(config()->body.nodeSvg);
      auto renderer = std::make_unique<QSvgRenderer>(path);
      if (renderer->isValid())
        mNodeSvgRenderer = std::move(renderer);
    }
  }

  if (mNodeSvgRenderer && mNodeSvgRenderer->isValid())
  {
    QRectF vb = mNodeSvgRenderer->viewBoxF();
    if (!vb.isValid() || vb.isEmpty())
      vb = QRectF(0, 0, 1, 1);
    QPixmap pixmap(vb.size().toSize());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    mNodeSvgRenderer->render(&painter, QRectF(QPointF(0, 0), vb.size()));
    return pixmap;
  }

  if (!mIconItem || !mIconItem->renderer() || !mIconItem->renderer()->isValid())
    return QPixmap();

  QPixmap pixmap(mIconItem->boundingRect().size().toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  mIconItem->renderer()->render(&painter, QRectF(QPointF(0, 0), mIconItem->boundingRect().size().toSize()));

  return pixmap;
}

QString NodeBase::nodeIcon() const
{
  if (!config()->body.nodeSvg.isEmpty())
    return config()->body.nodeSvg;

  if (mIconItem && !mIconPath.isEmpty())
    return mIconPath;

  return QString();
}
