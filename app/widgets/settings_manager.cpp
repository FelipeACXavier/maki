// SettingsManager.cpp
#include "settings_manager.h"

#include "common/app_configs.h"
#include "logging.h"

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

GenerationSettings SettingsManager::generation() const
{
  return mGeneration;
}

QVector<PluginInfo> SettingsManager::plugins() const
{
  return mPluginSettings;
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
  mAppearance.nativeMenuBar = mSettings.value("nativeMenuBar", mAppearance.nativeMenuBar).toBool();
  mAppearance.nodeCornerRadius = mSettings.value("nodeCornerRadius", mAppearance.nodeCornerRadius).toInt();
  mSettings.endGroup();

  mSettings.beginGroup("Generation");
  mGeneration.generationDir = mSettings.value("generationDir", mGeneration.generationDir).toString();
  mGeneration.pluginSearchPaths = mSettings.value("pluginSearchPaths").toStringList();
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
  mSettings.setValue("nativeMenuBar", mAppearance.nativeMenuBar);
  mSettings.setValue("nodeCornerRadius", mAppearance.nodeCornerRadius);
  mSettings.endGroup();

  mSettings.beginGroup("Generation");
  mSettings.setValue("generationDir", mGeneration.generationDir);
  mSettings.setValue("pluginSearchPaths", mGeneration.pluginSearchPaths);
  mSettings.endGroup();

  mSettings.beginGroup("Plugins");
  for (const auto& plugin : plugins())
  {
    mSettings.beginGroup(plugin.name);
    mSettings.setValue("name", plugin.name);
    mSettings.setValue("version", plugin.version.toString());
    mSettings.setValue("enabled", plugin.enabled);
    int index = 0;
    for (const auto& setting : plugin.settings)
    {
      mSettings.beginGroup(setting.getKey());
      mSettings.setValue("key", setting.getKey());
      mSettings.setValue("index", index);
      mSettings.setValue("label", setting.getLabel());
      mSettings.setValue("description", setting.getDescription());
      mSettings.setValue("value", setting.getValue());
      mSettings.setValue("defaultValue", setting.getDefaultValue());
      mSettings.setValue("type", Types::PropertyTypesToString(setting.getType()));
      if (setting.getType() == Types::PropertyTypes::INTEGER)
      {
        if (setting.getMetadata().contains("min"))
          mSettings.setValue("min", setting.getMetadata()["min"]);
        if (setting.getMetadata().contains("max"))
          mSettings.setValue("max", setting.getMetadata()["max"]);
      }
      index++;
      mSettings.endGroup();  // setting.key
    }
    mSettings.endGroup();  // plugin.name
  }
  mSettings.endGroup();  // Plugins

  mSettings.sync();
}

void SettingsManager::setGeneral(const GeneralSettings& s)
{
  mGeneral = s;
  save();
}

void SettingsManager::setAppearance(const AppearanceSettings& s)
{
  bool changed = (s.theme != mAppearance.theme) ||
                 (s.nativeMenuBar != mAppearance.nativeMenuBar) ||
                 (s.showCanvasGrid != mAppearance.showCanvasGrid);

  mAppearance = s;
  save();

  if (changed)
    emit themeChanged(mAppearance.theme, mAvailableThemes);
}

void SettingsManager::setGeneration(const GenerationSettings& s)
{
  mGeneration = s;
  save();
}

void SettingsManager::setPlugins(const QVector<PluginInfo>& s)
{
  mPluginSettings = s;
  save();
}

VoidResult SettingsManager::registerSettings(const QString& id, const maki::PluginVersion version, const QVector<maki::SettingField>& settings)
{
  // Since the plugin is registered, we try to load the save settings
  bool exists = false;

  mSettings.beginGroup("Plugins");
  const QStringList pluginGroups = mSettings.childGroups();
  for (const QString& pg : pluginGroups)
  {
    if (pg != id)
      continue;

    mSettings.beginGroup(pg);

    PluginInfo info;
    info.name = mSettings.value("name", info.name).toString();
    info.enabled = mSettings.value("enabled", info.enabled).toBool();
    info.version = maki::PluginVersion::fromString(mSettings.value("version", "").toString());

    // We not need to check to see if the versions match
    if (info.version != version)
    {
      // TODO(felaze): Prompt user
      LOG_WARNING("Versions are different. Saved: %s Registering: %s",
                  qPrintable(info.version.toString()), qPrintable(version.toString()));
    }

    exists = true;

    const QStringList settingGroups = mSettings.childGroups();
    info.settings.resize(settingGroups.size());
    for (const QString& sg : settingGroups)
    {
      mSettings.beginGroup(sg);

      int index = mSettings.value("index", 0).toInt();

      maki::SettingField setting;
      setting.setKey(mSettings.value("key", setting.getKey()).toString());
      setting.setLabel(mSettings.value("label", setting.getLabel()).toString());
      setting.setDescription(mSettings.value("description", setting.getDescription()).toString());
      setting.setValue(mSettings.value("value", setting.getValue()));
      setting.setDefaultValue(mSettings.value("defaultValue", setting.getDefaultValue()));
      setting.setType(Types::StringToPropertyTypes(mSettings.value("type", "").toString()));

      if (setting.getType() == Types::PropertyTypes::INTEGER)
      {
        QVariantMap metdata;
        auto min = mSettings.value("min");
        if (min.isValid())
          metdata["min"] = min.toInt();
        auto max = mSettings.value("max");
        if (min.isValid())
          metdata["max"] = max.toInt();

        setting.setMetadata(metdata);
      }

      info.settings.insert(index, setting);
      mSettings.endGroup();  // setting.name
    }
    mPluginSettings.push_back(info);
    mSettings.endGroup();  // plugin.name
  }
  mSettings.endGroup();

  if (!exists)
    mPluginSettings.append({id, true, version, settings});

  return VoidResult();
}