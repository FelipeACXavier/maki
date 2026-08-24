#pragma once

#include <oclero/qlementine/widgets/Label.hpp>

namespace maki
{
class ClickableLabel : public oclero::qlementine::Label
{
  Q_OBJECT

public:
  ClickableLabel(QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void mouseDoubleClickEvent(QMouseEvent* event) override;
};
}  // namespace maki