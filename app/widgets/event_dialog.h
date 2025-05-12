#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>

class EventConfig;
class QStandardItemModel;

class EventDialog : public QDialog
{
  Q_OBJECT
public:
  EventDialog(const QString& title, QWidget* parent = nullptr);

  void setup(const EventConfig& event);

  QString getName() const;
  QString getType() const;
  QString getReturnType() const;
  QStandardItemModel* getArguments() const;

private:
  QLineEdit* mName;
  QComboBox* mType;
  QComboBox* mReturnType;
  QTableView* mArguments;

  void populateNodeList();
};
