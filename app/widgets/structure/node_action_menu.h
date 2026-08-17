#pragma once

#include <QWidget>

class CanvasView;
class NodeItem;
class QTreeWidget;
class QGraphicsOpacityEffect;
class QPropertyAnimation;

class NodeActionRow : public QWidget
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
  void openMainFlowRequested(NodeItem* task);
  void addFlowRequested(NodeItem* task);
  void addSubtaskRequested(NodeItem* task);

private:
  QTreeWidget* mTree = nullptr;
  NodeItem* mTask = nullptr;
  QTimer* mFadeTimer = nullptr;

  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
};
