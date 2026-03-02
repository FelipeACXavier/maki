// SettingsManager.h
#pragma once

#include <QApplication>
#include <QObject>
#include <QSettings>

#include "common/theme.h"
#include "generator_plugin.h"
#include "isettings.h"

namespace Config
{
struct ThemeInfo;
}

struct GeneralSettings
{
  bool restoreLastSession = true;
  bool autosaveEnabled = true;
  int autosaveIntervalMinutes = 5;
  bool confirmOnCloseWithExecution = true;
  bool enableDebugLogs = true;
  int recentHistorySize = 10;
  bool showWelcomeMessage = true;
  QVector<QString> recentFiles = {};
};

struct AppearanceSettings
{
  QString theme = "Dark";
  int uiScalePercent = 100;  // 100%, 110%, ...
  bool showCanvasGrid = true;
  bool nativeMenuBar = true;
  int nodeCornerRadius = 8;
};

struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath();
  QStringList pluginSearchPaths = {
#ifdef Q_OS_WIN
      QCoreApplication::applicationDirPath() + "/plugins"
#else
      QCoreApplication::applicationDirPath() + "/../plugins"
#endif
  };
};

struct PluginInfo
{
  QString name;
  bool enabled;
  maki::PluginVersion version;
  QVector<maki::SettingField> settings;
};

class SettingsManager : public QObject, public maki::ISettings
{
  Q_OBJECT
public:
  SettingsManager(QObject* parent = nullptr);

  GeneralSettings general() const;
  AppearanceSettings appearance() const;
  GenerationSettings generation() const;
  QVector<PluginInfo> plugins() const;

  void setGeneral(const GeneralSettings& s);
  void setAppearance(const AppearanceSettings& s);
  void setGeneration(const GenerationSettings& s);
  void setPlugins(const QVector<PluginInfo>& s);

  VoidResult registerSettings(const QString& id, const maki::PluginVersion version, const QVector<maki::SettingField>& settings) override;

  void load();
  void save();

  QList<Config::ThemeInfo> availableThemes() const;

signals:
  void themeChanged(const QString& theme, const QList<Config::ThemeInfo>& availableThemes);

public slots:
  void addRecentFile(const QString& s);

private:
  QSettings mSettings;
  GeneralSettings mGeneral;
  AppearanceSettings mAppearance;
  GenerationSettings mGeneration;

  QVector<PluginInfo> mPluginSettings;

  QList<Config::ThemeInfo> mAvailableThemes;
};
