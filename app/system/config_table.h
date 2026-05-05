#pragma once

#include <QString>
#include <map>

#include "config.h"
#include "result.h"

/**
 * @brief Manages a table of configuration settings.
 */
class ConfigurationTable
{
public:
  /**
   * @brief Constructs an empty ConfigurationTable.
   */
  ConfigurationTable();

  /**
   * @brief Adds a configuration entry to the table.
   *
   * @param key The unique identifier for the configuration entry.
   * @param value A shared pointer to the NodeConfig object representing the configuration.
   * @return VoidResult indicating success or failure of the operation.
   */
  VoidResult add(const QString& key, std::shared_ptr<NodeConfig> value);

  /**
   * @brief Retrieves a configuration entry from the table.
   *
   * @param key The unique identifier for the configuration entry.
   * @return A shared pointer to the NodeConfig object representing the configuration, or nullptr if not found.
   */
  std::shared_ptr<NodeConfig> get(const QString& key) const;

private:
  /**
   * @brief Internal map storing configuration entries.
   */
  std::map<QString, std::shared_ptr<NodeConfig>> mMap;
};
