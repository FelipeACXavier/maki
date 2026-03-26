// SettingsManager.cpp
#include "settings_manager.h"

#include <QFile>

#include "common/app_configs.h"
#include "logging.h"

#define LOAD_SETTING(MEMBER, FIELD, TYPE)                            \
  do                                                                 \
  {                                                                  \
    auto tmp##FIELD = mSettings.value("" #FIELD, MEMBER.FIELD);      \
    if (!tmp##FIELD.isValid())                                       \
    {                                                                \
      LOG_ERROR("Settings are corrupted field is invalid: " #FIELD); \
      return;                                                        \
    }                                                                \
    MEMBER.FIELD = tmp##FIELD.to##TYPE();                            \
  } while (false);

#define SAVE_SETTING(MEMBER, FIELD)              \
  do                                             \
  {                                              \
    mSettings.setValue("" #FIELD, MEMBER.FIELD); \
  } while (false);

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
    , mSettings(Config::ORGANIZATION_NAME, Config::APPLICATION_NAME)
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
  if (!QFile(mSettings.fileName()).exists())
    return;

  LOG_DEBUG("Loading from: %s", qPrintable(mSettings.fileName()));
  mSettings.beginGroup("General");
  LOAD_SETTING(mGeneral, restoreLastSession, Bool);
  LOAD_SETTING(mGeneral, restoreLastSession, Bool);
  LOAD_SETTING(mGeneral, autosaveEnabled, Bool);
  LOAD_SETTING(mGeneral, autosaveIntervalMinutes, Int);
  LOAD_SETTING(mGeneral, confirmOnCloseWithExecution, Bool);
  LOAD_SETTING(mGeneral, enableDebugLogs, Bool);
  LOAD_SETTING(mGeneral, recentHistorySize, Int);
  LOAD_SETTING(mGeneral, showWelcomeMessage, Bool);

  mSettings.beginGroup("RecentFiles");
  const QStringList recentFiles = mSettings.childKeys();
  mGeneral.recentFiles.resize(recentFiles.size());
  for (const auto& key : recentFiles)
  {
    bool ok = false;
    auto index = key.toInt(&ok);
    auto file = mSettings.value(key, "").toString();
    if (!ok || file.isEmpty())
    {
      LOG_ERROR("Corrupted save");
      mGeneral.recentFiles.clear();
      break;
    }

    mGeneral.recentFiles[index] = file;
  }
  mSettings.endGroup();
  mSettings.endGroup();

  mSettings.beginGroup("Appearance");
  LOAD_SETTING(mAppearance, theme, String);
  LOAD_SETTING(mAppearance, uiScalePercent, Int);
  LOAD_SETTING(mAppearance, showCanvasGrid, Bool);
  LOAD_SETTING(mAppearance, nativeMenuBar, Bool);
  LOAD_SETTING(mAppearance, nodeCornerRadius, Int);
  mSettings.endGroup();

  mSettings.beginGroup("Generation");
  LOAD_SETTING(mGeneration, generationDir, String);
  LOAD_SETTING(mGeneration, pluginSearchPaths, StringList);
  mSettings.endGroup();
}

void SettingsManager::save()
{
  mSettings.beginGroup("General");
  SAVE_SETTING(mGeneral, restoreLastSession);
  SAVE_SETTING(mGeneral, autosaveEnabled);
  SAVE_SETTING(mGeneral, autosaveIntervalMinutes);
  SAVE_SETTING(mGeneral, confirmOnCloseWithExecution);
  SAVE_SETTING(mGeneral, enableDebugLogs);
  SAVE_SETTING(mGeneral, recentHistorySize);
  SAVE_SETTING(mGeneral, showWelcomeMessage);
  mSettings.beginGroup("RecentFiles");
  for (int i = 0; i < mGeneral.recentFiles.size(); ++i)
  {
    mSettings.setValue(QString("%1").arg(i), mGeneral.recentFiles.at(i));
  }
  mSettings.endGroup();  // RecentFiles
  mSettings.endGroup();  // General

  mSettings.beginGroup("Appearance");
  SAVE_SETTING(mAppearance, theme);
  SAVE_SETTING(mAppearance, uiScalePercent);
  SAVE_SETTING(mAppearance, showCanvasGrid);
  SAVE_SETTING(mAppearance, nativeMenuBar);
  SAVE_SETTING(mAppearance, nodeCornerRadius);
  mSettings.endGroup();  // Appearance

  mSettings.beginGroup("Generation");
  SAVE_SETTING(mGeneration, generationDir);
  SAVE_SETTING(mGeneration, pluginSearchPaths);
  mSettings.endGroup();

  mSettings.beginGroup("Plugins");
  for (const auto& plugin : plugins())
  {
    mSettings.beginGroup(plugin.name);
    mSettings.setValue("name", plugin.name);
    mSettings.setValue("version", plugin.version.toString());
    mSettings.setValue("enabled", plugin.enabled);
    for (int i = 0; i < plugin.settings.size(); ++i)
    {
      const auto setting = plugin.settings.at(i);
      mSettings.beginGroup(setting.getKey());
      mSettings.setValue("key", setting.getKey());
      mSettings.setValue("index", i);
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
      mSettings.endGroup();  // setting.key
    }
    mSettings.endGroup();  // plugin.name
  }
  mSettings.endGroup();  // Plugins

  mSettings.sync();
}

void SettingsManager::setGeneral(const GeneralSettings& s)
{
  bool changed = mGeneral != s;

  mGeneral = s;
  save();

  if (changed)
    emit settingsChanged();
}

void SettingsManager::setAppearance(const AppearanceSettings& s)
{
  bool changed = (mAppearance != s);

  mAppearance = s;
  save();

  if (changed)
    emit themeChanged(mAppearance.theme, mAvailableThemes);
}

void SettingsManager::setGeneration(const GenerationSettings& s)
{
  bool changed = mGeneration != s;

  mGeneration = s;
  save();

  if (changed)
    emit settingsChanged();
}

void SettingsManager::setPlugins(const QVector<PluginInfo>& s)
{
  mPluginSettings = s;
  save();
}

void SettingsManager::addRecentFile(const QString& s)
{
  // Check if the files was already added
  if (mGeneral.recentFiles.contains(s))
    return;

  mGeneral.recentFiles.push_front(s);
  if (mGeneral.recentFiles.size() >= mGeneral.recentHistorySize)
    mGeneral.recentFiles.pop_back();

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

      LOG_INFO("Adding setting %s to index %d of %d", qPrintable(setting.getKey()), index, info.settings.size());
      info.settings[index] = setting;
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