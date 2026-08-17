#pragma once

#include "elements/node.h"

#include <QIcon>
#include <QSize>

class ConfigurationTable;
class QFontMetricsF;
class QGraphicsItem;
class QPainter;
class QPointF;
class QRectF;

namespace behaviour
{
/** Diameter of the capability/empty-slot circle relative to node body * iconScale. */
inline constexpr qreal kComponentOverlayDiameterFactor = 0.40;

/** Wait pause-icon size relative to node body * iconScale. */
inline constexpr qreal kWaitComponentOverlayDiameterFactor = 0.48;

/** Canvas body SVG for Wait (hexagon; wait icon and chip are overlaid). */
inline const QString kWaitCanvasNodeSvg = QStringLiteral("shape_middle.svg");
inline const QString kWaitIconSvg = QStringLiteral("node_wait_icon.svg");

/** Gap between the Call capability icon and the event-name chip. */
inline constexpr qreal kCallCapabilityLabelGap = 4.0;

/** Gap between the event name and the expand chevron. */
inline constexpr qreal kCallChipChevronGap = 3.0;

/** Paints the selected capability component overlay on Call / async / sync task nodes. */
void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter);

/**
 * Renders the empty capability slot centered at @p center.
 * When @p hovered, uses filled_slot.svg instead of empty_slot.svg.
 */
void paintEmptySlotSvg(QPainter* painter, const QPointF& center, qreal diameter, bool hovered = false);

/** Pointing-hand vs arrow cursor on the item's view, if it has one. */
void setSceneViewCursor(QGraphicsItem* item, bool pointingHand);

/**
 * Shared empty-slot hover feedback (cursor, tooltip, repaint) for Task / Call / Wait.
 * No-op when @p hoveredState already matches @p hovered.
 */
void applyAddCapabilityHover(QGraphicsItem* item, bool& hoveredState, bool hovered);

qreal callChipChevronSize(const QFontMetricsF& fm);
void paintDownChevron(QPainter* painter, const QRectF& rect);

/**
 * Local-coord center for a Call capability icon (unset slot and selected overlay share this).
 * When @p withEventLabel or @p withEventChip is true, the center is nudged up so icon + event chip read as one stack.
 */
QPointF callCapabilityIconCenter(const QRectF& drawingBounds,
                                 qreal diameter,
                                 bool withEventLabel,
                                 bool withEventChip = false);

/** Pause icon rect, nudged up so the chevron fits below. */
QRectF waitPauseLocalRect(const QRectF& drawingBounds, qreal waitIconDiameter);

/** Local rect for Wait's chevron under the pause (click target). */
QRectF waitCapabilityChipLocalRect(const QRectF& drawingBounds, qreal waitIconDiameter);

void paintWaitCapabilityChip(const NodeItem* node,
                             QPainter* painter,
                             const QRectF& drawingBounds,
                             qreal waitIconDiameter,
                             bool hovered);

/** Local rect for the Call event name + chevron chip (empty if not applicable). */
QRectF callEventChipLocalRect(const QRectF& drawingBounds, qreal diameter, const QString& chipText);

void paintCallEventChip(QPainter* painter,
                        const QRectF& drawingBounds,
                        qreal diameter,
                        const QString& chipText,
                        bool hovered);

/** Local rect for Link out's target-picker chevron (bottom-right of the body). */
QRectF linkOutChevronLocalRect(const QRectF& drawingBounds);

void paintLinkOutChevron(QPainter* painter, const QRectF& drawingBounds, bool hovered);

/** Resolves a capability icon from a stored path and/or library config type key. */
QString resolveCapabilityIconPath(const QString& storedIcon,
                                  const QString& nodeTypeKey,
                                  const ConfigurationTable* configTable);

/** Picker-tile icon; Task uses a compact rounded-rect glyph instead of the full node SVG. */
QIcon capabilityPickerIcon(const QString& iconPath, const QSize& logicalSize, qreal devicePixelRatio = 1.0);
}  // namespace behaviour
