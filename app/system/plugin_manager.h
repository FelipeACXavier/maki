#pragma once

#include <QDir>
#include <QJsonArray>
#include <QPluginLoader>
#include <QWidget>

#include "generator_plugin.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "result.h"
#include "widgets/settings_manager.h"

class QComboBox;
class HostServices;

/**
 * @brief Represents a manifest file for a plugin.
 */
class Manifest
{
public:
  QString id;          /// Unique identifier of the plugin.
  QString name;        /// Name of the plugin.
  QString version;     /// Version of the plugin.
  QString entryPoint;  /// Entry point of the plugin.
  QString icon;        /// Icon associated with the plugin.
  QString path;        /// Path to the plugin.
  QStringList libs;    /// List of libraries required by the plugin.

  /**
   * @brief Creates a Manifest object from JSON data.
   *
   * @param path The path to the manifest file.
   * @param data The JSON data containing the manifest information.
   * @return A Manifest object populated with the data.
   */
  static Manifest fromJson(const QString& path, const JSON& data)
  {
    Manifest manifest;

    if (data.contains(ConfigKeys::ID))
      manifest.id = data[ConfigKeys::ID].toString();
    if (data.contains(ConfigKeys::NAME))
      manifest.name = data[ConfigKeys::NAME].toString();
    if (data.contains("version"))
      manifest.version = data["version"].toString();
#ifdef Q_OS_WIN
    if (data.contains("entryPointDLL"))
      manifest.entryPoint = data["entryPointDLL"].toString();
#else
    if (data.contains("entryPointSO"))
      manifest.entryPoint = data["entryPointSO"].toString();
#endif
    if (data.contains("icon"))
      manifest.icon = data["icon"].toString();

    for (const auto& argument : data["libraries"].toArray())
      manifest.libs.push_back(argument.toString());

    manifest.path = path + "/" + manifest.entryPoint;

    return manifest;
  }
};

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
    maki::IGeneratorPlugin* plugin;  /// Pointer to the generator plugin.
    Manifest manifest;               /// Manifest of the plugin.
    QAction* action;
    int comboIndex;
  };

  /**
   * @brief Constructs a new PluginManager object.
   *
   * @param parent The parent QObject.
   */
  PluginManager(QObject* parent = nullptr);

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
  VoidResult start(const PluginSettings& settings, QMenu* menu, QComboBox* comboBox, HostServices* services);

  /**
   * @brief Retrieves the currently selected plugin.
   *
   * @return The current maki::IGeneratorPlugin pointer.
   */
  maki::IGeneratorPlugin* currentPlugin() const;

  /**
   * @brief Retrieves a plugin by its language.
   *
   * @param language The language of the plugin to retrieve.
   * @return The corresponding maki::IGeneratorPlugin pointer, or nullptr if not found.
   */
  maki::IGeneratorPlugin* pluginByLanguage(const QString& language) const;

  /**
   * @brief Updates the plugins when a setting changes
   */
  void settingsChanged(const PluginSettings& settings, QMenu* menu, QComboBox* comboBox, HostServices* services);

private:
  maki::IGeneratorPlugin* mPlugin;  /// Pointer to the currently selected plugin.
  QVector<Plugin> mPlugins;         /// List of all plugins managed by this manager.

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
  VoidResult loadPlugin(const QDir& pluginDir, const Manifest& path, QMenu* menu, QComboBox* comboBox, HostServices* services);

  /**
   * @brief Sets the current plugin based on a language identifier.
   *
   * @param language The language identifier for the plugin to set.
   * @return true if the plugin was successfully set, false otherwise.
   */
  bool setPlugin(const QString& language);

  /**
   * @brief Sets the current plugin based on a language identifier and updates
   * the plugin selection UI element.
   *
   * @param comboBox The UI element to be updated
   * @param language The language identifier for the plugin to set.
   * @return true if the plugin was successfully set, false otherwise.
   */
  bool selectPlugin(QComboBox* comboBox, const QString& language);

  /**
   * @brief Retrieves the manifest of a plugin from a directory.
   *
   * @param path The QDir object representing the plugin directory.
   * @return A Result<Manifest> containing the manifest or an error if not found.
   */
  Result<Manifest> getPluginManifest(const QDir& path) const;

  /**
   * @brief Loads a plugin library directory.
   *
   * @param manifest The Manifest object containing the plugin information.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult loadPluginLibraryDir(const Manifest& manifest);

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
  VoidResult deregisterPlugin(const Plugin& plugin, QMenu* menu, QComboBox* comboBox);
};
