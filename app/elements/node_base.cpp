#include "node_base.h"

#include <QGraphicsColorizeEffect>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QTextDocument>
#include <QtGlobal>

#include "app_configs.h"
#include "logging.h"
#include "node.h"

const qreal MAX_WIDTH = 60.0;
const qreal MAX_HEIGHT = 60.0;
const qreal LABEL_H_SPACING = 8;
const qreal LABEL_V_SPACING = 2;

NodeBase::NodeBase(const QString& id, const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mConfig(nodeConfig)
    , mRenderer(nullptr)
    , mId(id)
    , mBounds(0, 0, config()->body.width, config()->body.height)
{
  setZValue(config()->body.zIndex);

  qreal scaleFactor = computeScaleFactor();
  mScaledBounds = QRectF(0, 0, config()->body.width * scaleFactor, config()->body.height * scaleFactor);

  if (!config()->body.iconPath.isEmpty())
    if (auto icon = iconPathFromTheme(config()->body.iconPath); !icon.isEmpty())
      mRenderer = new QSvgRenderer(icon);
}

NodeBase::~NodeBase()
{
  if (mRenderer)
    delete mRenderer;
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
  return config()->type;
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
  return nodeRect().united(labelBoundingRect());
}

QRectF NodeBase::nodeRect() const
{
  return mBounds;
}

QRectF NodeBase::labelBoundingRect() const
{
  if (mLabelText.isEmpty())
    return QRectF();

  const auto bounds = drawingRect(nodeRect());
  return QRectF(bounds.left() - LABEL_H_SPACING, bounds.bottom() + LABEL_V_SPACING, bounds.width() + 2 * LABEL_H_SPACING, mLabelFont.pointSizeF() * 3);
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

  const auto drawingBounds = drawingRect(bounds);
  if (mRenderer)
  {
    auto rect = shapeSvgTargetRect(mRenderer, drawingBounds);
    paintSvg(mRenderer, iconPathFromTheme(config()->body.iconPath), painter, rect.center(), rect.width(), rect.height());
  }
  else if (config()->body.shape == Types::Shape::RECTANGLE)
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

  paintLabel(painter, drawingBounds, text);
  if (isSelected())
    paintSelectionOutline(painter, text, bounds);
}

void NodeBase::paintSelectionOutline(QPainter* painter, const QPen& text, const QRectF& bounds) const
{
  const QPainterPath outline = nodeShape(bounds);
  if (outline.isEmpty())
    return;

  painter->save();
  painter->setPen(text);
  painter->setBrush(Qt::NoBrush);
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->drawPath(outline);
  painter->restore();
}

QRectF NodeBase::shapeSvgTargetRect(const QSvgRenderer* renderer, const QRectF& drawingBounds) const
{
  QRectF viewBox = renderer->viewBoxF();
  if (!viewBox.isValid() || viewBox.isEmpty())
    viewBox = QRectF(0, 0, 1, 1);

  constexpr qreal padding = 2.0;
  QRectF contentRect = drawingBounds.adjusted(padding, padding, -padding, -padding);
  const qreal sx = contentRect.width() / viewBox.width();
  const qreal sy = contentRect.height() / viewBox.height();
  const qreal scale = qMin(sx, sy);
  const QSizeF scaledSize(viewBox.width() * scale, viewBox.height() * scale);
  return QRectF(contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0, contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0,
                scaledSize.width(), scaledSize.height());
}

bool pixelOpaque(const QImage& image, int x, int y, int threshold)
{
  if (x < 0 || x >= image.width() || y < 0 || y >= image.height())
    return false;
  return qAlpha(image.pixel(x, y)) > threshold;
}

bool pixelOnSilhouetteEdge(const QImage& image, int x, int y, int threshold)
{
  if (!pixelOpaque(image, x, y, threshold))
    return false;

  static constexpr int kDx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  static constexpr int kDy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
  for (int i = 0; i < 8; ++i)
    if (!pixelOpaque(image, x + kDx[i], y + kDy[i], threshold))
      return true;
  return false;
}

QVector<QPoint> traceOuterSilhouetteContour(const QImage& image, int threshold)
{
  QPoint start(-1, -1);
  for (int y = 0; y < image.height() && start.x() < 0; ++y)
  {
    for (int x = 0; x < image.width(); ++x)
    {
      if (pixelOnSilhouetteEdge(image, x, y, threshold))
      {
        start = QPoint(x, y);
        break;
      }
    }
  }

  if (start.x() < 0)
    return {};

  static constexpr int kDx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  static constexpr int kDy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

  QVector<QPoint> contour;
  int x = start.x();
  int y = start.y();
  int dir = 0;
  const int maxSteps = image.width() * image.height() * 4;

  for (int step = 0; step < maxSteps; ++step)
  {
    contour.append(QPoint(x, y));

    bool foundNext = false;
    const int searchStart = (dir + 5) % 8;
    for (int i = 0; i < 8; ++i)
    {
      const int nd = (searchStart + i) % 8;
      const int nx = x + kDx[nd];
      const int ny = y + kDy[nd];
      if (pixelOnSilhouetteEdge(image, nx, ny, threshold))
      {
        x = nx;
        y = ny;
        dir = nd;
        foundNext = true;
        break;
      }
    }

    if (!foundNext)
      break;

    if (contour.size() > 2 && x == start.x() && y == start.y())
      break;
  }

  return contour;
}

QPainterPath contourToPath(const QVector<QPoint>& contour, const QRectF& targetRect, const QSize& imageSize)
{
  if (contour.size() < 3 || imageSize.width() <= 0 || imageSize.height() <= 0)
    return {};

  const qreal scaleX = targetRect.width() / imageSize.width();
  const qreal scaleY = targetRect.height() / imageSize.height();
  const int stride = qMax(1, contour.size() / 96);

  QPainterPath path;
  path.moveTo(targetRect.x() + (contour.first().x() + 0.5) * scaleX, targetRect.y() + (contour.first().y() + 0.5) * scaleY);
  for (int i = stride; i < contour.size(); i += stride)
    path.lineTo(targetRect.x() + (contour.at(i).x() + 0.5) * scaleX, targetRect.y() + (contour.at(i).y() + 0.5) * scaleY);
  path.closeSubpath();
  return path.simplified();
}

QPainterPath NodeBase::svgSilhouetteOutlinePath(QSvgRenderer* renderer, const QRectF& drawingBounds) const
{
  const QRectF target = shapeSvgTargetRect(renderer, drawingBounds);
  const int pixelWidth = qMax(12, qRound(target.width() * 3));
  const int pixelHeight = qMax(12, qRound(target.height() * 3));
  const QSize pixelSize(pixelWidth, pixelHeight);

  if (mSvgOutlineCacheKey == config()->body.iconPath && mSvgOutlineCacheTarget == target && !mSvgOutlineCachePath.isEmpty())
    return mSvgOutlineCachePath;

  QImage image(pixelSize.width(), pixelSize.height(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  {
    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);
    renderer->render(&imagePainter, QRectF(QPointF(0, 0), pixelSize));
  }

  const QVector<QPoint> contour = traceOuterSilhouetteContour(image, 48);
  QPainterPath outline = contourToPath(contour, target, pixelSize);
  if (outline.isEmpty())
    outline = geometricBodyOutlinePath(drawingBounds);

  // Hopefully, we only need to run this once per node
  // It would be even better if we had a global storage for these...
  mSvgOutlineCacheKey = config()->body.iconPath;
  mSvgOutlineCachePath = outline;
  mSvgOutlineCacheTarget = target;
  return outline;
}

QPainterPath NodeBase::geometricBodyOutlinePath(const QRectF& drawingBounds) const
{
  QPainterPath path;
  if (config()->body.shape == Types::Shape::RECTANGLE)
  {
    path.addRect(drawingBounds);
  }
  else if (config()->body.shape == Types::Shape::ELLIPSE)
  {
    path.addEllipse(drawingBounds);
  }
  else if (config()->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(drawingBounds.center().x(), drawingBounds.top()) << QPointF(drawingBounds.right(), drawingBounds.center().y())
            << QPointF(drawingBounds.center().x(), drawingBounds.bottom()) << QPointF(drawingBounds.left(), drawingBounds.center().y());
    path.addPolygon(diamond);
  }
  else
  {
    const qreal radius = config()->body.borderRadius > 0 ? config()->body.borderRadius : 5.0;
    path.addRoundedRect(drawingBounds, radius, radius);
  }
  return path;
}

QPainterPath NodeBase::nodeShape(const QRectF& bounds) const
{
  if (mRenderer && config()->libraryType != Types::LibraryTypes::STRUCTURAL)
    return svgSilhouetteOutlinePath(mRenderer, drawingRect(bounds));

  return geometricBodyOutlinePath(drawingRect(bounds));
}

void NodeBase::paintLabel(QPainter* painter, const QRectF& drawingBounds, const QPen& pen, bool elide) const
{
  if (mLabelText.isEmpty() || !mPaintLabel)
    return;

  painter->setFont(mLabelFont);
  painter->setPen(pen);

  QString text = mLabelText;
  QTextOption textOption;
  textOption.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  if (!elide)
  {
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  }
  else
  {
    const QFontMetricsF metrics(mLabelFont);
    const qreal availableWidth = labelBoundingRect().width();
    text = metrics.elidedText(mLabelText, Qt::ElideRight, static_cast<int>(availableWidth));
  }
  painter->drawText(labelBoundingRect(), text, textOption);
}

void NodeBase::initializeNodeSize()
{
}

void NodeBase::setLabel(const QString& name, qreal fontSize)
{
  setLabelName(name);

  mLabelFont.setPointSizeF(qMin(Fonts::MaxSize, fontSize));
  update();
}

void NodeBase::setLabelName(const QString& name)
{
  mLabelText = name;
  update();
}

void NodeBase::setLabelSize(qreal fontSize, const QSizeF& boundingSize)
{
  mLabelFont.setPointSizeF(qMin(Fonts::MaxSize, fontSize));
  update();
}

void NodeBase::toggleLabelVisibility()
{
  mPaintLabel = !mPaintLabel;
  update();
}

void NodeBase::settingsChanged(const AppearanceSettings& appearance)
{
}

void NodeBase::setIcon(const QString& path, const QColor& iconColor)
{
  // mIconPath = path;
  // mIconItem = new QGraphicsSvgItem(mIconPath, this);

  // QRectF nodeRect = boundingRect();
  // QRectF svgRect = mIconItem->boundingRect();

  // constexpr qreal padding = 2.0;

  // // available space inside the node
  // QRectF contentRect = nodeRect.adjusted(padding, padding, -padding, -padding);

  // // scale SVG to fit inside contentRect while keeping aspect ratio
  // qreal sx = contentRect.width() / svgRect.width();
  // qreal sy = contentRect.height() / svgRect.height();
  // qreal scale = std::min(sx, sy);

  // mIconItem->setScale(scale);

  // // after scaling, compute displayed size
  // QSizeF scaledSize(svgRect.width() * scale, svgRect.height() * scale);

  // // centre inside contentRect
  // qreal x = contentRect.x() + (contentRect.width() - scaledSize.width()) / 2.0;
  // qreal y = contentRect.y() + (contentRect.height() - scaledSize.height()) / 2.0;

  // mIconItem->setPos(x, y);

  // auto* effect = new QGraphicsColorizeEffect();
  // effect->setColor(iconColor);
  // effect->setStrength(1.0);

  // mIconItem->setGraphicsEffect(effect);
}

qreal NodeBase::computeScaleFactor() const
{
  qreal widthScale = (config()->body.width > MAX_WIDTH) ? MAX_WIDTH / config()->body.width : 1.0;

  qreal heightScale = (config()->body.height > MAX_HEIGHT) ? MAX_HEIGHT / config()->body.height : 1.0;

  return qMin(widthScale, heightScale);  // Use the smallest scale to maintain aspect ratio
}

QPixmap NodeBase::nodePixmap() const
{
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
  if (mIconItem && !mIconPath.isEmpty())
    return mIconPath;

  return QString();
}
