#pragma once

#include <qboxlayout.h>

#include <QDialog>

#include "style_helpers.h"

class BaseDialog : public QDialog
{
  Q_OBJECT
public:
  BaseDialog(const QString& title, QWidget* parent = nullptr);

public slots:
  void onThemeChanged();

protected:
  QList<WidgetWithIcon> mIcons;

  void addIcon(QWidget* widget, const QString& iconPath);
  void setSize();
  QVBoxLayout* layout();
};
