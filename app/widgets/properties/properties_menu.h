#pragma once

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"

class NodeItem;
class QTableView;
class QHBoxLayout;

class PropertiesMenu : public MenuBase, public QFrame
{
public:
  PropertiesMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

private:
  QString mCurrentNode;

  void clear();

  VoidResult loadProperties(NodeItem* node);
  VoidResult loadPropertyInt(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyReal(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyColor(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertySelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyString(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node);
};
