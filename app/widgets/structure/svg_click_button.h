#pragma once

#include <QWidget>

class QSvgWidget;

/** Small clickable widget that renders a multi-color SVG without theme recoloring. */
class SvgClickButton : public QWidget
{
  Q_OBJECT

public:
  explicit SvgClickButton(const QString& svgPath, const QSize& size, QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  QSvgWidget* mSvg = nullptr;
};
