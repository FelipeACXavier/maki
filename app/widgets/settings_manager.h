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

  bool operator!=(const GeneralSettings& s)
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

/**
 * @brief Holds generation settings.
 */
struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath() + "/generation";  /// Directory for generated files.
  QStringList pluginSearchPaths = {AppPaths::pluginSearchPaths()};                 /// Paths to search for plugins.

  bool operator!=(const GenerationSettings& s)
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
   * @return QVector<PluginInfo> List of plugin information.
   */
  QVector<PluginInfo> plugins() const;

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
  void setPlugins(const QVector<PluginInfo>& s);

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
   * @brief Emitted when the theme changes.
   *
   * @param theme New theme name.
   */
  void themeChanged();

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

  QVector<PluginInfo> mPluginSettings;  /// List of plugin information.

  oclero::qlementine::ThemeManager* mThemeManager;  /// Pointer to the ThemeManager.
};
