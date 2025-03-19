#pragma once

#include <QFrame>
#include <QListWidget>

#include "config.h"
#include "result.h"

class Canvas;
class NodeItem;
class BehaviourDialog;

class BehaviourMenu : public QFrame
{
  Q_OBJECT
public:
  BehaviourMenu(QWidget* parent);

  VoidResult onNodeSelected(NodeItem* node, bool selected);
  VoidResult onNodeRemoved(NodeItem* node);

  std::function<QList<NodeItem*>()> mGetAvailableNodes;

private slots:
  void addBehaviour();
  void editBehaviour(QListWidgetItem* item);
  void showContextMenu(const QPoint& pos);

  void onDialogAccepted();
  void onDialogRejected();

private:
  QString mCurrentNode;
  QListWidget* mBehaviourList;
  BehaviourDialog* mCurrentDialog;

  void clear();
  void setGlobalView();

  void addDynamicWidget(QWidget* dynamicWidget, QWidget* parent);
};
