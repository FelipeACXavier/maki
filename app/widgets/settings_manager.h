// SettingsManager.h
#pragma once

#include <QApplication>
#include <QObject>
#include <QSettings>
#include <oclero/qlementine.hpp>

#include "app_paths.h"
#include "common/theme.h"
#include "generator_plugin.h"
#include "isettings.h"
#include "style_helpers.h"

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
  QString lastOpenFileDir = "";
  QString language = "en_US";

  bool operator!=(const GeneralSettings& s)
  {
    return restoreLastSession != s.restoreLastSession ||
           autosaveEnabled != s.autosaveEnabled ||
           autosaveIntervalMinutes != s.autosaveIntervalMinutes ||
           confirmOnCloseWithExecution != s.confirmOnCloseWithExecution ||
           enableDebugLogs != s.enableDebugLogs ||
           recentHistorySize != s.recentHistorySize ||
           showWelcomeMessage != s.showWelcomeMessage ||
           language != s.language;
  }
};

struct AppearanceSettings
{
  QString theme = "Dark";
  int uiScalePercent = 100;  // 100%, 110%, ...
  bool showCanvasGrid = true;
  bool nativeMenuBar = true;
  int nodeCornerRadius = 8;
  maki::ThemeVars themeVars;

  bool operator!=(const AppearanceSettings& s)
  {
    return theme != s.theme ||
           uiScalePercent != s.uiScalePercent ||
           showCanvasGrid != s.showCanvasGrid ||
           nativeMenuBar != s.nativeMenuBar ||
           nodeCornerRadius != s.nodeCornerRadius ||
           themeVars != s.themeVars;
  }
};

struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath() + "/generation";
  QStringList pluginSearchPaths = {AppPaths::pluginSearchPaths()};

  bool operator!=(const GenerationSettings& s)
  {
    return generationDir != s.generationDir ||
           pluginSearchPaths != s.pluginSearchPaths;
  }
};

struct PluginInfo
{
  QString name;
  bool enabled;
  maki::PluginVersion version;
  QVector<maki::SettingField> settings;
  maki::SettingsFunction callback;
};

class SettingsManager : public QObject, public maki::ISettings
{
  Q_OBJECT
public:
  SettingsManager(oclero::qlementine::ThemeManager* themeManager, QObject* parent = nullptr);

  GeneralSettings general() const;
  AppearanceSettings appearance() const;
  GenerationSettings generation() const;
  QVector<PluginInfo> plugins() const;

  void setGeneral(const GeneralSettings& s);
  void setAppearance(const AppearanceSettings& s);
  void setGeneration(const GenerationSettings& s);
  void setPlugins(const QVector<PluginInfo>& s);

  QVector<maki::SettingField> getPluginSettings(const QString& id) const override;
  VoidResult registerSettings(const QString& id, const maki::PluginVersion version,
                              const QVector<maki::SettingField>& settings,
                              maki::SettingsFunction callback) override;

  void load();
  void save();

  std::vector<oclero::qlementine::Theme> availableThemes() const;

signals:
  void themeChanged(const QString& theme);
  void settingsChanged();

public slots:
  void addRecentFile(const QString& s);

private:
  QSettings mSettings;
  GeneralSettings mGeneral;
  AppearanceSettings mAppearance;
  GenerationSettings mGeneration;

  QVector<PluginInfo> mPluginSettings;

  oclero::qlementine::ThemeManager* mThemeManager;
};
