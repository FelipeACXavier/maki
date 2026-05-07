// SettingsManager.h
#pragma once

#include <QApplication>
#include <QObject>
#include <QSettings>
#include <oclero/qlementine.hpp>

#include "app_paths.h"
#include "generator_plugin.h"
#include "isettings.h"
#include "style_helpers.h"

namespace Config
{
struct ThemeInfo;
}

/**
 * @brief Holds general application settings.
 */
struct GeneralSettings
{
  bool restoreLastSession = true;           /// Whether to restore the last session on startup.
  bool autosaveEnabled = true;              /// Whether autosave is enabled.
  int autosaveIntervalMinutes = 5;          /// Interval for autosave in minutes.
  bool confirmOnCloseWithExecution = true;  /// Whether to confirm close with execution.
  bool enableDebugLogs = true;              /// Whether debug logs are enabled.
  int recentHistorySize = 10;               /// Size of the recent history list.
  bool showWelcomeMessage = true;           /// Whether to show the welcome message.
  QVector<QString> recentFiles = {};        /// List of recent files.
  QString lastOpenFileDir = "";             /// Directory of the last opened file.
  QString language = "en_US";               /// Application language.

  bool operator!=(const GeneralSettings& s) const
  {
    return restoreLastSession != s.restoreLastSession ||
           autosaveEnabled != s.autosaveEnabled ||
           autosaveIntervalMinutes != s.autosaveIntervalMinutes ||
           confirmOnCloseWithExecution != s.confirmOnCloseWithExecution ||
           enableDebugLogs != s.enableDebugLogs ||
           recentHistorySize != s.recentHistorySize ||
           recentFiles != s.recentFiles ||
           showWelcomeMessage != s.showWelcomeMessage ||
           lastOpenFileDir != s.lastOpenFileDir ||
           language != s.language;
  }
};

/**
 * @brief Holds appearance settings.
 */
struct AppearanceSettings
{
  QString theme = "Dark";               /// Current theme.
  int uiScalePercent = 100;             /// UI scale percentage.
  bool showCanvasGrid = true;           /// Whether to show the canvas grid.
  bool nativeMenuBar = true;            /// Whether to use the native menu bar.
  int nodeCornerRadius = 8;             /// Node corner radius.
  oclero::qlementine::Theme themeVars;  /// Theme variables.

  bool operator!=(const AppearanceSettings& s) const
  {
    return theme != s.theme ||
           uiScalePercent != s.uiScalePercent ||
           showCanvasGrid != s.showCanvasGrid ||
           nativeMenuBar != s.nativeMenuBar ||
           nodeCornerRadius != s.nodeCornerRadius ||
           themeVars != s.themeVars;
  }
};

/**
 * @brief Holds generation settings.
 */
struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath() + "/generation";  /// Directory for generated files.
  QStringList pluginSearchPaths = {AppPaths::pluginSearchPaths()};                 /// Paths to search for plugins.

  bool operator!=(const GenerationSettings& s) const
  {
    return generationDir != s.generationDir ||
           pluginSearchPaths != s.pluginSearchPaths;
  }
};

/**
 * @brief Holds information about a plugin.
 */
struct PluginInfo
{
  QString name;                          /// Name of the plugin.
  bool enabled;                          /// Whether the plugin is enabled.
  maki::PluginVersion version;           /// Version of the plugin.
  QVector<maki::SettingField> settings;  /// Settings for the plugin.
  maki::SettingsFunction callback;       /// Callback function for the plugin.

  bool operator==(const PluginInfo& s) const
  {
    return name == s.name &&
           enabled == s.enabled &&
           version == s.version &&
           settings == s.settings;
  }

  bool operator!=(const PluginInfo& s) const
  {
    return !(*this == s);
  }
};

/**
 * @brief Holds information about a plugin.
 */
struct PluginSettings
{
  QString defaultPlugin;             /// The default plugin to be used
  QVector<PluginInfo> plugins = {};  /// The list of plugin settings

  enum class Status
  {
    Unknown,
    Enabled,
    Disabled
  };

  bool operator!=(const PluginSettings& s) const
  {
    return defaultPlugin != s.defaultPlugin ||
           plugins != s.plugins;
  }

  Status pluginStatus(const QString& pluginName) const
  {
    for (const auto& p : plugins)
      if (p.name == pluginName)
        return p.enabled ? Status::Enabled : Status::Disabled;

    return Status::Unknown;
  }
};

/**
 * @brief Manages application settings and provides access to them.
 */
class SettingsManager : public QObject, public maki::ISettings
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new SettingsManager object.
   *
   * @param themeManager Pointer to the ThemeManager.
   * @param parent Parent object.
   */
  SettingsManager(oclero::qlementine::ThemeManager* themeManager, QObject* parent = nullptr);

  /**
   * @brief Gets the general settings.
   *
   * @return GeneralSettings The current general settings.
   */
  GeneralSettings general() const;

  /**
   * @brief Gets the appearance settings.
   *
   * @return AppearanceSettings The current appearance settings.
   */
  AppearanceSettings appearance() const;

  /**
   * @brief Gets the generation settings.
   *
   * @return GenerationSettings The current generation settings.
   */
  GenerationSettings generation() const;

  /**
   * @brief Gets the list of plugins.
   *
   * @return PluginSettings The current plugin settings
   */
  PluginSettings plugins() const;

  /**
   * @brief Applies the current temporary settings to the permanent settings and emits a signal indicating that settings have changed.
   *
   * This function is called to save any changes made during the user interface session. It copies the temporary settings to their corresponding permanent settings, and then calls save() to persist these changes to storage.
   */
  void applySettings();

  /**
   * @brief Sets the general settings.
   *
   * @param s The new general settings.
   */
  void setGeneral(const GeneralSettings& s);

  /**
   * @brief Sets the appearance settings.
   *
   * @param s The new appearance settings.
   */
  void setAppearance(const AppearanceSettings& s);

  /**
   * @brief Sets the generation settings.
   *
   * @param s The new generation settings.
   */
  void setGeneration(const GenerationSettings& s);

  /**
   * @brief Sets the list of plugins.
   *
   * @param s The new list of plugin information.
   */
  void setPlugins(const PluginSettings& s);

  /**
   * @brief Gets the settings for a specific plugin.
   *
   * @param id ID of the plugin.
   * @return QVector<maki::SettingField> List of settings for the plugin.
   */
  QVector<maki::SettingField> getPluginSettings(const QString& id) const override;

  /**
   * @brief Registers settings for a plugin.
   *
   * @param id ID of the plugin.
   * @param version Version of the plugin.
   * @param settings List of settings for the plugin.
   * @param callback Callback function for the plugin.
   */
  VoidResult registerSettings(const QString& id, const maki::PluginVersion version,
                              const QVector<maki::SettingField>& settings,
                              maki::SettingsFunction callback) override;

  /**
   * @brief Loads settings from a file.
   */
  void load();

  /**
   * @brief Saves settings to a file.
   */
  void save();

  /**
   * @brief Gets the list of available themes.
   *
   * @return std::vector<oclero::qlementine::Theme> List of available themes.
   */
  std::vector<oclero::qlementine::Theme> availableThemes() const;

  /**
   * @brief Get a theme based on its metadata name
   *
   * @return In case the theme exists, it is returned. Otherwise Result is set to Failed
   */
  Result<oclero::qlementine::Theme> themeByName(const QString& themeName) const;

  /**
   * @brief Handles a theme creation event.
   *
   * @param themePath Path to the created theme.
   */
  void themeCreated(const QString& themePath);

signals:
  /**
   * @brief Emitted when settings change.
   */
  void settingsChanged();

public slots:
  /**
   * @brief Adds a file to the recent files list.
   *
   * @param s Path to the file.
   */
  void addRecentFile(const QString& s);

private:
  QSettings mSettings;             /// Settings storage.
  GeneralSettings mGeneral;        /// Current general settings.
  AppearanceSettings mAppearance;  /// Current appearance settings.
  GenerationSettings mGeneration;  /// Current generation settings.
  PluginSettings mPluginSettings;  /// Current plugin settings.

  /// Temporaty copies until apply is called
  GeneralSettings mTmpGeneral;
  AppearanceSettings mTmpAppearance;
  GenerationSettings mTmpGeneration;
  PluginSettings mTmpPluginSettings;

  oclero::qlementine::ThemeManager* mThemeManager;  /// Pointer to the ThemeManager.

  /**
   * @brief Retrieves a setting field from a list of settings by its name.
   *
   * This function searches through the provided vector of `maki::SettingField` objects to find one with a matching name.
   *
   * @param settings The vector of settings to search through.
   * @param fieldName The name of the setting field to retrieve.
   * @return maki::SettingField A copy of the found setting field, or an empty field if not found.
   */
  maki::SettingField settingFromName(const QVector<maki::SettingField>& settings, const QString& fieldName) const;
};
