#pragma once

#include <QMouseEvent>
#include <QTabBar>

namespace maki
{
class EditorTabBar final : public QTabBar
{
  Q_OBJECT
public:
  explicit EditorTabBar(QWidget* parent = nullptr);

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

private:
  QPoint mPressPosition;
  int mPressIndex = -1;

  QPixmap createDragPreview(int index) const;
};
}  // namespace maki
