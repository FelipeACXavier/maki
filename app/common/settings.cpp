#include "settings.h"

#include "logging.h"
#include "macros.h"

bool GeneralSettings::operator!=(const GeneralSettings& s) const
{
  return restoreLastSession != s.restoreLastSession || autosaveEnabled != s.autosaveEnabled || autosaveIntervalMinutes != s.autosaveIntervalMinutes ||
         confirmOnCloseWithExecution != s.confirmOnCloseWithExecution || enableDebugLogs != s.enableDebugLogs || recentHistorySize != s.recentHistorySize ||
         recentFiles != s.recentFiles || showWelcomeMessage != s.showWelcomeMessage || lastOpenFileDir != s.lastOpenFileDir || language != s.language ||
         maximumLogHistory != s.maximumLogHistory || showTourOnStart != s.showTourOnStart;
}

bool AppearanceSettings::operator!=(const AppearanceSettings& s) const
{
  return theme != s.theme || uiScalePercent != s.uiScalePercent || showCanvasGrid != s.showCanvasGrid || nativeMenuBar != s.nativeMenuBar ||
         nodeCornerRadius != s.nodeCornerRadius || themeVars != s.themeVars || startLogFilterExpanded != s.startLogFilterExpanded ||
         numberOfColumns != s.numberOfColumns || edgeShape != s.edgeShape || numberOfColumnsTask != s.numberOfColumnsTask;
}

bool GenerationSettings::operator!=(const GenerationSettings& s) const
{
  return generationDir != s.generationDir || pluginSearchPaths != s.pluginSearchPaths;
}

bool PluginInfo::operator==(const PluginInfo& s) const
{
  return name == s.name && enabled == s.enabled && version == s.version && settings == s.settings && icon == s.icon;
}

bool PluginInfo::operator!=(const PluginInfo& s) const
{
  return !(*this == s);
}

bool PluginSettings::operator!=(const PluginSettings& s) const
{
  return defaultPlugin != s.defaultPlugin || showCombinedLogs != s.showCombinedLogs || plugins != s.plugins;
}

PluginSettings::Status PluginSettings::pluginStatus(const QString& pluginName) const
{
  if (!plugins.contains(pluginName))
  {
    LOG_DEBUG("Could not find plugin: {}. Available: {}", pluginName, maki::accumulate<QString>(plugins, [](QString total, const auto& plugin) {
                if (total.isEmpty())
                  return plugin.name;

                return total + ", " + plugin.name;
              }));

    return Status::Unknown;
  }

  return plugins[pluginName].enabled ? Status::Enabled : Status::Disabled;
}
