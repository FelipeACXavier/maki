#pragma once

#include <QGraphicsView>

#include "itab.h"
#include "result.h"

namespace maki
{
class ITab;
}

class PluginView;

class PluginTab : public QObject, public maki::ITab
{
  Q_OBJECT
public:
  PluginTab(QObject* parent = nullptr);

  VoidResult create(QGraphicsScene* tabContent) override;
  maki::ThemeVars currentTheme() override;
  maki::ThemeFonts labelFont() override;

  void onThemeChanged();

  PluginView* getView() const;

private:
  PluginView* mView;
};