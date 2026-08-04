#pragma once

#include <QProgressBar>
#include <QWidget>

namespace maki
{
class ProgressBar : public QProgressBar
{
public:
  ProgressBar(QWidget* parent = nullptr);

  void setError(bool error);

protected:
  void paintEvent(QPaintEvent*) override;

private:
  bool mError{false};
};

}  // namespace maki