#pragma once

#include <QDir>
#include <QJsonArray>
#include <QPluginLoader>
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
 * @brief Represents a manifest file for a plugin.
 */
class Manifest
{
public:
  struct InstallStep
  {
    QString command;
    QStringList args;
  };

  struct Environment
  {
    QString path;
  };

  QString id;          /// Unique identifier of the plugin.
  QString name;        /// Name of the plugin.
  QString version;     /// Version of the plugin.
  QString entryPoint;  /// Entry point of the plugin.
  QString icon;        /// Icon associated with the plugin.
  QString path;        /// Path to the plugin.
  QStringList libs;    /// List of libraries required by the plugin.
  Environment env;
  QVector<InstallStep> installationSteps;

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

    for (const auto& installation : data["installation"].toArray())
    {
      const auto obj = installation.toObject();
      if (!obj.contains("command"))
        continue;

      InstallStep step;
      step.command = obj["command"].toString();
      if (obj.contains("arguments"))
      {
        QStringList args;
        for (const auto& arg : obj["arguments"].toArray())
          args << arg.toString();

        step.args = args;
      }

      manifest.installationSteps.push_back(step);
    }

    if (data.contains("env"))
    {
      Environment env;
      const auto obj = data["env"].toObject();
      if (obj.contains("PATH"))
        env.path = obj["PATH"].toString();

      manifest.env = env;
    }

    manifest.path = path;

    return manifest;
  }

  QString pluginPath() const
  {
    return path + "/" + entryPoint;
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
    maki::IPlugin* plugin;  /// Pointer to the generator plugin.
    Manifest manifest;      /// Manifest of the plugin.
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
  VoidResult loadPlugin(const QDir& pluginDir, const Manifest& path, HostServices* services, PluginSettings::Status status);

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
  Result<Manifest> getPluginManifest(const QDir& path) const;

  /**
   * @brief Loads a plugin library directory.
   *
   * @param manifest The Manifest object containing the plugin information.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult loadPluginLibraryDir(const Manifest& manifest);

  /**
   * @brief Run the plugin installation steps
   *
   * @param manifest The Manifest object containing the plugin information.
   * @return A VoidResult indicating success or failure.
   */
  VoidResult installPlugin(const Manifest& manifest);

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
