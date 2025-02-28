// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringLiteral>

#include "config_table.h"
#include "json.h"
#include "result.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  VoidResult start();

private:
  JSON mConfig;
  std::unique_ptr<Ui::MainWindow> mUI;
  std::shared_ptr<ConfigurationTable> mConfigTable;

  VoidResult loadElements();
  VoidResult loadElementLibrary(const JSON& config);

  void startDrag();
};

#endif  // MAINWINDOW_H
