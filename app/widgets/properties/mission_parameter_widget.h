#pragma once

#include <QTableWidget>
#include <QWidget>

#include "mission_parameter.h"

namespace maki
{
class MissionParameterWidget : public QWidget
{
  Q_OBJECT

public:
  MissionParameterWidget(QWidget* parent = nullptr);

  void addParameter();
  void editParameter(const QString& parameterId);

  void setParameters(QVector<MissionParameter> parameters);

  void documentUpdated();

signals:
  void parametersChanged();

private:
  QTableWidget* mTable = nullptr;

  void editParameter(int index);
  void removeParameter();
  void rebuildTable();
};
}  // namespace maki