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

class SaveHandler;
class PluginManager;
class SettingsManager;

class MainWindow : public MainWindowlayout
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  VoidResult start();

  std::shared_ptr<SettingsManager> settingsManager() const;

private slots:
  void onNodeSelected(NodeItem* node, bool selected);
  void onNodeAdded(NodeItem* node);
  void onNodeRemoved(NodeItem* node);
  void onNodeModified(NodeItem* node);

  void onOpenFlow(Flow* flow, NodeItem* node);
  void onFlowAdded(Flow* flow, NodeItem* node);
  void onFlowRemoved(const QString& flowId, NodeItem* node);

private:
  JSON mConfig;
  std::unique_ptr<SaveHandler> mSaveHandler;
  std::unique_ptr<PluginManager> mPluginManager;
  std::shared_ptr<ConfigurationTable> mConfigTable;
  std::shared_ptr<SettingsManager> mSettingsManager;

  std::shared_ptr<Generator> mGenerator;
  Canvas* mActiveCanvas;

  logging::LogLevel mLogLevel;

  std::shared_ptr<SaveInfo> mStorage;

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

  // ================================================
  // Actions
  void onActionNew();
  void onActionLoad();
  void onActionSave();
  void onActionSaveAs();
  void onActionGenerate();
  void onActionLogLevelChanged();
};

#endif  // MAINWINDOW_H
