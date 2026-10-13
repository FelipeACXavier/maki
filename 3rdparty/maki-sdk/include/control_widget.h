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
  virtual void activate();
  virtual void setTouring();

signals:
  void activated();
  void dismissed();
};
}  // namespace maki