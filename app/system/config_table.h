#pragma once

#include <QString>
#include <map>

#include "config.h"
#include "result.h"

class ConfigurationTable
{
public:
  ConfigurationTable();

  VoidResult add(const QString& key, std::shared_ptr<NodeConfig> value);
  std::shared_ptr<NodeConfig> get(const QString& key) const;

private:
  std::map<QString, std::shared_ptr<NodeConfig>> mMap;
};
