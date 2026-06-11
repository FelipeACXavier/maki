#pragma once

#include <QApplication>
#include <QString>
#include <QVector>
#include <oclero/qlementine.hpp>

#include "app_paths.h"
#include "isettings.h"
#include "plugin_version.h"

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

  bool operator!=(const GeneralSettings& s) const;
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
  bool startLogFilterExpanded = true;   /// Start the log table filters collapsed or expanded.
  oclero::qlementine::Theme themeVars;  /// Theme variables.
  int numberOfColumns = 1;
  int edgeShape = 0;

  bool operator!=(const AppearanceSettings& s) const;
};

/**
 * @brief Holds generation settings.
 */
struct GenerationSettings
{
  QString generationDir = QCoreApplication::applicationDirPath() + "/generation";  /// Directory for generated files.
  QStringList pluginSearchPaths = {AppPaths::pluginSearchPaths()};                 /// Paths to search for plugins.

  bool operator!=(const GenerationSettings& s) const;
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
  QString icon;

  bool operator==(const PluginInfo& s) const;
  bool operator!=(const PluginInfo& s) const;
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

  bool operator!=(const PluginSettings& s) const;
  Status pluginStatus(const QString& pluginName) const;
};