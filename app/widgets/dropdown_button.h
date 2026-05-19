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

  void setRunning(bool running);
  void setSize(int width, int height);
  void reset();

signals:
  void executeRequested(const QString& option);
  void editOptionRequested(const QString& option);

protected:
  QSize sizeHint() const override;
  void paintEvent(QPaintEvent* event) override;

private:
  QMenu* mMenu = nullptr;
  QList<QString> mOptions;
  QString mCurrentOption;
  int mWidth = 150;
  int mHeight = 40;
  bool mRunning = false;

  void rebuildMenu();
  void updateButtonText();
  void showContextMenu(const QPoint& point);
  void buildMenu(QMenu* menu, const QString& option, bool addRun);
};
