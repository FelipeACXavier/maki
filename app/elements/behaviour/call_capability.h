#pragma once

#include <QList>
#include <QString>

#include "types.h"

class NodeItem;
class SaveInfo;

/** Shared Call-node domain rules (mode / events / capability assignment). */
namespace call_capability
{
inline const QString kModeProperty = QStringLiteral("call_mode");
inline const QString kModeSync = QStringLiteral("sync");
inline const QString kModeAsync = QStringLiteral("async");
inline const QString kCapabilityProperty = QStringLiteral("capability");
inline const QString kNodeSvgAsync = QStringLiteral("node_async.svg");
inline const QString kNodeSvgSync = QStringLiteral("node_sync.svg");
inline const QString kNodeSvgUnset = QStringLiteral("shape_middle.svg");
inline constexpr int kEventIndex = 0;

bool isCallNodeType(const QString& nodeType);
bool hasCapabilitySelected(const NodeItem& node);

bool canAsync(const SaveInfo& storage, const QString& capabilityId);
bool canSync(const SaveInfo& storage, const QString& capabilityId);
QString defaultMode(const SaveInfo& storage, const QString& capabilityId);
QList<Types::CallType> eventTypesForMode(const QString& mode);

/** Body SVG for the current Call property state. */
QString nodeSvgForState(const NodeItem& node);

/**
 * Writes capability (+ default mode/event/name) using the same rules as the properties panel.
 * @p capabilityName is the display name stored in capability.data.
 * @p capabilityId is the structural node id used for event lookup.
 */
void applyCapabilitySelection(NodeItem& node,
                              const QString& capabilityName,
                              const QString& capabilityId,
                              SaveInfo* storage);
}  // namespace call_capability
