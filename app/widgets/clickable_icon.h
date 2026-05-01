#pragma once

#include <QMouseEvent>
#include <oclero/qlementine/widgets/IconWidget.hpp>

class ClickableIcon : public oclero::qlementine::IconWidget
{
  Q_OBJECT
public:
  ClickableIcon(const QIcon& icon, QWidget* parent = nullptr);
  ClickableIcon(const QIcon& icon, const QSize& size, QWidget* parent = nullptr);

  void setCheckable(bool checkable);
  void setChecked(bool check);
  bool isChecked();

signals:
  void clicked();
  void toggled(bool check);

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  bool mChecked;
  bool mCheckable;
};