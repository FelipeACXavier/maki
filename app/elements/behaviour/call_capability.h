#pragma once

#include <QList>
#include <QString>

#include "types.h"

class NodeItem;
class SaveInfo;

/** Shared Call / Wait capability rules (mode / events / assignment / options JSON). */
namespace call_capability
{
inline const QString kModeProperty = QStringLiteral("call_mode");
inline const QString kModeSync = QStringLiteral("sync");
inline const QString kModeAsync = QStringLiteral("async");
inline const QString kCapabilityProperty = QStringLiteral("capability");
inline const QString kNodeSvgAsync = QStringLiteral("node_async.svg");
inline const QString kNodeSvgSync = QStringLiteral("node_sync.svg");
/** Initial Call body (palette + canvas before capability): arrow left; slot overlaid. */
inline const QString kNodeSvgCall = QStringLiteral("node_call.svg");

/** capability.options[0] = selected event; following entries are event arguments. */
inline constexpr int kEventIndex = 0;
inline constexpr int kArgIndex = kEventIndex + 1;

bool isCallNodeType(const QString& nodeType);
bool isWaitNodeType(const QString& nodeType);
/**
 * True when @p node may author capability OUT (signal) transitions:
 * Call in async mode, or legacy Async task.
 */
bool allowsCapabilitySignalTransitions(const NodeItem& node);
bool hasCapabilitySelected(const NodeItem& node);

bool canAsync(const SaveInfo& storage, const QString& capabilityId);
bool canSync(const SaveInfo& storage, const QString& capabilityId);
QString defaultMode(const SaveInfo& storage, const QString& capabilityId);
QList<Types::CallType> eventTypesForMode(const QString& mode);

/**
 * Normalizes @p mode / @p eventName for a capability (valid mode + an event that exists for that mode).
 * Leaves both empty-safe when the capability has no usable events.
 */
void resolveModeAndEvent(const SaveInfo& storage,
                         const QString& capabilityId,
                         QString& mode,
                         QString& eventName);

/** Body SVG for the current Call property state. */
QString nodeSvgForState(const NodeItem& node);

/** Display name stored in capability.data, or empty if unset. */
QString capabilityName(const NodeItem& node);

/** Structural capability node id for the selected capability, or empty. */
QString resolveCapabilityId(const NodeItem& node, const SaveInfo& storage);

/** Currently selected event name from capability.options[0], or empty. */
QString currentEventName(const NodeItem& node);

/**
 * Writes capability (+ default mode/event) using the same rules as the properties panel.
 * @p capabilityName is the display name stored in capability.data.
 * @p capabilityId is the structural node id used for event lookup.
 */
void applyCapabilitySelection(NodeItem& node,
                              const QString& capabilityName,
                              const QString& capabilityId,
                              SaveInfo* storage);

/** Writes capability.data only (empty options). Used by Wait — no call_mode / event. */
void applyCapabilitySelectionNameOnly(NodeItem& node, const QString& capabilityName);

/** Sets call_mode and the nested event selection together (preserves args when event is unchanged). */
void applyModeAndEvent(NodeItem& node, const QString& mode, const QString& eventName);

/** Writes one capability.options[index] entry (event or argument). */
void writeCapabilityOption(NodeItem& node,
                           int index,
                           const QString& value,
                           Types::PropertyTypes dataType,
                           bool isVariable);

/** True when @p value is not a literal of @p argType (treated as a variable reference). */
bool isArgumentVariable(Types::PropertyTypes argType, const QString& value);
}  // namespace call_capability
