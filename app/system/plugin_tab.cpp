#include "plugin_tab.h"

#include <QMenu>

#include "app_configs.h"
#include "itab.h"
#include "logging.h"
#include "plugin_view.h"

PluginTab::PluginTab(QMenu* menu, QObject* parent)
    : QObject(parent)
    , mMenu(menu)
{
}

void PluginTab::updateScene(const QString& name)
{
  if (mTabs.find(name) == mTabs.end())
    return;

  if (!mTabs[name].callback)
    return;

  LOG_WARN_ON_FAILURE(mTabs[name].callback(mTabs[name].scene));
  mTabs[name].view->fitInView(mTabs[name].scene->sceneRect(), Qt::KeepAspectRatio);
}

void PluginTab::openScene(const QString& name)
{
  LOG_DEBUG("Opening scene");

  if (mTabs.find(name) == mTabs.end())
    return;

  auto title = name + " view";
  emit openView(title, mTabs[name].view);
}

maki::ThemeVars PluginTab::currentTheme()
{
  return maki::ThemeVars{};
}

maki::ThemeFonts PluginTab::labelFont()
{
  return {
      Fonts::Main,
      Fonts::Property,
      Fonts::Label,
      Fonts::Hint,
      Fonts::MonoSpace};
}

void PluginTab::onThemeChanged()
{
  for (auto it = mTabs.constBegin(); it != mTabs.constEnd(); ++it)
    updateScene(it.key());
}

void PluginTab::registerPlugin(const QString& name, std::function<VoidResult(QGraphicsScene* scene)> callback)
{
  if (mTabs.find(name) != mTabs.end())
  {
    LOG_WARNING("Plugin already registered, ignoring registration");
    return;
  }

  PluginData pd;
  pd.view = new PluginView();
  pd.scene = new QGraphicsScene(pd.view);
  pd.view->setScene(pd.scene);
  pd.callback = callback;

  mTabs[name] = pd;

  auto action = new QAction(tr("Open") + " " + name, mMenu);
  connect(action, &QAction::triggered, [this, name] { openScene(name); });

  mMenu->addAction(action);
}
