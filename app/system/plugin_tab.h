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
  PluginTab(QMenu* menu, QObject* parent = nullptr);

  void updateScene(const QString& name) override;
  void openScene(const QString& name) override;
  maki::ThemeVars currentTheme() override;
  maki::ThemeFonts labelFont() override;
  void registerAppearenceUpdate(const QString& name, std::function<VoidResult(QGraphicsScene* scene)> callback) override;

  void onThemeChanged();

signals:
  void openView(const QString& name, PluginView* view);

private:
  struct PluginData
  {
    PluginView* view;
    QGraphicsScene* scene;
    std::function<VoidResult(QGraphicsScene* scene)> callback;
  };

  QMenu* mMenu;
  QMap<QString, PluginData> mTabs;
};