#pragma once

#include <QDir>
#include <QJsonArray>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QString>

#include "ihost_services.h"
#include "isettings.h"
#include "pipeline_action.h"
#include "plugin_version.h"
#include "result.h"

namespace maki
{

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
  QString iconPath;    /// Icon associated with the plugin.
  QString pluginRoot;  /// Path to the plugin.
  QStringList libs;    /// List of libraries required by the plugin.
  Environment env;
  QVector<InstallStep> installationSteps;

  QString pluginPath() const
  {
    return pluginRoot + "/" + entryPoint;
  }

  static QString resolveAssetPath(const QDir& assetDir, const QString& path)
  {
    if (path.isEmpty())
      return QString();

    if (QDir::isAbsolutePath(path))
      return QFileInfo(path).isFile() ? QDir::cleanPath(path) : QString{};

    QDir pluginDir = assetDir;
    pluginDir.cdUp();

    QDir pluginsDir = pluginDir;
    pluginsDir.cdUp();

    const QStringList candidates = {pluginDir.filePath(path), pluginsDir.filePath(path), assetDir.filePath(path)};
    for (const auto& candidate : candidates)
    {
      const QString resolved = QDir::cleanPath(candidate);

      if (QFileInfo(resolved).isFile())
        return resolved;
    }

    return QString();
  }

  /**
   * @brief Creates a Manifest object from JSON data.
   *
   * @param rootPath The path to the manifest file.
   * @param data The JSON data containing the manifest information.
   * @return A Manifest object populated with the data.
   */
  static Manifest fromJson(const QString& rootPath, const QJsonObject& data)
  {
    Manifest manifest;

    if (data.contains("id"))
      manifest.id = data["id"].toString();
    if (data.contains("name"))
      manifest.name = data["name"].toString();
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
      manifest.iconPath = Manifest::resolveAssetPath(rootPath, data["icon"].toString());

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

    manifest.pluginRoot = rootPath;

    return manifest;
  }
};

class IPlugin
{
public:
  virtual ~IPlugin() = default;

  // =================================================================
  // Purely virtual methods, must be implemented by every plugin
  virtual bool setup() = 0;
  virtual bool tearDown() = 0;
  virtual void setHostServices(maki::IHostServices* services) = 0;
  virtual QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() = 0;

  // =================================================================
  // Generic setters and getters, shall not be overloaded
  void setManifest(const maki::Manifest& manifest)
  {
    mManifest = manifest;
  }

  maki::Manifest manifest() const
  {
    return mManifest;
  }

  QVector<maki::SettingField> settings() const
  {
    return mSettings;
  }

  QDir assetDir() const
  {
    return mAssetDir.value_or(QDir());
  }

  void setAssetDir(const QDir& dir)
  {
    mAssetDir = dir;
  }

  QString languageName() const
  {
    return mManifest.name;
  }

  PluginVersion version() const
  {
    return maki::PluginVersion::fromString(mManifest.version);
  }

  void settingsChanged(const QVector<maki::SettingField>& settings)
  {
    for (auto& setting : mSettings)
      for (const auto& incoming : settings)
      {
        if (setting.getKey() != incoming.getKey())
          continue;

        setting.setValue(incoming.getValue());
        return;
      }
  }

protected:
  maki::Manifest mManifest = {};               // Plugin manifest, generic
  maki::IHostServices* mServices = nullptr;    // Pointer to the MAKI services
  QVector<maki::SettingField> mSettings = {};  // Collection of plugin settings if they exist
  std::optional<QDir> mAssetDir;               // If the plugin contains assets, they can be found here

  /**
   * @brief Gets the setting via its key
   *
   * @param key QString containing the key of the desired setting
   * @return std::optional<maki::SettingField> with the setting field if available and nullopt of failure
   */
  std::optional<maki::SettingField> getSetting(const QString& key) const
  {
    for (const auto& s : mSettings)
      if (s.getKey() == key)
        return s;

    return std::nullopt;
  }

  // =================================================================
  // Optional methods, may be overloaded if needed
  virtual void buildSettings()
  {
  }
};

}  // namespace maki

Q_DECLARE_METATYPE(maki::PluginVersion);

#define MAKI_GENERATORPLUGIN_IID "com.maki.GeneratorPlugin/1.0"
Q_DECLARE_INTERFACE(maki::IPlugin, MAKI_GENERATORPLUGIN_IID)
