#include "plugin_manager.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QMenu>
#include <QPluginLoader>

#include "common/style_helpers.h"
#include "host_services.h"
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

void PluginManager::start(QMenu* menu, QComboBox* comboBox, HostServices* services)
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
    codeGen->setHostServices(services);

    QAction* action = menu->addAction(codeGen->languageName());
    connect(action, &QAction::triggered, [this, codeGen, comboBox] {
      setPlugin(codeGen);
      if (codeGen)
        comboBox->setCurrentText(codeGen->languageName());
    });

    comboBox->addItem(codeGen->languageName(), codeGen->languageName());
    mPlugins.append(codeGen);
  }

  connect(comboBox, &QComboBox::currentTextChanged, [this](const QString& text) {
    setPlugin(pluginByLanguage(text));
  });

  // Set default plugin
  setPlugin(mPlugins.front());
  LOG_DEBUG("Starting with plugin: %s", qPrintable(currentPlugin()->languageName()));
}

void PluginManager::setPlugin(maki::IGeneratorPlugin* plugin)
{
  if (!plugin)
  {
    LOG_WARNING("Trying to set plugin that doesn't exist");
    return;
  }

  if (mPlugin)
    mPlugin->tearDown();

  mPlugin = plugin;
  plugin->setup();

  LOG_DEBUG("Setting plugin: %s", qPrintable(plugin->languageName()));
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
