// SettingsManager.cpp
#include "settings_manager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "common/app_configs.h"
#include "isettings.h"
#include "logging.h"
#include "oclero/qlementine/style/Theme.hpp"
#include "result.h"
#include "widgets/dialogs/prompt.h"

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

SettingsManager::SettingsManager(oclero::qlementine::ThemeManager* themeManager, QObject* parent)
    : QObject(parent)
    , mSettings(  QSettings::IniFormat, QSettings::UserScope, Config::ORGANIZATION_NAME, Config::APPLICATION_NAME)
    , mThemeManager(themeManager)
{
  load();
  // Even if there is nothing, let's create the file on start up
  save();
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

PluginSettings SettingsManager::plugins() const
{
  return mPluginSettings;
}

std::vector<oclero::qlementine::Theme> SettingsManager::availableThemes() const
{
  return mThemeManager->themes();
}

Result<oclero::qlementine::Theme> SettingsManager::themeByName(const QString& themeName) const
{
  for (const auto& theme : availableThemes())
    if (theme.meta.name == themeName)
      return theme;

  return Result<oclero::qlementine::Theme>::Failed("No theme named: " + themeName.toStdString());
}

void SettingsManager::themeCreated(const QString& themePath)
{
  // Refresh the user themes
  mThemeManager->loadDirectory(themePath);
}

void SettingsManager::load()
{
  if (!QFile(mSettings.fileName()).exists())
  {
    LOG_DEBUG("No configuration file: %s", qPrintable(mSettings.fileName()));
    return;
  }
  else if (mSettings.allKeys().isEmpty())
  {
    LOG_DEBUG("Empty configuration: %s", qPrintable(mSettings.fileName()));
    return;
  }

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
  LOAD_SETTING(mGeneral, language, String);
  LOAD_SETTING(mGeneral, lastOpenFileDir, String);

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
  LOAD_SETTING(mAppearance, numberOfColumns, Int);
  LOAD_SETTING(mAppearance, edgeShape, Int);
  LOAD_SETTING(mAppearance, startLogFilterExpanded, Bool);
  mSettings.endGroup();

  mSettings.beginGroup("Generation");
  LOAD_SETTING(mGeneration, generationDir, String);
  LOAD_SETTING(mGeneration, pluginSearchPaths, StringList);
  mSettings.endGroup();

  mSettings.beginGroup("Plugins");
  LOAD_SETTING(mPluginSettings, defaultPlugin, String);
  const QStringList pluginGroups = mSettings.childGroups();
  for (const QString& pg : pluginGroups)
  {
    mSettings.beginGroup(pg);

    PluginInfo info;
    info.name = mSettings.value("name", info.name).toString();
    info.enabled = mSettings.value("enabled", info.enabled).toBool();
    info.version = maki::PluginVersion::fromString(mSettings.value("version", "").toString());
    info.icon = mSettings.value("icon", "").toString();

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

      LOG_TRACE("Adding setting %s to index %d of %d", qPrintable(setting.getKey()), index, info.settings.size());
      info.settings[index] = setting;
      mSettings.endGroup();  // setting.name
    }
    mPluginSettings.plugins.push_back(info);
    mSettings.endGroup();  // plugin.name
  }
  mSettings.endGroup();

  // Make sure the tmps are up to date
  mTmpGeneral = mGeneral;
  mTmpAppearance = mAppearance;
  mTmpGeneration = mGeneration;
  mTmpPluginSettings = mPluginSettings;
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
  SAVE_SETTING(mGeneral, language);
  SAVE_SETTING(mGeneral, lastOpenFileDir);
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
  SAVE_SETTING(mAppearance, startLogFilterExpanded);
  SAVE_SETTING(mAppearance, nativeMenuBar);
  SAVE_SETTING(mAppearance, nodeCornerRadius);
  SAVE_SETTING(mAppearance, numberOfColumns);
  SAVE_SETTING(mAppearance, edgeShape);
  mSettings.endGroup();  // Appearance

  mSettings.beginGroup("Generation");
  SAVE_SETTING(mGeneration, generationDir);
  SAVE_SETTING(mGeneration, pluginSearchPaths);
  mSettings.endGroup();

  mSettings.beginGroup("Plugins");
  SAVE_SETTING(mPluginSettings, defaultPlugin);
  for (const auto& plugin : plugins().plugins)
  {
    mSettings.beginGroup(plugin.name);
    mSettings.setValue("name", plugin.name);
    mSettings.setValue("version", plugin.version.toString());
    mSettings.setValue("enabled", plugin.enabled);
    mSettings.setValue("icon", plugin.icon);
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

void SettingsManager::applySettings()
{
  bool changed = false;
  if (mGeneral != mTmpGeneral)
  {
    mGeneral = mTmpGeneral;
    changed = true;
  }

  if (mGeneration != mTmpGeneration)
  {
    mGeneration = mTmpGeneration;
    changed = true;
  }

  if (mAppearance != mTmpAppearance)
  {
    mAppearance = mTmpAppearance;
    changed = true;
  }

  if (mPluginSettings != mTmpPluginSettings)
  {
    mPluginSettings = mTmpPluginSettings;
    changed = true;
  }

  save();

  if (changed)
    emit settingsChanged();
}

void SettingsManager::setGeneral(const GeneralSettings& s)
{
  mTmpGeneral = s;
}

void SettingsManager::setAppearance(const AppearanceSettings& s)
{
  mTmpAppearance = s;
}

void SettingsManager::setGeneration(const GenerationSettings& s)
{
  mTmpGeneration = s;
}

void SettingsManager::setPlugins(const PluginSettings& settings)
{
  mTmpPluginSettings = settings;
}

void SettingsManager::addRecentFile(const QString& s)
{
  // Check if the file was already added so we fix the order
  if (mTmpGeneral.recentFiles.contains(s))
    mTmpGeneral.recentFiles.removeIf([s](const QString& file) { return file == s; });

  mTmpGeneral.recentFiles.push_front(s);
  if (mTmpGeneral.recentFiles.size() >= mTmpGeneral.recentHistorySize)
    mTmpGeneral.recentFiles.pop_back();

  applySettings();
}

QVector<maki::SettingField> SettingsManager::getPluginSettings(const QString& id) const
{
  for (const auto& plugin : plugins().plugins)
  {
    if (plugin.name == id)
      return plugin.settings;
  }

  return {};
}

VoidResult SettingsManager::registerSettings(const QString& id, const maki::PluginVersion version, const QString& iconPath,
                                             const QVector<maki::SettingField>& settings)
{
  QFileInfo info(iconPath);
  auto fixedIconPath = info.dir().filePath(info.completeBaseName());

  // Since the plugin is registered, we try to load the save settings
  bool exists = false;
  for (auto& plugin : mPluginSettings.plugins)
  {
    // Lets look for the plugin in the settings
    if (plugin.name != id)
      continue;

    exists = true;

    if (plugin.version != version)
    {
      const auto message = QString(tr("Current: %1 Incoming: %2")).arg(plugin.version.toString(), version.toString());
      if (maki::warningPrompt(tr("Plugin version mismatch. Replace?"), message))
        // The user confirmed that the version should be replaced
        LOG_WARNING("Versions are different: %s", qPrintable(message));
      else
        continue;
    }

    // We need to make sure all the settings are available
    // This should only happen with a version change
    for (const auto& incoming : settings)
    {
      auto current = settingFromName(plugin.settings, incoming.getKey());

      // Check if it is a new setting
      if (!current.getKey().isEmpty())
        continue;

      LOG_DEBUG("New setting (%s) added to plugin %s", qPrintable(incoming.getKey()), qPrintable(id));
      plugin.settings.append(incoming);
    }

    // If icon is empty, try adding it
    plugin.icon = fixedIconPath;
  }

  // If it is a new plugin, then we must register it
  if (!exists)
  {
    LOG_DEBUG("Registering plugin \"%s\" settings", qPrintable(id));
    mPluginSettings.plugins.append({id, true, version, settings, fixedIconPath});
  }
  else
  {
    LOG_DEBUG("Settings for plugin \"%s\" are already registered", qPrintable(id));
  }

  return VoidResult();
}

maki::SettingField SettingsManager::settingFromName(const QVector<maki::SettingField>& settings, const QString& fieldName) const
{
  for (const auto& s : settings)
    if (fieldName == s.getKey())
      return s;

  return maki::SettingField{};
}
