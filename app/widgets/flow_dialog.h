#pragma once

#include <QDialog>
#include <QListWidget>

#include "system/canvas_view.h"

class ConfigurationTable;

class FlowDialog : public QDialog
{
  Q_OBJECT
public:
  FlowDialog(const QString& title, QWidget* parent = nullptr);

  void setup(std::shared_ptr<ConfigurationTable> configTable);

protected:
  CanvasView* mCanvasView;
};
