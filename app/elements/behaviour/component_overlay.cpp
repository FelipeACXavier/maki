#include "elements/behaviour/component_overlay.h"

#include <QFileInfo>
#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPolygonF>
#include <QSvgRenderer>

#include "app_configs.h"
#include "app_paths.h"
#include "elements/behaviour/call_capability.h"
#include "elements/draggable.h"
#include "keys.h"
#include "save_info.h"
#include "style_helpers.h"
#include "system/canvas.h"
#include "system/config_table.h"
#include "types.h"

namespace behaviour
{
namespace
{
const QString kTaskFallbackIcon = QStringLiteral("node_task.svg");

bool isTaskTypeKey(const QString& nodeTypeKey, const NodeConfig* cfg)
{
  if (nodeTypeKey.endsWith(QStringLiteral("::Task")) || nodeTypeKey == QStringLiteral("Task"))
    return true;
  return cfg && cfg->type == QStringLiteral("Koda::Task");
}

bool isTaskCaller(const NodeSaveInfo& caller, const ConfigurationTable* configTable)
{
  const auto cfg = configTable ? configTable->get(caller.getnodeId()) : nullptr;
  return isTaskTypeKey(caller.getnodeId(), cfg.get());
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

std::shared_ptr<NodeSaveInfo> selectedComponentCaller(const NodeItem* node, const SaveInfo& storage)
{
  if (call_capability::isCallNodeType(node->nodeType()) || call_capability::isWaitNodeType(node->nodeType()))
  {
    const QString capabilityId = call_capability::resolveCapabilityId(*node, storage);
    if (capabilityId.isEmpty())
      return nullptr;
    for (const auto& caller : storage.getPossibleCallers(node->id(), Types::PropertyTypes::EVENT_SELECT))
    {
      if (caller && caller->getid() == capabilityId)
        return caller;
    }
    return nullptr;
  }

  QString propertyId;
  for (const auto& property : node->config()->properties)
  {
    if (property.type == Types::PropertyTypes::COMPONENT_SELECT)
    {
      propertyId = property.id;
      break;
    }
  }

  if (propertyId.isEmpty())
    return nullptr;

  const QVariant propertyValue = node->getProperty(propertyId);
  if (!propertyValue.isValid())
    return nullptr;

  const QJsonObject object = propertyValue.toJsonObject();
  if (!object.contains(ConfigKeys::DATA))
    return nullptr;

  auto callerName = object.value(ConfigKeys::DATA).toString();
  if (callerName.isEmpty() || callerName == QStringLiteral("-"))
    return nullptr;

  const auto callers = storage.getPossibleCallers(node->id(), Types::PropertyTypes::UNKNOWN);
  if (callers.isEmpty())
    return nullptr;

  for (const auto& caller : callers)
  {
    const QVariant callerNameProperty = caller->getProperty(ConfigKeys::NAME);
    if (callerNameProperty.isValid() && callerNameProperty.toString() == callerName)
      return caller;
  }

  return nullptr;
}

QString selectedComponentIconPath(const std::shared_ptr<NodeSaveInfo>& caller, const ConfigurationTable* configTable)
{
  if (!caller || isTaskCaller(*caller, configTable))
    return QString();

  return resolveCapabilityIconPath(caller->getIcon(), caller->getnodeId(), configTable);
}

std::optional<QColor> optionalColorProperty(const QVariant& value)
{
  if (!value.isValid())
    return std::nullopt;

  const QString name = value.toString().trimmed();
  if (name.isEmpty() || !QColor::isValidColorName(name))
    return std::nullopt;

  return QColor::fromString(name);
}

QColor callerBackgroundColor(const NodeSaveInfo& caller, const ConfigurationTable* configTable)
{
  if (const auto color = optionalColorProperty(caller.getProperty(QStringLiteral("color"))))
    return *color;

  if (configTable)
  {
    if (const auto cfg = configTable->get(caller.getnodeId()))
      return cfg->body.backgroundColor;
  }

  return QColor(0xe6, 0xe6, 0xe6);
}

QString resolveConfigIconPath(const QString& iconPath)
{
  if (iconPath.isEmpty())
    return {};
  const QString fromApp = AppPaths::icon(iconPath);
  if (!fromApp.isEmpty())
    return fromApp;
  return iconPathFromTheme(iconPath);
}
}  // namespace

QString resolveCapabilityIconPath(const QString& storedIcon,
                                  const QString& nodeTypeKey,
                                  const ConfigurationTable* configTable)
{
  if (!storedIcon.isEmpty())
  {
    if (QFileInfo::exists(storedIcon))
      return storedIcon;

    const QString byFileName = resolveConfigIconPath(QFileInfo(storedIcon).fileName());
    if (!byFileName.isEmpty())
      return byFileName;
  }

  const auto cfg = configTable ? configTable->get(nodeTypeKey) : nullptr;
  if (cfg && !cfg->body.iconPath.isEmpty())
    return resolveConfigIconPath(cfg->body.iconPath);

  if (isTaskTypeKey(nodeTypeKey, cfg.get()))
    return resolveConfigIconPath(kTaskFallbackIcon);

  return {};
}

QIcon capabilityPickerIcon(const QString& iconPath, const QSize& logicalSize, qreal devicePixelRatio)
{
  const bool taskIcon =
      QFileInfo(iconPath).fileName().compare(kTaskFallbackIcon, Qt::CaseInsensitive) == 0;
  if (!taskIcon)
    return iconPath.isEmpty() ? QIcon() : QIcon(iconPath);

  const qreal dpr = qMax(1.0, devicePixelRatio);
  const int w = qMax(1, qRound(logicalSize.width() * dpr));
  const int h = qMax(1, qRound(logicalSize.height() * dpr));
  QPixmap pixmap(w, h);
  pixmap.setDevicePixelRatio(dpr);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, true);
  const qreal pad = 1.0 * dpr;
  paintStructuralTaskIcon(&painter,
                          QRectF(pad, pad, w - pad * 2.0, h - pad * 2.0),
                          QPen(Qt::black, qMax(1.0, dpr)));
  return QIcon(pixmap);
}

void paintEmptySlotSvg(QPainter* painter, const QPointF& center, qreal diameter, bool hovered)
{
  if (!painter || diameter <= 0.0)
    return;

  const QString slotPath = iconPathFromTheme(hovered ? QStringLiteral("filled_slot.svg")
                                                     : QStringLiteral("empty_slot.svg"));
  QSvgRenderer slotRenderer(slotPath);
  if (!slotRenderer.isValid())
    return;

  const qreal radius = diameter * 0.5;
  const QRectF target(center.x() - radius, center.y() - radius, diameter, diameter);
  painter->setRenderHint(QPainter::Antialiasing, true);
  slotRenderer.render(painter, target);
}

void setSceneViewCursor(QGraphicsItem* item, bool pointingHand)
{
  if (!item || !item->scene())
    return;

  if (auto* view = dynamic_cast<QGraphicsView*>(item->scene()->parent()))
    view->setCursor(pointingHand ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

void applyAddCapabilityHover(QGraphicsItem* item, bool& hoveredState, bool hovered)
{
  if (!item || hoveredState == hovered)
    return;

  hoveredState = hovered;
  setSceneViewCursor(item, hovered);
  item->setToolTip(hovered ? QObject::tr("Add capability") : QString());
  item->update();
}

QFont callEventLabelFont(qreal diameter)
{
  QFont font;
  // Keep the event caption compact so it stays inside the Call body.
  const qreal fontPt = qBound(5.5, diameter * 0.16, 7.5);
  font.setPointSizeF(fontPt);
  return font;
}

qreal callCapabilityStackOverhang(qreal diameter, bool withEventLabel, bool withEventChip)
{
  if (!withEventLabel && !withEventChip)
    return 0.0;

  const QFont nameFont = callEventLabelFont(diameter);
  return kCallCapabilityLabelGap + QFontMetricsF(nameFont).height();
}

QPointF callCapabilityIconCenter(const QRectF& drawingBounds,
                                 qreal diameter,
                                 bool withEventLabel,
                                 bool withEventChip)
{
  QPointF center = drawingBounds.center();
  if (diameter <= 0.0)
    return center;

  const qreal overhang = callCapabilityStackOverhang(diameter, withEventLabel, withEventChip);
  if (overhang > 0.0)
    center.ry() -= overhang * 0.5;
  return center;
}

qreal callChipChevronSize(const QFontMetricsF& fm)
{
  return qMax(5.0, fm.height() * 0.45);
}

void paintDownChevron(QPainter* painter, const QRectF& rect)
{
  QPolygonF triangle;
  triangle << QPointF(rect.left(), rect.top() + rect.height() * 0.2)
           << QPointF(rect.right(), rect.top() + rect.height() * 0.2)
           << QPointF(rect.center().x(), rect.bottom() - rect.height() * 0.15);
  painter->drawPolygon(triangle);
}

qreal waitChevronSize(qreal waitIconDiameter)
{
  return qBound(8.0, waitIconDiameter * 0.22, 12.0);
}

struct WaitChipLayout
{
  QRectF pause;
  QRectF chevron;
};

WaitChipLayout waitChipLayout(const QRectF& drawingBounds, qreal waitIconDiameter)
{
  WaitChipLayout layout;
  if (waitIconDiameter <= 0.0 || !drawingBounds.isValid())
    return layout;

  const qreal chevron = waitChevronSize(waitIconDiameter);
  constexpr qreal kPauseToChevronGap = 4.0;
  const qreal overhang = kPauseToChevronGap + chevron;
  QPointF center = drawingBounds.center();
  center.ry() -= overhang * 0.5;

  qreal pauseD = waitIconDiameter;
  const qreal maxD = qMax(0.0, drawingBounds.height() - overhang - 4.0);
  if (pauseD > maxD)
    pauseD = maxD;

  layout.pause = QRectF(center.x() - pauseD * 0.5, center.y() - pauseD * 0.5, pauseD, pauseD);
  layout.chevron = QRectF(center.x() - chevron * 0.5, layout.pause.bottom() + kPauseToChevronGap, chevron, chevron);
  if (layout.chevron.bottom() > drawingBounds.bottom() - 1.0)
    layout.chevron.moveBottom(drawingBounds.bottom() - 1.0);
  return layout;
}

QRectF waitPauseLocalRect(const QRectF& drawingBounds, qreal waitIconDiameter)
{
  return waitChipLayout(drawingBounds, waitIconDiameter).pause;
}

QRectF waitCapabilityChipLocalRect(const QRectF& drawingBounds, qreal waitIconDiameter)
{
  return waitChipLayout(drawingBounds, waitIconDiameter).chevron;
}

void paintWaitCapabilityChip(const NodeItem* node,
                             QPainter* painter,
                             const QRectF& drawingBounds,
                             qreal waitIconDiameter,
                             bool hovered)
{
  Q_UNUSED(node);
  if (!painter)
    return;

  const QRectF chevron = waitCapabilityChipLocalRect(drawingBounds, waitIconDiameter);
  if (chevron.isEmpty())
    return;

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(hovered ? Config::HOVER : Config::FOREGROUND));
  paintDownChevron(painter, chevron);
}

QRectF callEventChipLocalRect(const QRectF& drawingBounds, qreal diameter, const QString& chipText)
{
  if (diameter <= 0.0 || chipText.isEmpty() || !drawingBounds.isValid())
    return {};

  const QPointF iconCenter = callCapabilityIconCenter(drawingBounds, diameter, false, true);
  const qreal radius = diameter * 0.5;
  const QFont font = callEventLabelFont(diameter);
  const QFontMetricsF fm(font);
  const qreal chevron = callChipChevronSize(fm);
  const qreal maxW = qMax(0.0, drawingBounds.width() - 4.0);
  const qreal maxTextW = qMax(0.0, maxW - kCallChipChevronGap - chevron);
  const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
  const qreal textW = fm.horizontalAdvance(elided);
  const qreal chipW = qMin(maxW, textW + kCallChipChevronGap + chevron);
  const qreal chipH = fm.height();
  QRectF chip(iconCenter.x() - chipW * 0.5, iconCenter.y() + radius + kCallCapabilityLabelGap, chipW, chipH);
  if (chip.bottom() > drawingBounds.bottom() - 1.0)
    chip.setBottom(drawingBounds.bottom() - 1.0);
  if (chip.height() < fm.height() * 0.6)
    return {};
  return chip;
}

void paintCallEventChip(QPainter* painter,
                        const QRectF& drawingBounds,
                        qreal diameter,
                        const QString& chipText,
                        bool hovered)
{
  if (!painter)
    return;

  const QRectF chip = callEventChipLocalRect(drawingBounds, diameter, chipText);
  if (chip.isEmpty())
    return;

  const QFont font = callEventLabelFont(diameter);
  const QFontMetricsF fm(font);
  const qreal chevron = callChipChevronSize(fm);
  const qreal maxTextW = qMax(0.0, chip.width() - kCallChipChevronGap - chevron);
  const QString elided = fm.elidedText(chipText, Qt::ElideRight, maxTextW);
  const QColor color = hovered ? Config::HOVER : Config::FOREGROUND;

  painter->setRenderHint(QPainter::Antialiasing, true);
  const QRectF textRect(chip.left(), chip.top(), maxTextW, chip.height());
  painter->setFont(font);
  painter->setPen(QPen(color));
  painter->setBrush(Qt::NoBrush);
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

  const QRectF chevronRect(chip.right() - chevron, chip.center().y() - chevron * 0.5, chevron, chevron);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(color));
  paintDownChevron(painter, chevronRect);
}

QRectF linkOutChevronLocalRect(const QRectF& drawingBounds)
{
  if (!drawingBounds.isValid() || drawingBounds.isEmpty())
    return {};

  const qreal size = qBound(8.0, qMin(drawingBounds.width(), drawingBounds.height()) * 0.14, 12.0);
  constexpr qreal kRightMargin = 10.0;
  constexpr qreal kBottomMargin = 18.0;
  return QRectF(drawingBounds.right() - kRightMargin - size, drawingBounds.bottom() - kBottomMargin - size, size, size);
}

void paintLinkOutChevron(QPainter* painter, const QRectF& drawingBounds, bool hovered)
{
  if (!painter)
    return;

  const QRectF chevron = linkOutChevronLocalRect(drawingBounds);
  if (chevron.isEmpty())
    return;

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(hovered ? Config::HOVER : Config::FOREGROUND));
  paintDownChevron(painter, chevron);
}

void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter)
{
  const QRectF drawingBounds = node->drawingRect(node->nodeRect());
  painter->setRenderHint(QPainter::Antialiasing, true);

  const bool isCall = call_capability::isCallNodeType(node->nodeType());
  const bool isWait = call_capability::isWaitNodeType(node->nodeType());
  const bool isCapabilitySlotNode = isCall || isWait;
  // Call/Wait already have their own SVG body; the task overlay preview's dashed slot would ghost
  // if the capability icon is nudged away from the geometric centre.
  if (!isCapabilitySlotNode)
    paintStructuralTaskOverlayPreview(painter, drawingBounds, QPen(Config::FOREGROUND, 1.0));

  if (isWait)
    return;

  if (node->config()->body.nodeSvg.isEmpty())
    return;

  const QGraphicsScene* sc = node->scene();
  if (!sc)
    return;

  const auto* canvas = dynamic_cast<const Canvas*>(sc);
  if (!canvas)
    return;

  const auto storage = canvas->projectStorage();
  if (!storage)
    return;

  const ConfigurationTable* configTable = canvas->configurationTable().get();
  const auto caller = selectedComponentCaller(node, *storage);
  if (!caller)
    return;

  const bool taskCaller = isTaskCaller(*caller, configTable);
  const QString iconPath = selectedComponentIconPath(caller, configTable);
  // Call: event chip is painted by CallNode. Else: caller name (unused for drawing).
  QString underIconLabel;
  if (!isCall)
    underIconLabel = caller->getProperty(ConfigKeys::NAME).toString().trimmed();
  const bool showUnderIconLabel = !underIconLabel.isEmpty();
  if (iconPath.isEmpty() && !taskCaller && !showUnderIconLabel && !isCall)
    return;

  const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * node->config()->body.iconScale
                         * kComponentOverlayDiameterFactor;
  const qreal radius = diameter * 0.5;
  const QPointF center = isCall ? callCapabilityIconCenter(drawingBounds, diameter, false, true)
                                : drawingBounds.center();

  if (taskCaller)
  {
    const QRectF slot(center.x() - radius, center.y() - radius, diameter, diameter);
    paintStructuralTaskIcon(painter, slot, QPen(Config::FOREGROUND, 1.0 / node->baseScale()));
    return;
  }

  painter->setPen(isCapabilitySlotNode ? Qt::NoPen : QPen(Qt::black, 1.0 / node->baseScale()));
  painter->setBrush(QBrush(callerBackgroundColor(*caller, configTable)));
  painter->drawEllipse(center, radius, radius);
  if (!iconPath.isEmpty())
    renderSvgInEllipse(painter, iconPath, center, diameter);
}
}  // namespace behaviour
