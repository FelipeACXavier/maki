#pragma once

#include "elements/node.h"

class ConfigurationTable;
class QPainter;
class QPointF;

namespace behaviour
{
/** Diameter of the capability/empty-slot circle relative to node body * iconScale. */
inline constexpr qreal kComponentOverlayDiameterFactor = 0.40;

/** Gap between the Call capability icon and its event-name label. */
inline constexpr qreal kCallCapabilityLabelGap = 4.0;

/** Size of the expand chevron chip under the Call event label. */
inline constexpr qreal kCallEventChipSize = 16.0;

/** Gap between the event label and the expand chip. */
inline constexpr qreal kCallEventChipGap = 2.0;

/** Paints the selected capability component overlay on Call / async / sync task nodes. */
void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter);

/** Renders empty_slot.svg centered at @p center with the given diameter (no icon padding). */
void paintEmptySlotSvg(QPainter* painter, const QPointF& center, qreal diameter);

/**
 * Local-coord center for a Call capability icon.
 * When @p withEventLabel is true, the center is nudged up so icon + event (+ optional chip) read as one stack.
 */
QPointF callCapabilityIconCenter(const QRectF& drawingBounds,
                                 qreal diameter,
                                 bool withEventLabel,
                                 bool withEventChip = false);

/** Local rect for the Call event expand chip (empty if not applicable). */
QRectF callEventChipLocalRect(const QRectF& drawingBounds, qreal diameter, bool withEventLabel);

/** Resolves a capability icon from a stored path and/or library config type key. */
QString resolveCapabilityIconPath(const QString& storedIcon,
                                  const QString& nodeTypeKey,
                                  const ConfigurationTable* configTable);
}  // namespace behaviour
