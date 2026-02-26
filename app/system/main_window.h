// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringLiteral>

#include "common/theme.h"
#include "compiler/generator.h"
#include "config_table.h"
#include "json.h"
#include "logging.h"
#include "main_window_layout.h"
#include "result.h"
#include "widgets/notification_manager.h"

class NodeItem;
class Pipeline;
class SaveHandler;
class HostServices;
class PluginManager;
class LocalServerTab;
class SettingsManager;
class NotificationManager;
class PluginTab;
class PluginView;
class QPlainTextEdit;

class MainWindow : public MainWindowLayout
{
  Q_OBJECT

public:
  explicit MainWindow(QApplication* app, QWidget* parent = nullptr);
  ~MainWindow();

  VoidResult start();

private slots:
  void onNodeSelected(NodeItem* node, bool selected);
  void onNodeAdded(NodeItem* node);
  void onNodeRemoved(NodeItem* node);
  void onNodeModified(NodeItem* node);

  void onOpenFlow(Flow* flow, NodeItem* node);
  void onFlowAdded(Flow* flow, NodeItem* node);
  void onFlowRemoved(const QString& flowId, const QString& nodeId);

private:
  JSON mConfig;
  std::unique_ptr<SaveHandler> mSaveHandler;
  std::unique_ptr<PluginManager> mPluginManager;
  std::shared_ptr<ConfigurationTable> mConfigTable;
  std::shared_ptr<SettingsManager> mSettingsManager;

  Pipeline* mPipeline = nullptr;
  Generator* mGenerator = nullptr;
  HostServices* mHostServices = nullptr;
  Canvas* mActiveCanvas = nullptr;
  ProcessTab* mProcessTab = nullptr;
  LocalServerTab* mLocalServerTab = nullptr;
  PluginTab* mPluginTab = nullptr;
  NotificationManager* mNotificationManager = nullptr;

  logging::LogLevel mLogLevel;

  std::shared_ptr<SaveInfo> mStorage;

  QApplication* mApp;

  Canvas* canvas() const;
  Canvas* rootCanvas() const;
  VoidResult loadElements();
  VoidResult loadLibrary(const JSON& config);
  VoidResult loadElementLibrary(const QString& name, const JSON& config);

  void bind();
  void bindCanvas();
  void unbindCanvas();
  void bindShortcuts();
  void startUI();

  void onCanvasTabChanged(int index);
  void closeCanvasTab(int index);

  int libraryTypeToIndex(Types::LibraryTypes type) const;

  void onThemeChanged(const QString& t, const QList<Config::ThemeInfo>& at);

  void handleLogging(const QString& message, QTextBrowser* textBrowser);

  void addProcessTab();
  void addBrowserTab();
  void addPluginTab(const QString& name, PluginView* view);
  void addEditorTab(QPlainTextEdit* view);

  // ================================================
  // Actions
  void onActionNew();
  void onActionLoad(const QString& filename);
  void onActionSave();
  void onActionSaveAs();
  void onActionGenerate();
  void onActionSimulate();
  void onActionLogLevelChanged();
};

#endif  // MAINWINDOW_H
