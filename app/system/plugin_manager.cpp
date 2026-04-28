#include "plugin_manager.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>

#include "app_paths.h"
#include "common/style_helpers.h"
#include "host_services.h"
#include "logging.h"

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

VoidResult PluginManager::start(QMenu* menu, QComboBox* comboBox, HostServices* services)
{
  if (!menu)
    return VoidResult::Failed("No menu provided, cannot set the language plugins");

  if (!comboBox)
    return VoidResult::Failed("No comboBox provided, cannot set the language plugins");

  for (const auto& path : AppPaths::pluginSearchPaths())
  {
    QDir pluginParentDir(path);
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
      LOG_WARN_ON_FAILURE(loadPlugin(pluginsDir, manifest, menu, comboBox, services));
    }
  }

  // Connect the signal later so it is not triggered by the setup of the combobox
  connect(comboBox, &QComboBox::currentTextChanged, [this](const QString& text) {
    setPlugin(text);
  });

  if (mPlugins.isEmpty())
  {
    LOG_DEBUG("No plugins to load");
    return VoidResult();
  }

  // Set default plugin
  setPlugin(mPlugins.front().plugin->languageName());
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

  QPluginLoader loader(pluginPath);
  if (!loader.load())
    return VoidResult::Failed("Loader: " + loader.errorString().toStdString());

  QObject* plugin = loader.instance();
  if (!plugin)
    return VoidResult::Failed("Failed to load plugin: " + loader.errorString().toStdString());

  auto* codeGen = qobject_cast<maki::IGeneratorPlugin*>(plugin);
  if (!codeGen)
    return VoidResult::Failed("Plugin: " + pluginPath.toStdString() + " does not adhere to maki::IGeneratorPlugin");

  // Update UI with new plugin
  QAction* action = menu->addAction(pluginName);
  connect(action, &QAction::triggered, [this, pluginName, comboBox] {
    if (setPlugin(pluginName))
      comboBox->setCurrentText(pluginName);
  });

  comboBox->addItem(pluginName, pluginName);

  codeGen->setName(manifest.name);
  codeGen->setVersion(manifest.version);
  codeGen->setHostServices(services);

  QDir assets = QDir(pluginDir.absoluteFilePath("assets"));
  LOG_DEBUG("Using asset path: %s", qPrintable(assets.absolutePath()));
  if (assets.exists())
    codeGen->setAssetDir(assets);

  mPlugins.append({codeGen, manifest});
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
