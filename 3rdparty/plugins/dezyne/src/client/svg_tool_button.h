#pragma once

#include <QSvgRenderer>
#include <QToolButton>

class SvgToolButton : public QToolButton
{
public:
  explicit SvgToolButton(const QString& svgPath, QWidget* parent = nullptr);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  QSvgRenderer mRenderer;
};