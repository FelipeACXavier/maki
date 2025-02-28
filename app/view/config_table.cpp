#include "config_table.h"

ConfigurationTable::ConfigurationTable()
    : mMap({})
{
}

VoidResult ConfigurationTable::add(const QString& key, std::shared_ptr<NodeConfig> value)
{
  if (mMap.find(key) != mMap.end())
    return VoidResult::Failed("Key already exists");

  mMap.insert({key, value});

  return VoidResult();
}

std::shared_ptr<NodeConfig> ConfigurationTable::get(const QString& key) const
{
  if (mMap.find(key) == mMap.end())
    return nullptr;

  return mMap.at(key);
}
