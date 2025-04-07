// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringLiteral>

#include "compiler/generator.h"
#include "config_table.h"
#include "json.h"
#include "logging.h"
#include "result.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class SaveHandler;
class PluginManager;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  VoidResult start();

private slots:
  void onNodeSelected(NodeItem* node, bool selected);
  void onNodeAdded(NodeItem* node);
  void onNodeRemoved(NodeItem* node);
  void onNodeModified(NodeItem* node);

private:
  JSON mConfig;
  std::unique_ptr<Ui::MainWindow> mUI;
  std::unique_ptr<SaveHandler> mSaveHandler;
  std::unique_ptr<PluginManager> mPluginManager;
  std::shared_ptr<ConfigurationTable> mConfigTable;

  std::shared_ptr<Generator> mGenerator;

  logging::LogLevel mLogLevel;

  Canvas* canvas() const;
  VoidResult loadElements();
  VoidResult loadLibrary(const JSON& config);
  VoidResult loadElementLibrary(const QString& name, const JSON& config);

  void bind();
  void bindShortcuts();
  void startUI();

  // ================================================
  // Actions
  void onActionLoad();
  void onActionSave();
  void onActionSaveAs();
  void onActionGenerate();
  void onActionLogLevelChanged();
};

#endif  // MAINWINDOW_H
