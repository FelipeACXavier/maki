#pragma once

#include <QGraphicsView>
#include <oclero/qlementine.hpp>

#include "itab.h"
#include "result.h"

namespace maki
{
class ITab;
}

class PluginView;

/**
 * @brief Tab manager responsible for integrating plugin-provided views into the UI.
 *
 * PluginTab allows external plugins to register graphical views that operate
 * on a QGraphicsScene. Each plugin registers a callback that populates a scene
 * when the tab is opened or updated. The class also integrates plugin entries
 * into the application menu and propagates theme changes to the plugin views.
 */
class PluginTab : public QObject, public maki::ITab
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a PluginTab instance.
   *
   * @param menu Menu where plugin entries will be added.
   * @param parent Optional Qt parent object.
   */
  PluginTab(QMenu* menu, QObject* parent = nullptr);

  /**
   * @brief Updates the scene associated with a plugin tab.
   *
   * This triggers the plugin callback responsible for populating
   * or refreshing the scene content.
   *
   * @param name Name of the plugin tab to update.
   */
  void updateScene(const QString& name) override;

  /**
   * @brief Opens the scene associated with a plugin tab.
   *
   * Emits a signal that allows the UI to display the corresponding
   * PluginView.
   *
   * @param name Name of the plugin tab to open.
   */
  void openScene(const QString& name) override;

  /**
   * @brief Returns the currently active theme variables.
   *
   * Plugins may use these variables to style their visual components
   * consistently with the rest of the application.
   *
   * @return The current theme variable set.
   */
  maki::Theme currentTheme() override;

  /**
   * @brief Registers a plugin-provided tab.
   *
   * The plugin provides a callback that receives a QGraphicsScene
   * and populates it with its visual content.
   *
   * @param name Name of the plugin tab.
   * @param callback Function responsible for populating the scene.
   */
  void registerPlugin(const QString& name, std::function<VoidResult(QGraphicsScene* scene)> callback) override;

  /**
   * @brief Notifies the tab that the application theme has changed.
   *
   * This allows plugin views to update their styling according
   * to the new theme.
   */
  void onThemeChanged();

signals:
  /**
   * @brief Signal emitted when a plugin view should be opened.
   *
   * The UI layer can connect to this signal to display the view
   * associated with the plugin.
   *
   * @param name Name of the plugin tab.
   * @param view Pointer to the PluginView instance.
   */
  void openView(const QString& name, PluginView* view);

private:
  /**
   * @brief Internal data structure storing information about a plugin tab.
   */
  struct PluginData
  {
    /// Pointer to the graphical view associated with the plugin.
    PluginView* view;

    /// Scene used by the plugin to render its graphical content.
    QGraphicsScene* scene;

    /// Callback responsible for populating or updating the scene.
    std::function<VoidResult(QGraphicsScene* scene)> callback;
  };

  /// Menu where plugin tabs are registered as actions.
  QMenu* mMenu;

  /// Mapping between plugin names and their associated data.
  QMap<QString, PluginData> mTabs;

  /**
   * @brief Converts a qlementine theme to the maki theme
   *
   * @param theme The qlemetine theme to be converted
   * @return A maki::Theme corresponding to the qlementine theme
   */
  maki::Theme qlementineToMaki(const oclero::qlementine::Theme& theme) const;
};
