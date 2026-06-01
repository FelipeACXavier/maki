#pragma once

#include <QDir>
#include <QGraphicsItem>
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
  static Manifest fromJson(const QString& path, const QJsonObject& data)
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

  QString iconPath() const
  {
    return QDir(path).dirName() + "/" + icon;
  }
};

class IPlugin
{
public:
  virtual ~IPlugin() = default;

  virtual bool setup() = 0;
  virtual bool tearDown() = 0;
  virtual void setHostServices(maki::IHostServices* services) = 0;
  virtual void setAssetDir(const QDir& dir) = 0;

  virtual void settingsChanged(const QVector<maki::SettingField>& settings) = 0;

  virtual QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() = 0;

  virtual void setManifest(const maki::Manifest& manifest)
  {
    mManifest = manifest;
  }

  virtual maki::Manifest manifest() const
  {
    return mManifest;
  }

  virtual QString languageName() const
  {
    return mManifest.name;
  }

  virtual PluginVersion version() const
  {
    return maki::PluginVersion::fromString(mManifest.version);
  }

protected:
  maki::Manifest mManifest;
};
}  // namespace maki

Q_DECLARE_METATYPE(maki::PluginVersion);

#define MAKI_GENERATORPLUGIN_IID "com.maki.GeneratorPlugin/1.0"
Q_DECLARE_INTERFACE(maki::IPlugin, MAKI_GENERATORPLUGIN_IID)
