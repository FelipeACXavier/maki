#pragma once

#include <QToolButton>

class CloseButton : public QToolButton
{
  Q_OBJECT

public:
  CloseButton(QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;
};