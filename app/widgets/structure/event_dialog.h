#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>

class FlowSaveInfo;
class QStandardItem;
class QStandardItemModel;

class EventDialog : public QDialog
{
  Q_OBJECT
public:
  EventDialog(const QString& title, QWidget* parent = nullptr);

  void setup(std::shared_ptr<FlowSaveInfo> event);

  std::shared_ptr<FlowSaveInfo> getInfo() const;
  QString getName() const;
  QString getType() const;
  QString getReturnType() const;
  QStandardItemModel* getArguments() const;

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  std::shared_ptr<FlowSaveInfo> mStorage;
  int mEnterCount;

  void populateNodeList();
  void createNameInput(QWidget* parent);
  void createTypeInput(QWidget* parent);
  void createReturnTypeInput(QWidget* parent);
  void createArgumentInput(QWidget* parent);
  void updateArgumentTable(QStandardItem* item);
};
