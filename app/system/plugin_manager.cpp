#include "plugin_manager.h"

#include <qcoreapplication.h>
#include <qdir.h>

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>

#include "app_paths.h"
#include "common/style_helpers.h"
#include "host_services.h"
#include "logging.h"
#include "result.h"
#include "widgets/language_manager.h"
#include "widgets/settings_manager.h"

// Include after the rest to avoid conflicts
#ifdef Q_OS_WIN
#include <windows.h>
#endif

PluginManager::PluginManager(QObject* parent)
    : QObject(parent)
    , mPlugin{nullptr}
{
}

PluginManager::~PluginManager()
{
  if (mPlugin != nullptr)
    mPlugin->tearDown();
}

VoidResult PluginManager::start(const PluginSettings& settings, QMenu* menu, QComboBox* comboBox, HostServices* services)
{
  if (!menu)
    return VoidResult::Failed("No menu provided, cannot set the language plugins");

  if (!comboBox)
    return VoidResult::Failed("No comboBox provided, cannot set the language plugins");

  for (const auto& path : AppPaths::pluginSearchPaths())
  {
    QDir pluginParentDir(path);
    LOG_DEBUG("Plugin search path: %s", qPrintable(pluginParentDir.absolutePath()));
    for (const auto& subFolder : pluginParentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
      QDir pluginsDir(pluginParentDir.absoluteFilePath(subFolder));

      LOG_DEBUG("Loading plugins from %s", qPrintable(pluginsDir.path()));
      auto manifestResult = getPluginManifest(pluginsDir);
      if (!manifestResult)
      {
        LOG_DEBUG(manifestResult.ErrorMessage());
        continue;
      }

      Manifest manifest = manifestResult.Value();
      if (settings.pluginStatus(manifest.name) == PluginSettings::Status::Disabled)
      {
        LOG_DEBUG("Not loading plugin: %s, it is disabled", qPrintable(manifest.name));
        continue;
      }

      LOG_WARN_ON_FAILURE(loadPlugin(pluginsDir, manifest, menu, comboBox, services));
    }
  }

  // Connect the signal later so it is not triggered by the setup of the combobox
  connect(comboBox, &QComboBox::currentTextChanged, [this](const QString& text) {
    LOG_DEBUG("Content changed: %s", qPrintable(text));
    setPlugin(text);
  });

  if (mPlugins.isEmpty())
  {
    LOG_DEBUG("No plugins to load");
    return VoidResult();
  }

  // Set default plugin
  const auto defaultPlugin = settings.defaultPlugin.isEmpty() ? mPlugins.front().plugin->languageName() : settings.defaultPlugin;
  if (selectPlugin(comboBox, defaultPlugin))
    LOG_DEBUG("Starting with plugin: %s", qPrintable(currentPlugin()->languageName()));

  return VoidResult();
}

VoidResult PluginManager::loadPlugin(const QDir& pluginDir, const Manifest& manifest, QMenu* menu, QComboBox* comboBox, HostServices* services)
{
  if (manifest.entryPoint.isEmpty())
    return VoidResult::Failed("No entry point defined in manifest");

  auto pluginName = manifest.name;
  auto pluginPath = manifest.path;

  LOG_DEBUG("Loading plugin: %s from %s", qPrintable(pluginName), qPrintable(pluginPath));
  auto exists = pluginByLanguage(pluginName);
  if (exists != nullptr)
    return VoidResult::Failed(std::format("Plugin {} ({}) already exists", pluginName.toStdString(), pluginPath.toStdString()));

  RETURN_ON_FAILURE(loadPluginLibraryDir(manifest));

  auto loader = QSharedPointer<QPluginLoader>::create(pluginPath);
  if (!loader->load())
    return VoidResult::Failed("Loader: " + loader->errorString().toStdString());

  QObject* plugin = loader->instance();
  if (!plugin)
    return VoidResult::Failed("Failed to load plugin: " + loader->errorString().toStdString());

  auto* codeGen = qobject_cast<maki::IGeneratorPlugin*>(plugin);
  if (!codeGen)
    return VoidResult::Failed("Plugin: " + pluginPath.toStdString() + " does not adhere to maki::IGeneratorPlugin");

  // Update UI with new plugin
  QAction* action = menu->addAction(pluginName);
  connect(action, &QAction::triggered, [this, pluginName, comboBox] {
    selectPlugin(comboBox, pluginName);
  });

  comboBox->addItem(pluginName, pluginName);
  int pluginIndex = comboBox->count() - 1;

  codeGen->setName(manifest.name);
  codeGen->setVersion(manifest.version);
  codeGen->setHostServices(services);

  QDir assets = QDir(pluginDir.absoluteFilePath("assets"));
  LOG_DEBUG("Using asset path: %s", qPrintable(assets.absolutePath()));
  if (assets.exists())
    codeGen->setAssetDir(assets);

  mPlugins.append({loader, codeGen, manifest, action, pluginIndex});
  LOG_DEBUG("Loaded plugin for language: %s", qPrintable(pluginName));

  return VoidResult();
}

Result<Manifest> PluginManager::getPluginManifest(const QDir& path) const
{
  auto manifestFiles = path.entryList({"plugin.json"}, QDir::Files);
  if (manifestFiles.isEmpty())
    return Result<Manifest>::Failed("No manifest available");

  if (manifestFiles.size() != 1)
    return Result<Manifest>::Failed("Plugin folder has more than one manifest");

  auto fileName = path.absoluteFilePath(manifestFiles.at(0));
  auto manifest = JSON::fromFile(fileName);
  if (!manifest.IsSuccess())
    return Result<Manifest>::Failed("Failed to parse manifest: " + manifest.ErrorMessage());

  auto data = manifest.Value();
  return Manifest::fromJson(path.absolutePath(), data);
}

bool PluginManager::selectPlugin(QComboBox* comboBox, const QString& language)
{
  if (!setPlugin(language))
    return false;

  comboBox->setCurrentText(language);
  return true;
}

bool PluginManager::setPlugin(const QString& language)
{
  auto plugin = pluginByLanguage(language);
  if (plugin == nullptr)
  {
    LOG_WARNING("Trying to set plugin that doesn't exist: " + language.toStdString());
    return false;
  }

  if (mPlugin)
    mPlugin->tearDown();

  mPlugin = plugin;
  plugin->setup();

  LOG_DEBUG("Setting plugin: %s", qPrintable(language));
  return true;
}

maki::IGeneratorPlugin* PluginManager::currentPlugin() const
{
  return mPlugin;
}

maki::IGeneratorPlugin* PluginManager::pluginByLanguage(const QString& language) const
{
  for (const auto& plugin : mPlugins)
  {
    if (plugin.manifest.name == language)
      return plugin.plugin;
  }

  return nullptr;
}

VoidResult PluginManager::loadPluginLibraryDir(const Manifest& manifest)
{
  if (manifest.libs.isEmpty())
    return VoidResult();

#ifdef Q_OS_WIN
  const auto pluginDir = QFileInfo(manifest.path).absolutePath();
  LOG_DEBUG("Adding DLL path: %s", qPrintable(pluginDir));
  DLL_DIRECTORY_COOKIE cookie = AddDllDirectory(reinterpret_cast<LPCWSTR>(pluginDir.utf16()));
  if (!cookie)
    return VoidResult::Failed("Failed to add DLL directory: " + pluginDir.toStdString());

    // const QString pluginDir = QFileInfo(manifest.path).absolutePath();
    // LOG_DEBUG("Adding DLL path to PATH: %s", qPrintable(pluginDir));

    // QString path = qEnvironmentVariable("PATH");
    // const QString normalisedPluginDir = QDir(pluginDir).absolutePath();

    // const QStringList parts = path.split(';', Qt::SkipEmptyParts);
    // bool alreadyPresent = false;
    // for (const QString& part : parts)
    // {
    //   if (QDir(part).absolutePath().compare(normalisedPluginDir, Qt::CaseInsensitive) == 0)
    //   {
    //     alreadyPresent = true;
    //     break;
    //   }
    // }

    // if (!alreadyPresent)
    //   qputenv("PATH", (normalisedPluginDir + ";" + path).toLocal8Bit());

#endif
  return VoidResult();
}

void PluginManager::settingsChanged(const PluginSettings& settings, QMenu* menu, QComboBox* comboBox, HostServices* services)
{
  for (const auto& ps : settings.plugins)
  {
    auto index = getPluginIndex(ps.name);
    if (index >= 0 && !ps.enabled)
    {
      LOG_WARN_ON_FAILURE(deregisterPlugin(mPlugins.at(index), menu, comboBox));
    }
    else if (index < 0 && ps.enabled)
    {
      for (const auto& path : AppPaths::pluginSearchPaths())
      {
        QDir pluginParentDir(path);
        for (const auto& subFolder : pluginParentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
          QDir pluginsDir(pluginParentDir.absoluteFilePath(subFolder));

          LOG_DEBUG("Loading plugins from %s", qPrintable(pluginsDir.path()));
          auto manifestResult = getPluginManifest(pluginsDir);
          if (!manifestResult || manifestResult.Value().name != ps.name)
            continue;

          Manifest manifest = manifestResult.Value();
          LOG_WARN_ON_FAILURE(loadPlugin(pluginsDir, manifest, menu, comboBox, services));
        }
      }
    }
  }
}

int PluginManager::getPluginIndex(const QString& pluginName) const
{
  for (int i = 0; i < mPlugins.size(); ++i)
    if (mPlugins.at(i).plugin && mPlugins.at(i).plugin->languageName() == pluginName)
      return i;

  return -1;
}

VoidResult PluginManager::deregisterPlugin(const Plugin& plugin, QMenu* menu, QComboBox* comboBox)
{
  LOG_TRACE("Deregistering plugin: %s", qPrintable(plugin.manifest.name));
  const auto index = getPluginIndex(plugin.plugin->languageName());
  if (index == -1)
    return VoidResult::Failed("Plugin not registered");

  // In case we are running the plugin, we must update it
  if (plugin.plugin->languageName() == mPlugin->languageName())
  {
    // Get the next plugin to auto update the picker
    int nextIndex = 0;
    if (index + 1 >= mPlugins.size())
      nextIndex = index - 1;
    else
      nextIndex = index + 1;

    LOG_TRACE("Plugin was running");
    if (nextIndex >= 0)
    {
      const auto nextPlugin = mPlugins.at(nextIndex);
      setPlugin(nextPlugin.plugin->languageName());
    }
    else if (mPlugin)
    {
      // If there are no plugins to swap, just tear down the current plugin and reset it
      mPlugin->tearDown();
      mPlugin = nullptr;
    }
  }

  // Finally, remove it from the list
  comboBox->removeItem(plugin.comboIndex);
  if (plugin.action)
    menu->removeAction(plugin.action);

  // Unload immediately
  plugin.loader->unload();

  auto removed = mPlugins.removeIf([&](const Plugin& p) { return p.manifest.name == plugin.manifest.name; });
  if (removed != 1)
    return VoidResult::Failed("Failed to remove plugin from the list");

  return VoidResult();
}
