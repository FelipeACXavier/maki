#pragma once

#include <QFrame>
#include <QListWidget>

#include "config.h"
#include "result.h"

class NodeItem;

class BehaviourMenu : public QFrame
{
  Q_OBJECT
public:
  BehaviourMenu(QWidget* parent);

  VoidResult onNodeSelected(NodeItem* node);
  VoidResult onNodeRemoved(NodeItem* node);

  std::function<QList<NodeItem*>()> mGetAvailableNodes;

private slots:
  void addBehaviour();
  void editBehaviour(QListWidgetItem* item);
  void showContextMenu(const QPoint& pos);

private:
  QString mCurrentNode;
  QListWidget* mBehaviourList;

  void clear();
  void setGlobalView();

  void addDynamicWidget(QWidget* dynamicWidget, QWidget* parent);
};
