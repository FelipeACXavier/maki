#pragma once

#include <QDir>
#include <QJsonArray>
#include <QWidget>

#include "generator_plugin.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "result.h"

class QComboBox;
class HostServices;

class Manifest
{
public:
  QString id;
  QString name;
  QString version;
  QString entryPoint;
  QString icon;
  QString path;
  QStringList libs;

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

class PluginManager : public QObject
{
  Q_OBJECT
public:
  struct Plugin
  {
    maki::IGeneratorPlugin* plugin;
    Manifest manifest;
  };

  PluginManager(QObject* parent = nullptr);

  virtual ~PluginManager();

  VoidResult start(QMenu* menu, QComboBox* comboBox, HostServices* services);

  maki::IGeneratorPlugin* currentPlugin() const;
  maki::IGeneratorPlugin* pluginByLanguage(const QString& language) const;

private:
  maki::IGeneratorPlugin* mPlugin;
  QVector<Plugin> mPlugins;

  VoidResult loadPlugin(const Manifest& path, QMenu* menu, QComboBox* comboBox, HostServices* services);
  bool setPlugin(const QString& language);

  Result<Manifest> getPluginManifest(const QDir& path) const;
  VoidResult loadPluginLibraryDir(const Manifest& manifest);
};
