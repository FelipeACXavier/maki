#include "plugin_manager.h"

#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>

#include "compiler/dezyne_generator.h"
#include "logging.h"

PluginManager::PluginManager()
    : QObject()
    , mPlugin{nullptr}
{
}

void PluginManager::start(QMenu* menu)
{
  if (!menu)
  {
    LOG_WARNING("No menu provided, cannot set the langauge plugins");
    return;
  }

  // QDir pluginsDir = QDir(QApplication::applicationDirPath().append("/../plugins"));

  // LOG_INFO("Loading plugins from %s", qPrintable(pluginsDir.path()));
  // auto pluginNames = pluginsDir.entryList(QDir::Files);
  // if (pluginNames.isEmpty())
  // {
  //   LOG_DEBUG("No plugins to load");
  //   return;
  // }

  // for (const QString& fileName : pluginNames)
  // {
  //   QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
  //   QObject* plugin = loader.instance();

  //   if (!plugin)
  //   {
  //     LOG_DEBUG("Failed to load plugin: %s", qPrintable(loader.errorString()));
  //     continue;
  //   }

  //   auto* codeGen = qobject_cast<GeneratorPlugin*>(plugin);
  //   if (!codeGen)
  //     continue;

  //   LOG_DEBUG("Loaded plugin for language: %s", qPrintable(codeGen->languageName()));

  //   QAction* action = menu->addAction(codeGen->languageName());
  //   connect(action, &QAction::triggered, [this, codeGen] { setPlugin(codeGen); });

  //   mPlugins.append(codeGen);
  // }

  // TODO(felaze): Use the plugin system above once the interfaces are defined
  DezyneGenerator* generator = new DezyneGenerator();
  mPlugins.append(generator);

  // Set default plugin
  setPlugin(mPlugins.front());
  LOG_DEBUG("Starting with plugin: %s", qPrintable(currentPlugin()->languageName()));
}

void PluginManager::setPlugin(GeneratorPlugin* plugin)
{
  mPlugin = plugin;
}

GeneratorPlugin* PluginManager::currentPlugin() const
{
  return mPlugin;
}

GeneratorPlugin* PluginManager::pluginByLanguage(generator::Language language) const
{
  for (const auto& plugin : mPlugins)
  {
    if (plugin->supportedLanguage() == language)
      return plugin;
  }

  return nullptr;
}
