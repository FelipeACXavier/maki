#pragma once

#include <QWidget>

class CanvasView;
class NodeItem;
class QTreeWidget;

/** Floating menu (QTreeWidget) with Add flow / Add subtask actions beside a selected Task. */
class NodeActionMenu : public QWidget
{
  Q_OBJECT

public:
  explicit NodeActionMenu(QWidget* parent = nullptr);

  void showForTask(NodeItem* task, CanvasView* view);
  void updatePosition(CanvasView* view);
  void hideMenu();

  QString trackedTaskId() const;

signals:
  void addFlowRequested(NodeItem* task);
  void addSubtaskRequested(NodeItem* task);

private:
  QTreeWidget* mTree = nullptr;
  NodeItem* mTask = nullptr;
};
