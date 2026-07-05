#include "elements/behaviour/component_overlay.h"

#include <QFileInfo>
#include <QGraphicsScene>
#include <QPainter>
#include <QSvgRenderer>

#include "app_configs.h"
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
constexpr qreal kComponentOverlayDiameterFactor = 0.40;

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

QString resolveStoredIconPath(const QString& storedIcon, const QString& nodeId, const ConfigurationTable* configTable)
{
  if (!storedIcon.isEmpty())
  {
    if (QFileInfo::exists(storedIcon))
      return storedIcon;

    const QString byFileName = iconPathFromTheme(QFileInfo(storedIcon).fileName());
    if (!byFileName.isEmpty())
      return byFileName;
  }

  if (!configTable)
    return QString();

  const auto cfg = configTable->get(nodeId);
  if (!cfg)
    return QString();

  if (!cfg->body.iconPath.isEmpty())
  {
    const QString resolved = iconPathFromTheme(cfg->body.iconPath);
    if (!resolved.isEmpty())
      return resolved;
  }

  return QString();
}

std::shared_ptr<NodeSaveInfo> selectedComponentCaller(const NodeItem* node, const SaveInfo& storage)
{
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

  return resolveStoredIconPath(caller->getIcon(), caller->getnodeId(), configTable);
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
}  // namespace

void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter)
{
  const QRectF drawingBounds = node->drawingRect(node->nodeRect());
  painter->setRenderHint(QPainter::Antialiasing, true);
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
  if (iconPath.isEmpty())
    return;

  const qreal diameter = qMin(drawingBounds.width(), drawingBounds.height()) * node->config()->body.iconScale * kComponentOverlayDiameterFactor;
  const QPointF center = drawingBounds.center();
  const qreal radius = diameter * 0.5;

  painter->setPen(QPen(Qt::black, 1.0 / node->baseScale()));
  painter->setBrush(QBrush(callerBackgroundColor(*caller, configTable)));
  painter->drawEllipse(center, radius, radius);
  renderSvgInEllipse(painter, iconPath, center, diameter);
}
}  // namespace behaviour
