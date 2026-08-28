#pragma once

#include <QWidget>

#include "../frame.h"
#include "control_widget.h"

class CanvasView;
class NodeItem;
class QTreeWidget;
class QGraphicsOpacityEffect;
class QPropertyAnimation;

class NodeActionRow : public StyledFrame
{
  Q_OBJECT
public:
  NodeActionRow(const QString& svgPath, const QString& labelText, QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;

  void paintEvent(QPaintEvent* event) override;

private:
  bool mHovered;
  void setHovered(bool hovered);
};

/** Floating menu beside a hovered Task: open main flow / add flow / add subtask. */
class TaskNodeMenu : public maki::ControlWidget
{
  Q_OBJECT

public:
  static TaskNodeMenu* create(QWidget* parent = nullptr);

  TaskNodeMenu(QWidget* parent = nullptr);

signals:
  void openMainFlowRequested();
  void addFlowRequested();
  void addSubtaskRequested();

private:
  QTreeWidget* mTree = nullptr;
  void hideMenu();
};