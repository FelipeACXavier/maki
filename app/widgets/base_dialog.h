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

protected:
  void setSize(double ratio, double screenFraction);
  QVBoxLayout* layout();
  QDialogButtonBox* createButtons(const QString& ok, const QString& cancel);
};
