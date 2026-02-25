#include "plugin_tab.h"

#include "logging.h"
#include "plugin_view.h"
#include "theme.h"

PluginTab::PluginTab(QObject* parent)
    : QObject(parent)
{
  mView = new PluginView();
  mScene = new QGraphicsScene(mView);
  mView->setScene(mScene);
}

PluginView* PluginTab::getView() const
{
  return mView;
}

void PluginTab::updateScene()
{
  for (const auto& callback : mCallbacks)
  {
    if (callback)
    {
      LOG_WARN_ON_FAILURE(callback(mScene));
      mView->fitInView(mScene->sceneRect(), Qt::KeepAspectRatio);
    }
  }
}

maki::ThemeVars PluginTab::currentTheme()
{
  return Config::SYSTEM_THEME;
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
  updateScene();
}

void PluginTab::registerAppearenceUpdate(std::function<VoidResult(QGraphicsScene* scene)> callback)
{
  mCallbacks.push_back(callback);
}