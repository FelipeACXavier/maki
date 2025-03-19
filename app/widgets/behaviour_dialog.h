#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

class NodeItem;

class BehaviourDialog : public QDialog
{
  Q_OBJECT
public:
  BehaviourDialog(const QString& title, QWidget* parent = nullptr);

  void setup(const QList<NodeItem*>& nodes);

  QString getName() const;

public slots:
  void nodeSelected(NodeItem* node, bool selected);

private:
  QLineEdit* mName;
  QListWidget* mListWidget;

  void populateNodeList();
};
