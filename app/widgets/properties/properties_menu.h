#pragma once

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"

class TransitionItem;
class NodeItem;
class QTableView;
class QHBoxLayout;
class SaveInfo;

class PropertiesMenu : public QFrame, public MenuBase
{
public:
  PropertiesMenu(QWidget* parent);

  VoidResult start(std::shared_ptr<SaveInfo> storage);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  VoidResult onTransitionSelected(TransitionItem* transition);

private:
  QString mCurrentNode;
  std::shared_ptr<SaveInfo> mStorage;

  void clear();

  VoidResult loadProperties(NodeItem* node);
  VoidResult loadPropertyInt(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyReal(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyColor(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertySelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyString(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyStateSelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyEventSelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyComponentSelect(const PropertiesConfig& property, NodeItem* node);
};
