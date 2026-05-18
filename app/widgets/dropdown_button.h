#pragma once

#include <QToolButton>

#include "compiler/pipeline_graph.h"

class DropDownButton : public QToolButton
{
  Q_OBJECT

public:
  DropDownButton(QWidget* parent = nullptr);

  void addOption(const QString& name);
  void setCurrentOption(const QString& name);
  QString currentOption();

  // void setBaseText(const QString& text);

  void reset();

signals:
  void executeRequested(const QString& option);
  void editOptionRequested(const QString& option);

private:
  QMenu* mMenu = nullptr;
  QList<QString> mOptions;
  QString mCurrentOption;

  void rebuildMenu();
  void updateButtonText();
  void showContextMenu(const QPoint& point);
  void buildMenu(QMenu* menu, const QString& option, bool addRun);
};