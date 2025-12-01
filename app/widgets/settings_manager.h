// SettingsManager.h
#pragma once

#include <QApplication>
#include <QObject>
#include <QSettings>

#include "common/theme.h"

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
};

struct AppearanceSettings
{
  QString theme = "system";
  int uiScalePercent = 100;  // 100%, 110%, ...
  bool showCanvasGrid = true;
  int nodeCornerRadius = 8;
};

struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath();
  QStringList pluginSearchPaths;
};

class SettingsManager : public QObject
{
  Q_OBJECT
public:
  SettingsManager(QObject* parent = nullptr);

  GeneralSettings general() const;
  AppearanceSettings appearance() const;
  GenerationSettings generation() const;

  void setGeneral(const GeneralSettings& s);
  void setAppearance(const AppearanceSettings& s);
  void setGeneration(const GenerationSettings& s);

  void load();
  void save();

  QList<Config::ThemeInfo> availableThemes() const;

signals:
  void themeChanged(const QString& theme, const QList<Config::ThemeInfo>& availableThemes);

private:
  QSettings mSettings;
  GeneralSettings mGeneral;
  AppearanceSettings mAppearance;
  GenerationSettings mGeneration;

  QList<Config::ThemeInfo> mAvailableThemes;
};
