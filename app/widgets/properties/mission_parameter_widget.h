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

  const std::vector<MissionParameter>& parameters() const;
  void setParameters(std::vector<MissionParameter> parameters);

  void addParameter();

signals:
  void parametersChanged();

private:
  QTableWidget* mTable = nullptr;
  std::vector<MissionParameter> mParameters;

  void editParameter(size_t index);
  void removeParameter();
  void rebuildTable();
};
}  // namespace maki