#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>

#include "property_config.h"

class QStandardItem;
class QStandardItemModel;

class FieldDialog : public QDialog
{
  Q_OBJECT
public:
  FieldDialog(const QString& title, QWidget* parent = nullptr);

  void setup(const PropertiesConfig& config);

  PropertiesConfig getInfo() const;
  QString getName() const;
  QString getType() const;
  QString getReturnType() const;
  QStandardItemModel* getArguments() const;

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  PropertiesConfig mStorage;
  int mEnterCount;

  void populateNodeList();
  void createNameInput(QWidget* parent);
  void createTypeInput(QWidget* parent);
  void createArgumentInput(QWidget* parent);
  void updateArgumentTable(QStandardItem* item);

  void addEnumField(QWidget* parent);
  void removeEnumField();
};
