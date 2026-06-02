#include "plugin_manager.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>
#include <QProcess>
#include <QProgressBar>
#include <QTimer>
#include <oclero/qlementine/widgets/Label.hpp>

#include "app_paths.h"
#include "common/style_helpers.h"
#include "compiler/plugin_action_registry.h"
#include "host_services.h"
#include "logging.h"
#include "notifications.h"
#include "pipeline_action.h"
#include "result.h"
#include "widgets/settings_manager.h"

// Include after the rest to avoid conflicts
#ifdef Q_OS_WIN
#include <windows.h>
#endif

PluginManager::PluginManager(maki::PipelineActionRegistry* registry, Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPlugin{nullptr}
    , mRegistry(registry)
    , mPipeline(pipeline)
    , mIsRunning(false)
{
  connect(mPipeline, &Pipeline::finishedLast, [this](const Pipeline::Info& info, int exitCode, const QString& message) {
    if (!mIsRunning)
      return;

    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());

    // Send an empty content so the widget is cleared
    NOTIFY_LONG_INFO(mProgressId, "Installation Progress", nullptr);
    mProgressId.clear();
    mIsRunning = false;
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](const Pipeline::Info& info, QProcess::ProcessError /* error */, const QString& message) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());
    NOTIFY_ERROR("Plugin installation", "Error occurred: {} ", message.toStdString());
    mProgressId.clear();
    mIsRunning = false;
  });
  connect(mPipeline, &Pipeline::startingPipeline, [this](const Pipeline::Info& info) {
    if (!mIsRunning)
      return;

    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::startingGroup, [this](const Pipeline::Info& info, const QString& groupName) {
    if (!mIsRunning)
      return;

    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::processStarted, [this](const Pipeline::Info& info, const QString& /* process */, const QStringList& /* arguments */) {
    if (!mIsRunning)
      return;

    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::finishedGroup, [this](const Pipeline::Info& info, const QString& groupName, int exitCode, const QString& message) {
    if (!mIsRunning)
      return;

    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Installation Progress", progressContent());
  });
}

PluginManager::~PluginManager()
{
  if (mPlugin != nullptr)
    mPlugin->tearDown();
}

VoidResult PluginManager::start(const PluginSettings& settings, HostServices* services)
{
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

      maki::Manifest manifest = manifestResult.Value();
      auto status = settings.pluginStatus(manifest.name);
      if (status == PluginSettings::Status::Disabled)
      {
        LOG_DEBUG("Not loading plugin: %s, it is disabled", qPrintable(manifest.name));
        continue;
      }

      LOG_WARN_ON_FAILURE(loadPlugin(pluginsDir, manifest, services, status));
    }
  }

  if (mPlugins.isEmpty())
  {
    LOG_DEBUG("No plugins to load");
    return VoidResult();
  }

  // Try setting the config plugin, otherwise just try the first
  if (setPlugin(settings.defaultPlugin) || setPlugin(mPlugins.front().plugin->languageName()))
  {
    if (currentPlugin())
      LOG_DEBUG("Starting with plugin: %s", qPrintable(currentPlugin()->languageName()));
  }

  if (mPipeline->size() > 0)
  {
    LOG_DEBUG("There are installation steps needed");
    QTimer::singleShot(0, this, [this] {
      auto result = mPipeline->start();
      if (!result.IsSuccess())
      {
        LOG_WARNING(result.ErrorMessage());
        return;
      }

      mIsRunning = true;
    });
  }

  return VoidResult();
}

VoidResult PluginManager::loadPlugin(const QDir& pluginDir, const maki::Manifest& manifest, HostServices* services, PluginSettings::Status status)
{
  if (manifest.entryPoint.isEmpty())
    return VoidResult::Failed("No entry point defined in manifest");

  auto pluginName = manifest.name;
  auto pluginPath = manifest.pluginPath();

  LOG_DEBUG("Loading plugin: %s from %s with status: %d", qPrintable(pluginName), qPrintable(pluginPath), (int)status);
  auto exists = pluginByLanguage(pluginName);
  if (exists != nullptr)
    return VoidResult::Failed(std::format("Plugin {} ({}) already exists", pluginName.toStdString(), pluginPath.toStdString()));

  RETURN_ON_FAILURE(loadPluginLibraryDir(manifest));

  // If it is the first time we are seeing this plugin, then we might need to install it
  if (status == PluginSettings::Status::Unknown)
  {
    RETURN_ON_FAILURE(installPlugin(manifest));
  }

  auto loader = QSharedPointer<QPluginLoader>::create(pluginPath);
#ifdef DEV_BUILD
  loader->setLoadHints(QLibrary::LoadHints{});
#endif
  if (!loader->load())
    return VoidResult::Failed("Loader: " + loader->errorString().toStdString());

  QObject* plugin = loader->instance();
  if (!plugin)
    return VoidResult::Failed("Failed to load plugin: " + loader->errorString().toStdString());

  auto* codeGen = qobject_cast<maki::IPlugin*>(plugin);
  if (!codeGen)
    return VoidResult::Failed("Plugin: " + pluginPath.toStdString() + " does not adhere to maki::IPlugin");

  codeGen->setManifest(manifest);
  codeGen->setHostServices(services);

  QDir assets = QDir(pluginDir.absoluteFilePath("assets"));
  LOG_DEBUG("Using asset path: %s", qPrintable(assets.absolutePath()));
  if (assets.exists())
    codeGen->setAssetDir(assets);

  // Register plugin actions in
  for (const auto& action : codeGen->pipelineActions())
  {
    LOG_DEBUG("Registering action: %s", qPrintable(action->id()));
    LOG_WARN_ON_FAILURE(mRegistry->registerAction(codeGen->languageName(), action));
  }

  mPlugins.append({loader, codeGen});
  LOG_DEBUG("Loaded plugin for language: %s", qPrintable(pluginName));

  emit pluginAdded(mPlugins.last());

  return VoidResult();
}

Result<maki::Manifest> PluginManager::getPluginManifest(const QDir& path) const
{
  auto manifestFiles = path.entryList({"plugin.json"}, QDir::Files);
  if (manifestFiles.isEmpty())
    return Result<maki::Manifest>::Failed("No manifest available");

  if (manifestFiles.size() != 1)
    return Result<maki::Manifest>::Failed("Plugin folder has more than one manifest");

  auto fileName = path.absoluteFilePath(manifestFiles.at(0));
  auto manifest = JSON::fromFile(fileName);
  if (!manifest.IsSuccess())
    return Result<maki::Manifest>::Failed("Failed to parse manifest: " + manifest.ErrorMessage());

  auto data = manifest.Value();
  return maki::Manifest::fromJson(path.absolutePath(), data);
}

bool PluginManager::setPlugin(const QString& language)
{
  auto plugin = pluginByLanguage(language);
  if (plugin == nullptr)
  {
    LOG_TRACE("Trying to set plugin that doesn't exist: " + language.toStdString());
    return false;
  }

  if (mPlugin)
  {
    // No need to set the same plugin again...
    if (mPlugin->languageName() == language)
    {
      LOG_TRACE("Plugin %s already set", qPrintable(language));
      return false;
    }

    mPlugin->tearDown();
  }

  mPlugin = plugin;
  plugin->setup();

  LOG_DEBUG("Setting plugin: %s", qPrintable(language));
  return true;
}

maki::IPlugin* PluginManager::currentPlugin() const
{
  return mPipeline && mPipeline->isRunning() ? nullptr : mPlugin;
}

maki::IPlugin* PluginManager::pluginByLanguage(const QString& language) const
{
  for (const auto& plugin : mPlugins)
  {
    if (plugin.plugin->languageName() == language)
      return plugin.plugin;
  }

  return nullptr;
}

VoidResult PluginManager::loadPluginLibraryDir(const maki::Manifest& manifest)
{
  if (manifest.libs.isEmpty())
    return VoidResult();

#ifdef Q_OS_WIN
  const auto pluginDir = QFileInfo(manifest.pluginPath()).absolutePath();
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

VoidResult PluginManager::installPlugin(const maki::Manifest& manifest)
{
#ifndef __EMSCRIPTEN__
  mPipeline->startGroup(manifest.name);
  for (const auto& step : manifest.installationSteps)
  {
    QProcess* install = new QProcess(this);
    install->setWorkingDirectory(manifest.path);
    install->setProgram(step.command);
    install->setArguments(step.args);
    mPipeline->add(install, maki::OnFail::STOP);
  }

  mPipeline->endGroup();
#endif
  return VoidResult();
}

void PluginManager::settingsChanged(const PluginSettings& settings, HostServices* services)
{
  for (const auto& ps : settings.plugins)
  {
    auto index = getPluginIndex(ps.name);
    if (index >= 0)
    {
      if (!ps.enabled)
        LOG_WARN_ON_FAILURE(deregisterPlugin(mPlugins.at(index)));
      else
        mPlugins.at(index).plugin->settingsChanged(ps.settings);
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

          maki::Manifest manifest = manifestResult.Value();
          LOG_WARN_ON_FAILURE(loadPlugin(pluginsDir, manifest, services, PluginSettings::Status::Enabled));
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

VoidResult PluginManager::deregisterPlugin(const Plugin& plugin)
{
  LOG_TRACE("Deregistering plugin: %s", qPrintable(plugin.plugin->languageName()));
  const auto index = getPluginIndex(plugin.plugin->languageName());
  if (index == -1)
    return VoidResult::Failed("Plugin not registered");

  if (!plugin.plugin)
    return VoidResult::Failed("Plugin missing");

  // In case we are running the plugin, we must update it
  if (mPlugin && plugin.plugin->languageName() == mPlugin->languageName())
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
    else
    {
      // If there are no plugins to swap, just tear down the current plugin and reset it
      mPlugin->tearDown();
      mPlugin = nullptr;
    }
  }

  // Unload immediately
  if (!plugin.loader->unload())
    LOG_WARNING("Failed to unload the plugin: %s", qPrintable(plugin.plugin->languageName()));

  // Finally, remove it from the list
  auto removed = mPlugins.removeIf([&](const Plugin& p) { return p.plugin->languageName() == plugin.plugin->languageName(); });
  if (removed != 1)
    return VoidResult::Failed("Failed to remove plugin from the list");

  return VoidResult();
}

VoidResult PluginManager::reloadPlugin(const QString& pluginName, HostServices* services)
{
  LOG_DEBUG("Reloading plugin: %s", qPrintable(pluginName));
  const int index = getPluginIndex(pluginName);
  if (index < 0)
    return VoidResult::Failed("Plugin not loaded: " + pluginName.toStdString());

  const auto old = mPlugins.at(index);
  const QDir pluginDir = QDir(old.plugin->manifest().path);

  RETURN_ON_FAILURE(deregisterPlugin(old));

  auto manifestResult = getPluginManifest(pluginDir);
  if (!manifestResult)
    return VoidResult::Failed(manifestResult.ErrorMessage());

  RETURN_ON_FAILURE(loadPlugin(pluginDir, manifestResult.Value(), services, PluginSettings::Status::Enabled));

  setPlugin(pluginName);
  return VoidResult();
}

QWidget* PluginManager::progressContent()
{
  QWidget* container = new QWidget();
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* layout = new QVBoxLayout(container);

  auto getRunningTask = [](Pipeline::GroupInfo group) {
    bool allDone = true;
    for (const auto& p : group.processes)
    {
      if (p.status == Pipeline::State::Running)
        return p.name;
      if (p.status == Pipeline::State::Error)
        return QString("Error");

      allDone = allDone && (p.status == Pipeline::State::Done);
    }

    return QString(allDone ? "Done" : "Waiting");
  };
  auto getCompleteTasks = [](Pipeline::GroupInfo group) {
    int count = 0;
    for (const auto& p : group.processes)
      if (p.status == Pipeline::State::Done)
        count++;

    return count;
  };

  // For each group, we have
  for (const auto& group : mInfo.groupInfo)
  {
    if (group.processes.isEmpty())
      continue;

    auto* row = new QWidget(container);
    row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* rowLayout = new QVBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(4);

    // Header row: group name + count
    auto* header = new QWidget(row);
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    auto* nameLabel = new oclero::qlementine::Label(group.name, header);
    nameLabel->setRole(oclero::qlementine::TextRole::H4);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto completed = getCompleteTasks(group);
    auto total = group.processes.count();
    auto* countLabel = new QLabel(QString("%1 / %2 tasks").arg(completed).arg(total), header);

    headerLayout->addWidget(nameLabel);
    headerLayout->addWidget(countLabel);

    // Progress bar
    auto* progress = new QProgressBar(row);
    progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    progress->setRange(0, total);
    progress->setValue(completed);
    progress->setTextVisible(false);

    // Current task label
    auto currentTask = getRunningTask(group);
    auto* currentTaskLabel = new oclero::qlementine::Label(currentTask, row);
    currentTaskLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    currentTaskLabel->setWordWrap(false);
    currentTaskLabel->setRole(oclero::qlementine::TextRole::H5);

    rowLayout->addWidget(header);
    rowLayout->addWidget(progress);
    rowLayout->addWidget(currentTaskLabel);

    layout->addWidget(row);
  }

  return container;
}
