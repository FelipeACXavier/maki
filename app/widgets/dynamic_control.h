#pragma once

#include <QWidget>

#include "result.h"
#include "types.h"

class NodeItem;

class DynamicControl : public QWidget
{
public:
  DynamicControl(Types::ControlTypes type, QWidget* parent);

  VoidResult setupAddField(NodeItem* node);

private slots:
  void onCustomContextMenuRequested(const QPoint& pos);
  void onRemoveRequested();
};
