// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringLiteral>
#include <oclero/qlementine.hpp>

#include "compiler/pipeline_graph.h"
#include "config_table.h"
#include "flow_info.h"
#include "focus_properties.h"
#include "json.h"
#include "logging.h"
#include "main_window_layout.h"
#include "result.h"
#include "test_and_set.h"
#include "widgets/notification_manager.h"
#include "widgets/settings_manager.h"

class Flow;
class Canvas;
class NodeItem;
class Pipeline;
class Generator;
class SaveInfo;
class SaveHandler;
class HostServices;
class PluginManager;
class SettingsManager;
class NotificationManager;
class PluginTab;
class PluginView;
class QPlainTextEdit;
class Logger;
class LanguageManager;
class EdgeRouter;

namespace maki
{
class PluginPipeline;
}

/**
 * @brief The MainWindow class is the main window of the application.
 */
class MainWindow : public MainWindowLayout
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new MainWindow object.
   *
   * @param app Pointer to the QApplication instance.
   * @param themeManager Pointer to the ThemeManager instance.
   * @param parent Pointer to the parent widget, if any.
   */
  explicit MainWindow(QApplication* app, oclero::qlementine::ThemeManager* themeManager, QWidget* parent = nullptr);

  /**
   * @brief Destroys the MainWindow object.
   */
  ~MainWindow();

  /**
   * @brief Starts the application.
   *
   * @return VoidResult indicating success or failure.
   */
  VoidResult start();

private slots:
  /**
   * @brief Handles node selection events.
   *
   * @param node Pointer to the selected NodeItem.
   * @param selected True if the node is selected, false otherwise.
   */
  void onNodeSelected(NodeItem* node, bool selected);

  /**
   * @brief Handles node addition events.
   *
   * @param node Pointer to the added NodeItem.
   */
  void onNodeAdded(NodeItem* node);

  /**
   * @brief Handles node removal events.
   *
   * @param nodeId ID of the removed node.
   * @param parentId ID of the parent node.
   */
  void onNodeRemoved(const QString& nodeId, const QString& parentId);

  /**
   * @brief Handles node modification events.
   *
   * @param node Pointer to the modified NodeItem.
   */
  void onNodeModified(NodeItem* node);

  /**
   * @brief Handles open flow events.
   *
   * @param flow Pointer to the opened Flow.
   * @param node Pointer to the associated NodeItem.
   */
  void onOpenFlow(Flow* flow, const QString& nodeId, const maki::FocusProperties& properties);

  /**
   * @brief Handles flow addition events.
   *
   * @param flow Pointer to the added Flow.
   * @param node Pointer to the associated NodeItem.
   */
  void onFlowAdded(Flow* flow, NodeItem* node);

  /**
   * @brief Handles flow removal events.
   *
   * @param flowId ID of the removed flow.
   * @param nodeId ID of the parent node.
   */
  void onFlowRemoved(const QString& flowId, NodeItem* node);

  /**
   * @brief Handles change events.
   *
   * @param event Pointer to the QEvent object.
   */
  void changeEvent(QEvent* event) override;

private:
  std::unique_ptr<SaveHandler> mSaveHandler;          /// Handler for saving data.
  std::unique_ptr<PluginManager> mPluginManager;      /// Manager for plugins.
  std::shared_ptr<ConfigurationTable> mConfigTable;   /// Configuration table shared pointer.
  std::shared_ptr<SettingsManager> mSettingsManager;  /// Settings manager shared pointer.
  std::shared_ptr<LanguageManager> mLanguageManager;  /// Language manager shared pointer.

  Pipeline* mPipeline = nullptr;                        /// Pointer to the pipeline.
  Generator* mGenerator = nullptr;                      /// Pointer to the generator.
  HostServices* mHostServices = nullptr;                /// Pointer to the host services.
  Canvas* mActiveCanvas = nullptr;                      /// Pointer to the active canvas.
  PluginTab* mPluginTab = nullptr;                      /// Pointer to the plugin tab.
  NotificationManager* mNotificationManager = nullptr;  /// Pointer to the notification manager.
  Logger* mLogger = nullptr;                            /// Pointer to the logger.
  maki::PluginPipeline* mPluginPipeline = nullptr;

  oclero::qlementine::ThemeManager* mThemeManager;  /// Pointer to the theme manager.

  std::shared_ptr<SaveInfo> mStorage;  /// Shared pointer to save information.
  std::shared_ptr<EdgeRouter> mRouter;

  QApplication* mApp;  /// Pointer to the QApplication instance.
  TestAndSet<bool> mLoading;

  /**
   * @brief Returns the active canvas.
   *
   * @return Canvas* Pointer to the active canvas.
   */
  Canvas* canvas() const;

  /**
   * @brief Returns the root canvas.
   *
   * @return Canvas* Pointer to the root canvas.
   */
  Canvas* rootCanvas() const;

  /**
   * @brief Loads elements into the application.
   *
   * @return VoidResult indicating success or failure.
   */
  VoidResult loadElements();

  /**
   * @brief Loads a library based on the provided configuration.
   *
   * @param config JSON object containing the library configuration.
   * @return VoidResult indicating success or failure.
   */
  VoidResult loadLibrary(const JSON& config, bool dataOnly);

  /**
   * @brief Loads an element library based on the provided name and configuration.
   *
   * @param name Name of the library to load.
   * @param config JSON object containing the library configuration.
   * @return VoidResult indicating success or failure.
   */
  VoidResult loadElementLibrary(const QString& name, const JSON& config, bool dataOnly);

  /**
   * @brief Binds signals and slots for the application.
   */
  void bind();

  /**
   * @brief Binds signals and slots for the canvas.
   */
  void bindCanvas();

  /**
   * @brief Unbinds signals and slots for the canvas.
   */
  void unbindCanvas();

  /**
   * @brief Binds shortcuts to actions.
   */
  void bindShortcuts();

  /**
   * @brief Starts the user interface.
   */
  void startUI();

  /**
   * @brief Handles changes in the canvas tab.
   *
   * @param index Index of the changed tab.
   */
  void onCanvasTabChanged(int index);

  /**
   * @brief Closes a canvas tab at the specified index.
   *
   * @param index Index of the tab to close.
   */
  void closeCanvasTab(int index);

  /**
   * @brief Converts a library type to its corresponding index.
   *
   * @param type Library type to convert.
   * @return int Index corresponding to the library type.
   */
  int libraryTypeToIndex(Types::LibraryTypes type) const;

  /**
   * @brief Handles theme change events.
   *
   * @param t Name of the new theme.
   * @param at List of theme information.
   */
  void onThemeChanged(const AppearanceSettings& themeName, bool intialConfig);

  /**
   * @brief Handles settings change events.
   */
  void onSettingsChanged();

  /**
   * @brief Shows the about dialog.
   */
  void showAboutDialog();

  /**
   * @brief Adds a browser tab to the application.
   */
  void addBrowserTab();

  /**
   * @brief Adds a plugin tab with the specified name and view.
   *
   * @param name Name of the plugin tab.
   * @param view Pointer to the PluginView instance.
   */
  void addPluginTab(const QString& name, PluginView* view);

  /**
   * @brief Removes a plugin tab with the specified name
   *
   * @param view Pointer to the PluginView instance to be removed;
   */
  void removePluginTab(PluginView* view);

  /**
   * @brief Adds an editor tab with the specified view.
   *
   * @param view Pointer to the QPlainTextEdit instance.
   */
  void addEditorTab(QPlainTextEdit* view);

  // ================================================
  // Actions
  void onActionNew();                                      /// Handles new action.
  void onActionLoad(const QString& filename);              /// Handles load action with a specified file name.
  VoidResult onActionSave();                               /// Handles save action.
  void onActionExit();                                     /// Handles exit action.
  void onActionRestart();                                  /// Handles restart action.
  void onActionSaveAs();                                   /// Handles save as action.
  void onActionGenerate(const QString& pipelineId);        /// Handles generate action.
  void onActionDeletePipeline(const QString& pipelineId);  /// Handles pipeline deletion.
  void onActionEditPipeline(const QString& pipelineId);    /// Handles generate action.
  void onActionLogLevelChanged();                          /// Handles log level change action.
  void onFileLoaded(const QString& file, const SaveInfo& info, const QString& error);

  void suggestCapability(NodeItem* node);
};  // namespace qlementclassMainWindow:public MainWindowLayout

#endif  // MAINWINDOW_H
