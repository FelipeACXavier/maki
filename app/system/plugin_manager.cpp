#include "plugin_manager.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>

#include "common/style_helpers.h"
#include "logging.h"

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

void PluginManager::start(QMenu* menu, QComboBox* comboBox)
{
  if (!menu)
  {
    LOG_WARNING("No menu provided, cannot set the langauge plugins");
    return;
  }

  if (!comboBox)
  {
    LOG_WARNING("No comboBox provided, cannot set the langauge plugins");
    return;
  }

  QDir pluginsDir(getDirPathFor("plugins"));

  LOG_INFO("Loading plugins from %s", qPrintable(pluginsDir.path()));
  auto pluginNames = pluginsDir.entryList(QDir::Files);
  if (pluginNames.isEmpty())
  {
    LOG_DEBUG("No plugins to load");
    return;
  }

  for (const QString& fileName : pluginNames)
  {
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QObject* plugin = loader.instance();

    if (!plugin)
    {
      LOG_DEBUG("Failed to load plugin: %s", qPrintable(loader.errorString()));
      continue;
    }

    auto* codeGen = qobject_cast<maki::IGeneratorPlugin*>(plugin);
    if (!codeGen)
      continue;

    LOG_DEBUG("Loaded plugin for language: %s", qPrintable(codeGen->languageName()));

    QAction* action = menu->addAction(codeGen->languageName());
    connect(action, &QAction::triggered, [this, codeGen] { setPlugin(codeGen); });

    comboBox->addItem(codeGen->languageName(), codeGen->languageName());

    mPlugins.append(codeGen);
  }

  // Set default plugin
  setPlugin(mPlugins.front());
  LOG_DEBUG("Starting with plugin: %s", qPrintable(currentPlugin()->languageName()));
}

void PluginManager::setPlugin(maki::IGeneratorPlugin* plugin)
{
  if (mPlugin)
    mPlugin->tearDown();

  mPlugin = plugin;
  plugin->setup();
}

maki::IGeneratorPlugin* PluginManager::currentPlugin() const
{
  return mPlugin;
}

maki::IGeneratorPlugin* PluginManager::pluginByLanguage(const QString& language) const
{
  for (const auto& plugin : mPlugins)
  {
    if (plugin->languageName() == language)
      return plugin;
  }

  return nullptr;
}
