#include "flow_menu.h"

FlowMenu::FlowMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
}
