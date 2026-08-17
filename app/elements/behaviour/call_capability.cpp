#include "elements/behaviour/call_capability.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QVariantMap>

#include "elements/node.h"
#include "keys.h"
#include "save_info.h"

namespace call_capability
{
namespace
{
QJsonObject capabilityObject(const NodeItem& node)
{
  const QVariant existing = node.getProperty(kCapabilityProperty);
  if (!existing.isValid())
    return {};

  QJsonObject object = existing.toJsonObject();
  if (object.isEmpty())
  {
    const QVariantMap map = existing.toMap();
    if (!map.isEmpty())
      object = QJsonObject::fromVariantMap(map);
  }
  return object;
}

void writeCapabilityData(NodeItem& node, const QString& name, const QJsonArray& options)
{
  QJsonObject object = capabilityObject(node);
  object[ConfigKeys::DATA] = name;
  object[ConfigKeys::OPTIONS] = options;
  node.setProperty(kCapabilityProperty, QVariant::fromValue(object));
}

bool eventListContains(const SaveInfo& storage,
                       const QString& capabilityId,
                       const QString& mode,
                       const QString& eventName)
{
  if (eventName.isEmpty())
    return false;
  const auto events = storage.getEventsOfTypeFromNode(capabilityId, eventTypesForMode(mode));
  for (const auto& event : events)
  {
    if (event->getname() == eventName)
      return true;
  }
  return false;
}
}  // namespace

bool isCallNodeType(const QString& nodeType)
{
  return nodeType == QStringLiteral("Koda::Call");
}

bool isWaitNodeType(const QString& nodeType)
{
  return nodeType == QStringLiteral("Koda::Wait for signal")
         || nodeType == QStringLiteral("Koda::Wait");
}

bool allowsCapabilitySignalTransitions(const NodeItem& node)
{
  const QString& nodeType = node.nodeType();
  // Legacy Async task always allows signal transitions.
  if (nodeType == QStringLiteral("Koda::Async task") || nodeType == QStringLiteral("Mission::Async task"))
    return true;

  // Unified Call: only Async mode (Sync Call cannot attach capability OUT signals).
  if (isCallNodeType(nodeType))
    return node.getProperty(kModeProperty).toString() == kModeAsync;

  return false;
}

bool hasCapabilitySelected(const NodeItem& node)
{
  const QString data = capabilityName(node);
  return !data.isEmpty();
}

bool canAsync(const SaveInfo& storage, const QString& capabilityId)
{
  return !storage.getEventsOfTypeFromNode(capabilityId, {Types::CallType::TRIGGER}).isEmpty();
}

bool canSync(const SaveInfo& storage, const QString& capabilityId)
{
  return !storage
              .getEventsOfTypeFromNode(capabilityId,
                                       {Types::CallType::TRIGGER, Types::CallType::ABORT, Types::CallType::IN})
              .isEmpty();
}

QString defaultMode(const SaveInfo& storage, const QString& capabilityId)
{
  return canAsync(storage, capabilityId) ? kModeAsync : kModeSync;
}

QList<Types::CallType> eventTypesForMode(const QString& mode)
{
  if (mode == kModeAsync)
    return {Types::CallType::TRIGGER};
  return {Types::CallType::TRIGGER, Types::CallType::ABORT, Types::CallType::IN};
}

void resolveModeAndEvent(const SaveInfo& storage,
                         const QString& capabilityId,
                         QString& mode,
                         QString& eventName)
{
  const bool asyncOk = canAsync(storage, capabilityId);
  const bool syncOk = canSync(storage, capabilityId);
  if (!asyncOk && !syncOk)
  {
    mode.clear();
    eventName.clear();
    return;
  }

  if (mode != kModeSync && mode != kModeAsync)
    mode = defaultMode(storage, capabilityId);
  if (mode == kModeAsync && !asyncOk)
    mode = kModeSync;
  if (mode == kModeSync && !syncOk && asyncOk)
    mode = kModeAsync;

  if (!eventListContains(storage, capabilityId, mode, eventName))
  {
    eventName.clear();
    const auto events = storage.getEventsOfTypeFromNode(capabilityId, eventTypesForMode(mode));
    if (!events.isEmpty())
      eventName = events.first()->getname();
  }
}

QString nodeSvgForState(const NodeItem& node)
{
  // Unset: node_call.svg (arrow + overlaid empty slot). Once a capability is chosen,
  // body switches to async/sync SVG as before.
  if (!hasCapabilitySelected(node))
    return kNodeSvgCall;
  return node.getProperty(kModeProperty).toString() == kModeAsync ? kNodeSvgAsync : kNodeSvgSync;
}

QString capabilityName(const NodeItem& node)
{
  const QString data = capabilityObject(node).value(ConfigKeys::DATA).toString().trimmed();
  if (data.isEmpty() || data == QStringLiteral("-"))
    return {};
  return data;
}

QString resolveCapabilityId(const NodeItem& node, const SaveInfo& storage)
{
  const QString name = capabilityName(node);
  if (name.isEmpty())
    return {};

  for (const auto& caller : storage.getPossibleCallers(node.id(), Types::PropertyTypes::EVENT_SELECT))
  {
    const QVariant callerName = caller->getProperty(ConfigKeys::NAME);
    if (callerName.isValid() && callerName.toString() == name)
      return caller->getid();
  }
  return {};
}

QString currentEventName(const NodeItem& node)
{
  const QJsonArray options = capabilityObject(node).value(ConfigKeys::OPTIONS).toArray();
  if (options.size() <= kEventIndex)
    return {};
  return options.at(kEventIndex).toObject().value(ConfigKeys::DATA).toString().trimmed();
}

void applyCapabilitySelection(NodeItem& node,
                              const QString& name,
                              const QString& capabilityId,
                              SaveInfo* storage)
{
  if (name.isEmpty() || capabilityId.isEmpty())
    return;

  writeCapabilityData(node, name, QJsonArray());

  if (!storage)
    return;

  QString mode;
  QString eventName;
  resolveModeAndEvent(*storage, capabilityId, mode, eventName);
  if (mode.isEmpty() || eventName.isEmpty())
    return;

  applyModeAndEvent(node, mode, eventName);
}

void applyCapabilitySelectionNameOnly(NodeItem& node, const QString& name)
{
  if (name.isEmpty())
    return;

  writeCapabilityData(node, name, QJsonArray());
}

void applyModeAndEvent(NodeItem& node, const QString& mode, const QString& eventName)
{
  if (mode != kModeSync && mode != kModeAsync)
    return;
  if (eventName.isEmpty())
    return;

  // Avoid no-op setProperty storms (e.g. menu sync reacting to nodeModified).
  if (node.getProperty(kModeProperty).toString() == mode && currentEventName(node) == eventName)
    return;

  const QString previousEvent = currentEventName(node);
  node.setProperty(kModeProperty, mode);

  const QString name = capabilityName(node);
  if (name.isEmpty())
    return;

  // Keep argument options when only the call mode changes (same event).
  QJsonArray options;
  if (previousEvent == eventName)
    options = capabilityObject(node).value(ConfigKeys::OPTIONS).toArray();

  QJsonObject eventItem;
  eventItem[ConfigKeys::DATA] = eventName;
  eventItem[ConfigKeys::TYPE] = Types::PropertyTypesToString(Types::PropertyTypes::EVENT_SELECT);
  eventItem[ConfigKeys::IS_VARIABLE] = false;
  while (options.size() <= kEventIndex)
    options.append(QJsonObject());
  options[kEventIndex] = eventItem;
  if (previousEvent != eventName)
  {
    while (options.size() > kArgIndex)
      options.removeLast();
  }

  writeCapabilityData(node, name, options);
}

void writeCapabilityOption(NodeItem& node,
                           int index,
                           const QString& value,
                           Types::PropertyTypes dataType,
                           bool isVariable)
{
  QJsonObject object = capabilityObject(node);
  if (object.isEmpty() && !hasCapabilitySelected(node))
    return;

  QJsonArray array = object.value(ConfigKeys::OPTIONS).toArray();

  QJsonObject item;
  item[ConfigKeys::DATA] = value;
  item[ConfigKeys::TYPE] = Types::PropertyTypesToString(dataType);
  item[ConfigKeys::IS_VARIABLE] = isVariable;

  while (array.size() <= index)
    array.append(QJsonObject());
  array[index] = item;
  object[ConfigKeys::OPTIONS] = array;
  node.setProperty(kCapabilityProperty, QVariant::fromValue(object));
}

bool isArgumentVariable(Types::PropertyTypes argType, const QString& value)
{
  if (argType == Types::PropertyTypes::INTEGER)
  {
    bool ok = false;
    value.toInt(&ok);
    return !ok;
  }
  if (argType == Types::PropertyTypes::REAL)
  {
    bool ok = false;
    value.toDouble(&ok);
    return !ok;
  }
  if (argType == Types::PropertyTypes::BOOLEAN)
  {
    return !(value == QStringLiteral("true") || value == QStringLiteral("false")
             || value == QStringLiteral("True") || value == QStringLiteral("False"));
  }
  if (argType == Types::PropertyTypes::STRING)
  {
    return !(value.size() > 2 && value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"')));
  }
  return false;
}
}  // namespace call_capability
