#include "plugin_tab.h"

#include "plugin_view.h"
#include "theme.h"

PluginTab::PluginTab(QObject* parent)
    : QObject(parent)
{
  mView = new PluginView();
}

PluginView* PluginTab::getView() const
{
  return mView;
}

VoidResult PluginTab::create(QGraphicsScene* tabContent)
{
  mView->setScene(tabContent);
  mView->fitInView(tabContent->sceneRect(), Qt::KeepAspectRatio);

  return VoidResult();
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
}