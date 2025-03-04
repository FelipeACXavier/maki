#pragma once

#include <QWidget>

#include "result.h"
#include "types.h"

class DynamicControl : public QWidget
{
public:
  DynamicControl(QWidget* parent);

  VoidResult setup(Types::ControlTypes type);

private slots:
  void onCustomContextMenuRequested(const QPoint& pos);
  void onRemoveRequested();

private:
  VoidResult setupAddField();
};
