#include "elements/behaviour/component_overlay.h"

#include <QFileInfo>
#include <QFontMetricsF>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>
#include <QPainter>
#include <QPen>
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
bool isTaskCaller(const NodeSaveInfo& caller, const ConfigurationTable* configTable)
{
  if (caller.getnodeId().endsWith(QStringLiteral("::Task")))
    return true;

  if (!configTable)
    return false;

  const auto cfg = configTable->get(caller.getnodeId());
  return cfg && cfg->type == QStringLiteral("Koda::Task");
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

  if (!configTable)
    return {};

  const auto cfg = configTable->get(nodeTypeKey);
  if (!cfg || cfg->body.iconPath.isEmpty())
    return {};

  return resolveConfigIconPath(cfg->body.iconPath);
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

void applyAddCapabilityHover(QGraphicsItem* item, bool& hoveredState, bool hovered)
{
  if (!item || hoveredState == hovered)
    return;

  hoveredState = hovered;

  if (QGraphicsScene* scene = item->scene())
  {
    if (auto* view = dynamic_cast<QGraphicsView*>(scene->parent()))
      view->setCursor(hovered ? Qt::PointingHandCursor : Qt::ArrowCursor);
  }

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
  qreal overhang = 0.0;
  if (withEventLabel)
  {
    const QFont nameFont = callEventLabelFont(diameter);
    overhang += kCallCapabilityLabelGap + QFontMetricsF(nameFont).height();
  }
  if (withEventChip)
    overhang += kCallEventChipGap + kCallEventChipSize;
  return overhang;
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

QPointF waitCapabilityIconCenter(const QRectF& drawingBounds)
{
  QPointF center = drawingBounds.center();
  center.ry() += drawingBounds.height() * kWaitCapabilityVerticalOffsetFactor;
  return center;
}

QRectF callEventChipLocalRect(const QRectF& drawingBounds, qreal diameter, bool withEventLabel)
{
  if (diameter <= 0.0)
    return {};

  const QPointF iconCenter = callCapabilityIconCenter(drawingBounds, diameter, withEventLabel, true);
  const qreal radius = diameter * 0.5;
  qreal chipTop = iconCenter.y() + radius + kCallEventChipGap;
  if (withEventLabel)
  {
    const QFontMetricsF fm(callEventLabelFont(diameter));
    chipTop = iconCenter.y() + radius + kCallCapabilityLabelGap + fm.height() + kCallEventChipGap;
  }

  return QRectF(iconCenter.x() - kCallEventChipSize * 0.5, chipTop, kCallEventChipSize, kCallEventChipSize);
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

  const QString iconPath = selectedComponentIconPath(caller, configTable);
  // Call: event name under icon. Wait: none (title already names the capability). Else: caller name.
  QString underIconLabel;
  if (isCall)
    underIconLabel = call_capability::currentEventName(*node);
  else if (!isWait)
    underIconLabel = caller->getProperty(ConfigKeys::NAME).toString().trimmed();
  const bool showUnderIconLabel = !underIconLabel.isEmpty();
  if (iconPath.isEmpty() && !showUnderIconLabel)
    return;

  const qreal diameterFactor = isWait ? kWaitComponentOverlayDiameterFactor : kComponentOverlayDiameterFactor;
  const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * node->config()->body.iconScale
                         * diameterFactor;
  const qreal radius = diameter * 0.5;
  const QPointF center = isWait ? waitCapabilityIconCenter(drawingBounds)
                                : (isCall ? callCapabilityIconCenter(drawingBounds, diameter, showUnderIconLabel, true)
                                          : drawingBounds.center());

  painter->setPen(isCapabilitySlotNode ? Qt::NoPen : QPen(Qt::black, 1.0 / node->baseScale()));
  painter->setBrush(QBrush(callerBackgroundColor(*caller, configTable)));
  painter->drawEllipse(center, radius, radius);
  if (!iconPath.isEmpty())
    renderSvgInEllipse(painter, iconPath, center, diameter);

  if (showUnderIconLabel && isCall)
  {
    const QFont nameFont = callEventLabelFont(diameter);
    const QFontMetricsF fm(nameFont);
    const qreal textTop = center.y() + radius + kCallCapabilityLabelGap;
    const qreal maxTextBottom = drawingBounds.bottom() - 1.0;
    const qreal textHeight = qMin(fm.height(), qMax(0.0, maxTextBottom - textTop));
    if (textHeight > 0.0)
    {
      const QRectF textRect(drawingBounds.left(), textTop, drawingBounds.width(), textHeight);
      painter->setFont(nameFont);
      painter->setPen(QPen(Config::FOREGROUND));
      painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop,
                        fm.elidedText(underIconLabel, Qt::ElideRight, textRect.width()));
    }
  }
}
}  // namespace behaviour
