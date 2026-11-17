#pragma once

#include <QString>
#include <QVariant>
#include <optional>
#include <unordered_map>
#include <vector>

#include "typing/type_reference.h"

namespace maki
{

using NodeId = QString;
using DataId = QString;

/**
 * Identifies one particular argument of one capability.
 *
 * This is deliberately more precise than simply referring to a node,
 * because a capability may consume/produce several values of the same type.
 */
struct DataPort
{
  NodeId nodeId;

  QString eventName;
  QString argumentName;

  bool operator==(const DataPort& other) const
  {
    return nodeId == other.nodeId && eventName == other.eventName && argumentName == other.argumentName;
  }
};

/**
 * Where the value of a DataEntry originates.
 */
enum class DataSourceKind
{
  Capability,
  MissionParameter
};

/**
 * User-provided value used to instantiate a mission parameter.
 *
 * QVariant is convenient inside MAKI because the editor is Qt based.
 * You may later replace this with a more explicit typed representation
 * if nested records become important.
 */
struct MissionValue
{
  QVariant value;
  bool hasValue = false;
};

/**
 * Represents one logical value flowing through the mission.
 *
 * Example:
 *
 * DetectAruco.pose
 *       |
 *       v
 * objectPose
 *       |
 *       +---- Navigate.target
 *       +---- Pick.pose
 */
struct DataEntry
{
  DataId id;
  QString name;
  koda::types::TypeReference type;
  DataSourceKind sourceKind = DataSourceKind::Capability;

  std::optional<DataPort> producer;
  std::vector<DataPort> consumers;
  std::optional<MissionValue> missionValue;
};

class DataFlowModel
{
public:
  DataFlowModel() = default;

  static DataFlowModel& instance();
  void clear();

  DataId createEntry(const QString& name, const koda::types::TypeReference& type);
  DataId createMissionParameter(const QString& name, const koda::types::TypeReference& type);
  bool removeEntry(const DataId& id);

  DataEntry* get(const DataId& id);
  const DataEntry* get(const DataId& id) const;
  const std::unordered_map<DataId, DataEntry>& entries() const;

  bool setProducer(const DataId& id, const DataPort& producer);
  bool addConsumer(const DataId& id, const DataPort& consumer);
  bool removeProducer(const DataId& id);
  bool removeConsumer(const DataId& id, const DataPort& consumer);
  void removeNode(const NodeId& nodeId);

  std::optional<DataId> entryProducedBy(const DataPort& producer) const;
  std::optional<DataId> entryConsumedBy(const DataPort& consumer) const;
  std::vector<DataId> entriesProducedBy(const NodeId& nodeId) const;
  std::vector<DataId> entriesConsumedBy(const NodeId& nodeId) const;

  bool setMissionValue(const DataId& id, const QVariant& value);
  bool clearMissionValue(const DataId& id);
  std::vector<DataId> missionParameters() const;

  bool hasProducer(const DataPort& port) const;
  bool hasConsumer(const DataPort& port) const;
  bool isUnused(const DataId& id) const;

private:
  DataId generateId() const;

private:
  std::unordered_map<DataId, DataEntry> mEntries;
};

}  // namespace maki