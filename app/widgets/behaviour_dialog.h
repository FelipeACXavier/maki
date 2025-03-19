#pragma once

#include <QDialog>
#include <QLineEdit>

class NodeItem;

class BehaviourDialog : public QDialog
{
  Q_OBJECT
public:
  BehaviourDialog(const QString& title, QWidget* parent = nullptr);

  void setup(const QList<NodeItem*>& nodes);

  QString getName() const;

private:
  QLineEdit* mName;
};
