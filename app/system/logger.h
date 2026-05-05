#pragma once
#include <QMap>
#include <QObject>
#include <QString>

#include "ilogging.h"
#include "logging.h"

namespace maki
{
class ILogging;
}

/**
 * @brief Logger class that handles logging functionality.
 */
class Logger : public QObject, public maki::ILogging
{
public:
  /**
   * @brief Constructs a new Logger object.
   *
   * @param parent Pointer to the parent QObject.
   */
  Logger(QObject* parent = nullptr);

  /**
   * @brief Registers a plugin with a callback function.
   *
   * @param name Name of the plugin.
   * @param callback Reference to the log function callback.
   */
  void registerPlugin(const QString& name, maki::LogFunction& callback) override;

private:
  QMap<QString, maki::LogFunction> mPlugins;  /// Map of plugin names to log functions.
};
