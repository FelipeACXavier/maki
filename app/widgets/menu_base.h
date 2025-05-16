#pragma once

#include "result.h"

class NodeItem;

class MenuBase
{
public:
  virtual VoidResult onNodeAdded(NodeItem* node) = 0;
  virtual VoidResult onNodeRemoved(NodeItem* node) = 0;
  virtual VoidResult onNodeModified(NodeItem* node) = 0;
  virtual VoidResult onNodeSelected(NodeItem* node, bool selected) = 0;
};
