#pragma once

#include "elements/node.h"

class ConfigurationTable;
class QPainter;
class QPointF;

namespace behaviour
{
/** Diameter of the capability/empty-slot circle relative to node body * iconScale. */
inline constexpr qreal kComponentOverlayDiameterFactor = 0.40;

/** Paints the selected capability component overlay on Call / async / sync task nodes. */
void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter);

/** Renders empty_slot.svg centered at @p center with the given diameter (no icon padding). */
void paintEmptySlotSvg(QPainter* painter, const QPointF& center, qreal diameter);

/** Resolves a capability icon from a stored path and/or library config type key. */
QString resolveCapabilityIconPath(const QString& storedIcon,
                                  const QString& nodeTypeKey,
                                  const ConfigurationTable* configTable);
}  // namespace behaviour
