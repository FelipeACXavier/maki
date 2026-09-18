#pragma once

#include <QWidget>

namespace maki
{
class ControlWidget : public QWidget
{
  Q_OBJECT
public:
  ControlWidget(QWidget* parent = nullptr);

  virtual void start();
signals:
  void dismissed();
};
}  // namespace maki