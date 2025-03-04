#pragma once

#include <QFrame>

#include "elements/config.h"
#include "result.h"

class NodeItem;
class QHBoxLayout;

class PropertiesMenu : public QFrame
{
public:
  PropertiesMenu(QWidget* parent);

  VoidResult onNodeSelected(NodeItem* node);

private:
  void clear();

  VoidResult loadProperties(NodeItem* node);
  VoidResult loadPropertyInt(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyReal(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertySelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyString(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node);

  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddField(const ControlsConfig& control, QWidget* parent, QHBoxLayout* controlLayout);

  void addDynamicWidget(QWidget* control, QWidget* parent);
};
