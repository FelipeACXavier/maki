#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QTableView>

#include "widgets/base_dialog.h"

class QStandardItem;
class QStandardItemModel;

struct FlowSaveInfo;

class EventDialog : public BaseDialog
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
  void createNameInput();
  void createTypeInput();
  void createReturnTypeInput();
  void createArgumentInput();
  void updateArgumentTable(int row, int column, const QString& text);
};
