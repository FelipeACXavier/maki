#include "data_flow_model.h"

#include <QUuid>
#include <algorithm>

namespace maki
{

DataFlowModel& DataFlowModel::instance()
{
  static DataFlowModel dataFlowModel;
  return dataFlowModel;
}

void DataFlowModel::clear()
{
  mEntries.clear();
}

DataId DataFlowModel::createEntry(const QString& name, const koda::types::TypeReference& type)
{
  const DataId id = generateId();

  DataEntry entry;
  entry.id = id;
  entry.name = name;
  entry.type = type;
  entry.sourceKind = DataSourceKind::Capability;

  mEntries.emplace(id, std::move(entry));

  return id;
}

DataId DataFlowModel::createMissionParameter(const QString& name, const koda::types::TypeReference& type)
{
  const DataId id = generateId();

  DataEntry entry;
  entry.id = id;
  entry.name = name;
  entry.type = type;
  entry.sourceKind = DataSourceKind::MissionParameter;
  entry.missionValue = MissionValue{};

  mEntries.emplace(id, std::move(entry));

  return id;
}

bool DataFlowModel::removeEntry(const DataId& id)
{
  return mEntries.erase(id) > 0;
}

DataEntry* DataFlowModel::get(const DataId& id)
{
  const auto it = mEntries.find(id);

  if (it == mEntries.end())
    return nullptr;

  return &it->second;
}

const DataEntry* DataFlowModel::get(const DataId& id) const
{
  const auto it = mEntries.find(id);

  if (it == mEntries.end())
    return nullptr;

  return &it->second;
}

const std::unordered_map<DataId, DataEntry>& DataFlowModel::entries() const
{
  return mEntries;
}

bool DataFlowModel::setProducer(const DataId& id, const DataPort& producer)
{
  auto* entry = get(id);

  if (!entry)
    return false;

  // A logical blackboard entry should have exactly one producer.
  entry->producer = producer;
  entry->sourceKind = DataSourceKind::Capability;
  entry->missionValue.reset();

  return true;
}

bool DataFlowModel::addConsumer(const DataId& id, const DataPort& consumer)
{
  auto* entry = get(id);

  if (!entry)
    return false;

  const auto it = std::find(entry->consumers.begin(), entry->consumers.end(), consumer);

  if (it != entry->consumers.end())
    return false;

  entry->consumers.push_back(consumer);

  return true;
}

bool DataFlowModel::removeProducer(const DataId& id)
{
  auto* entry = get(id);

  if (!entry || !entry->producer)
    return false;

  entry->producer.reset();

  return true;
}

bool DataFlowModel::removeConsumer(const DataId& id, const DataPort& consumer)
{
  auto* entry = get(id);

  if (!entry)
    return false;

  const auto oldSize = entry->consumers.size();

  std::erase(entry->consumers, consumer);

  return entry->consumers.size() != oldSize;
}

void DataFlowModel::removeNode(const NodeId& nodeId)
{
  for (auto& [id, entry] : mEntries)
  {
    if (entry.producer && entry.producer->nodeId == nodeId)
      entry.producer.reset();

    std::erase_if(entry.consumers, [&nodeId](const DataPort& port) { return port.nodeId == nodeId; });
  }

  // Optional policy:
  //
  // Remove entries that became completely orphaned after deleting the node.
  std::erase_if(mEntries, [](const auto& pair) {
    const auto& entry = pair.second;

    return entry.sourceKind == DataSourceKind::Capability && !entry.producer && entry.consumers.empty();
  });
}

std::optional<DataId> DataFlowModel::entryProducedBy(const DataPort& producer) const
{
  for (const auto& [id, entry] : mEntries)
    if (entry.producer && *entry.producer == producer)
      return id;

  return std::nullopt;
}

std::optional<DataId> DataFlowModel::entryConsumedBy(const DataPort& consumer) const
{
  for (const auto& [id, entry] : mEntries)
  {
    const auto it = std::find(entry.consumers.begin(), entry.consumers.end(), consumer);

    if (it != entry.consumers.end())
      return id;
  }

  return std::nullopt;
}

std::vector<DataId> DataFlowModel::entriesProducedBy(const NodeId& nodeId) const
{
  std::vector<DataId> result;

  for (const auto& [id, entry] : mEntries)
    if (entry.producer && entry.producer->nodeId == nodeId)
      result.push_back(id);

  return result;
}

std::vector<DataId> DataFlowModel::entriesConsumedBy(const NodeId& nodeId) const
{
  std::vector<DataId> result;

  for (const auto& [id, entry] : mEntries)
  {
    const bool consumes =
        std::any_of(entry.consumers.begin(), entry.consumers.end(), [&nodeId](const DataPort& port) { return port.nodeId == nodeId; });

    if (consumes)
      result.push_back(id);
  }

  return result;
}

bool DataFlowModel::setMissionValue(const DataId& id, const QVariant& value)
{
  auto* entry = get(id);

  if (!entry || entry->sourceKind != DataSourceKind::MissionParameter)
    return false;

  if (!entry->missionValue)
    entry->missionValue = MissionValue{};

  entry->missionValue->value = value;
  entry->missionValue->hasValue = true;

  return true;
}

bool DataFlowModel::clearMissionValue(const DataId& id)
{
  auto* entry = get(id);

  if (!entry || entry->sourceKind != DataSourceKind::MissionParameter || !entry->missionValue)
    return false;

  entry->missionValue->value.clear();
  entry->missionValue->hasValue = false;

  return true;
}

std::vector<DataId> DataFlowModel::missionParameters() const
{
  std::vector<DataId> result;

  for (const auto& [id, entry] : mEntries)
    if (entry.sourceKind == DataSourceKind::MissionParameter)
      result.push_back(id);

  return result;
}

bool DataFlowModel::hasProducer(const DataPort& port) const
{
  return entryProducedBy(port).has_value();
}

bool DataFlowModel::hasConsumer(const DataPort& port) const
{
  for (const auto& [id, entry] : mEntries)
    if (std::find(entry.consumers.begin(), entry.consumers.end(), port) != entry.consumers.end())
      return true;

  return false;
}

bool DataFlowModel::isUnused(const DataId& id) const
{
  const auto* entry = get(id);

  if (!entry)
    return false;

  return entry->consumers.empty();
}

DataId DataFlowModel::generateId() const
{
  return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

}  // namespace maki