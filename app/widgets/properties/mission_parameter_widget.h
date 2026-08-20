#pragma once

#include <QTableWidget>
#include <QWidget>

#include "mission_parameter.h"
#include "save_info.h"

namespace maki
{
class MissionParameterWidget : public QWidget
{
  Q_OBJECT

public:
  MissionParameterWidget(QWidget* parent = nullptr);

  void addParameter();

  void setParameters(QVector<MissionParameter> parameters);

  void setStorage(std::shared_ptr<SaveInfo> storage);

signals:
  void parametersChanged();

private:
  QTableWidget* mTable = nullptr;
  std::shared_ptr<SaveInfo> mStorage;

  void editParameter(int index);
  void removeParameter();
  void rebuildTable();
};
}  // namespace maki