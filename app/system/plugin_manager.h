#pragma once

#include <QDir>
#include <QJsonArray>
#include <QPluginLoader>
#include <QSysInfo>
#include <QWidget>

#include "compiler/pipeline.h"
#include "iplugin.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "result.h"
#include "widgets/settings_manager.h"

class QComboBox;
class HostServices;
namespace maki
{
class PipelineActionRegistry;
}

/**
 * @brief Manages plugins and their associated functionality.
 */
class PluginManager : public QObject
{
  Q_OBJECT

public:
  struct Plugin
  {
    QSharedPointer<QPluginLoader> loader;
    maki::IPlugin* plugin;  /// Pointer to the generator plugin.
  };

  /**
   * @brief Constructs a new PluginManager object.
   *
   * @param parent The parent QObject.
   */
  PluginManager(maki::PipelineActionRegistry* registry, Pipeline* pipeline, QObject* parent = nullptr);

  /**
   * @brief Destructs the PluginManager object.
   */
  virtual ~PluginManager();

  /**
   * @brief Starts the plugin manager and populates the menu, combo box, and services.
   *
   * @param menu The QMenu to populate with plugins.
   * @param comboBox The QComboBox to populate with plugin options.
   * @param services The HostServices object for plugin interaction.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult start(const PluginSettings& settings, HostServices* services);

  /**
   * @brief Retrieves the currently selected plugin.
   *
   * @return The current maki::IPlugin pointer.
   */
  maki::IPlugin* currentPlugin() const;

  /**
   * @brief Retrieves a plugin by its language.
   *
   * @param language The language of the plugin to retrieve.
   * @return The corresponding maki::IPlugin pointer, or nullptr if not found.
   */
  maki::IPlugin* pluginByLanguage(const QString& language) const;

  /**
   * @brief Updates the plugins when a setting changes
   */
  void settingsChanged(const PluginSettings& settings, HostServices* services);

  /**
   * @brief Reloads a given plugin
   *
   * @param pluginName The name of the plugin to be reloaded.
   * @param menu The QMenu to populate with plugins.
   * @param comboBox The QComboBox to populate with plugin options.
   * @param services The HostServices object for plugin interaction.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult reloadPlugin(const QString& pluginName, HostServices* services);

signals:

  void pluginAdded(const Plugin& plugin) const;

private:
  maki::IPlugin* mPlugin;    /// Pointer to the currently selected plugin.
  QVector<Plugin> mPlugins;  /// List of all plugins managed by this manager.
  maki::PipelineActionRegistry* mRegistry;

  Pipeline::Info mInfo;  /// Holds information regarding the current generation pipeline
  Pipeline* mPipeline;   /// Pipeline responsible for installation processes
  QString mProgressId;   /// Holds the id of the progress widget
  bool mIsRunning;

  /**
   * @brief Loads a plugin from a directory and populates the menu, combo box, and services.
   *
   * @param pluginDir The QDir object representing the plugin directory.
   * @param path The Manifest object containing the plugin information.
   * @param menu The QMenu to populate with plugins.
   * @param comboBox The QComboBox to populate with plugin options.
   * @param services The HostServices object for plugin interaction.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult loadPlugin(const QDir& pluginDir, const maki::Manifest& path, HostServices* services, PluginSettings::Status status);

  /**
   * @brief Sets the current plugin based on a language identifier.
   *
   * @param language The language identifier for the plugin to set.
   * @return true if the plugin was successfully set, false otherwise.
   */
  bool setPlugin(const QString& language);

  /**
   * @brief Retrieves the manifest of a plugin from a directory.
   *
   * @param path The QDir object representing the plugin directory.
   * @return A Result<Manifest> containing the manifest or an error if not found.
   */
  Result<maki::Manifest> getPluginManifest(const QDir& path) const;

  /**
   * @brief Loads a plugin library directory.
   *
   * @param manifest The Manifest object containing the plugin information.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult loadPluginLibraryDir(const maki::Manifest& manifest);

  /**
   * @brief Run the plugin installation steps
   *
   * @param manifest The Manifest object containing the plugin information.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult installPlugin(const maki::Manifest& manifest);

  /**
   * @brief Retrieves the index of a plugin in the manager's list by name.
   *
   * @param pluginName The name of the plugin to find.
   * @return The index of the plugin if found; -1 otherwise.
   */
  int getPluginIndex(const QString& pluginName) const;

  /**
   * @brief Deregisters a plugin from the manager and updates the menu and combo box.
   *
   * @param plugin The Plugin object to deregister.
   * @param menu The QMenu to update.
   * @param comboBox The QComboBox to update.
   */
  VoidResult deregisterPlugin(const Plugin& plugin);

  QWidget* progressContent();  /// Creates and returns the progress content widget.
};
