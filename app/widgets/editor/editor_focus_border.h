#pragma once

#include <QWidget>

namespace maki
{
class EditorFocusBorder : public QWidget
{
  Q_OBJECT
public:
  explicit EditorFocusBorder(QWidget* parent = nullptr);
  void setActive(bool active);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool mActive = false;
};
}  // namespace maki