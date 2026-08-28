#pragma once

#include <QWidget>

namespace maki
{
class ControlWidget : public QWidget
{
  Q_OBJECT
public:
  ControlWidget(QWidget* parent = nullptr);

signals:
  void dismissed();
};
}  // namespace maki