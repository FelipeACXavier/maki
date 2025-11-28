// SettingsManager.cpp
#include "settings_manager.h"

#include "common/app_configs.h"

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
    ,
    // organisation & app name: adjust to yours
    mSettings(Config::ORGANIZATION_NAME, Config::APPLICATION_NAME)
{
  mAvailableThemes = Config::discoverThemes();
  load();
}

GeneralSettings SettingsManager::general() const
{
  return mGeneral;
}

AppearanceSettings SettingsManager::appearance() const
{
  return mAppearance;
}

QList<Config::ThemeInfo> SettingsManager::availableThemes() const
{
  return mAvailableThemes;
}

void SettingsManager::load()
{
  mSettings.beginGroup("General");
  mGeneral.restoreLastSession = mSettings.value("restoreLastSession", mGeneral.restoreLastSession).toBool();
  mGeneral.autosaveEnabled = mSettings.value("autosaveEnabled", mGeneral.autosaveEnabled).toBool();
  mGeneral.autosaveIntervalMinutes = mSettings.value("autosaveIntervalMinutes", mGeneral.autosaveIntervalMinutes).toInt();
  mGeneral.confirmOnCloseWithExecution = mSettings.value("confirmOnCloseWithExecution", mGeneral.confirmOnCloseWithExecution).toBool();
  mGeneral.enableDebugLogs = mSettings.value("enableDebugLogs", mGeneral.enableDebugLogs).toBool();
  mSettings.endGroup();

  mSettings.beginGroup("Appearance");
  mAppearance.theme = mSettings.value("theme", mAppearance.theme).toString();
  mAppearance.uiScalePercent = mSettings.value("uiScalePercent", mAppearance.uiScalePercent).toInt();
  mAppearance.showCanvasGrid = mSettings.value("showCanvasGrid", mAppearance.showCanvasGrid).toBool();
  mAppearance.nodeCornerRadius = mSettings.value("nodeCornerRadius", mAppearance.nodeCornerRadius).toInt();
  mSettings.endGroup();
}

void SettingsManager::save()
{
  mSettings.beginGroup("General");
  mSettings.setValue("restoreLastSession", mGeneral.restoreLastSession);
  mSettings.setValue("autosaveEnabled", mGeneral.autosaveEnabled);
  mSettings.setValue("autosaveIntervalMinutes", mGeneral.autosaveIntervalMinutes);
  mSettings.setValue("confirmOnCloseWithExecution", mGeneral.confirmOnCloseWithExecution);
  mSettings.setValue("enableDebugLogs", mGeneral.enableDebugLogs);
  mSettings.endGroup();

  mSettings.beginGroup("Appearance");
  mSettings.setValue("theme", mAppearance.theme);
  mSettings.setValue("uiScalePercent", mAppearance.uiScalePercent);
  mSettings.setValue("showCanvasGrid", mAppearance.showCanvasGrid);
  mSettings.setValue("nodeCornerRadius", mAppearance.nodeCornerRadius);
  mSettings.endGroup();

  mSettings.sync();
}

void SettingsManager::setGeneral(const GeneralSettings& s)
{
  mGeneral = s;
  save();
}

void SettingsManager::setAppearance(const AppearanceSettings& s)
{
  bool changed = (s.theme != mAppearance.theme);
  mAppearance = s;
  save();

  if (changed)
    emit themeChanged(mAppearance.theme, mAvailableThemes);
}
