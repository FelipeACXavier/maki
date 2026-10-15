#pragma once

#include <QToolButton>

class CloseButton : public QToolButton
{
  Q_OBJECT

public:
  explicit CloseButton(QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;
};
