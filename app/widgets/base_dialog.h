#pragma once

#include <qboxlayout.h>

#include <QDialog>

#include "style_helpers.h"

class QDialogButtonBox;

class BaseDialog : public QDialog
{
  Q_OBJECT
public:
  BaseDialog(const QString& title, double ratio, double screenFraction, QWidget* parent = nullptr);

public slots:
  void onThemeChanged();

protected:
  QList<WidgetWithIcon> mIcons;

  void addIcon(QWidget* widget, const QString& iconPath);
  void setSize(double ratio, double screenFraction);
  QVBoxLayout* layout();
  QDialogButtonBox* createButtons(const QString& ok, const QString& cancel);
};
