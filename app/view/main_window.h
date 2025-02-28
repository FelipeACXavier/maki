// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringLiteral>

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
  std::unique_ptr<Ui::MainWindow> mUI;
  JSON mConfig;

  VoidResult loadElements();
  VoidResult loadElementLibrary(const JSON& config);

  void startDrag();
};

#endif  // MAINWINDOW_H
