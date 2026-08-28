#pragma once

#include <QWidget>

class QSvgWidget;

class SvgButton : public QWidget
{
  Q_OBJECT

public:
  SvgButton(const QString& svgPath, const QSize& size, QWidget* parent = nullptr);

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  QSvgWidget* mSvg = nullptr;
};