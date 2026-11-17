#pragma once

#include <QString>

#include "mission_parameter.h"

struct NodeEvent
{
  // The id of this node
  QString id;
  QString nodeType;
  QString parentId;

  // Empty for structural nodes.
  // Otherwise identifies the owning FlowSaveInfo.
  QString ownerFlowId;

  QString propertyId;
  maki::Value propertyValue;
};

struct FlowEvent
{
  // The id of this flow
  QString id;

  // Identified the owning NodeSaveInfo.
  QString ownerNodeId;
};

struct TransitionEvent
{
  // The id of this transition
  QString id;

  // Otherwise identifies the owning FlowSaveInfo.
  QString ownerFlowId;
};

struct ParentEvent
{
};

struct ChildEvent
{
};

struct DocumentEvent
{
  enum class Type
  {
    NodeAdded,
    NodeRemoved,
    NodeModified,
    FlowAdded,
    FlowRemoved,
    FlowModified,
    TransitionAdded,
    TransitionRemoved,
    TransitionModified,
    ParentAdded,
    ParentRemoved,
    ChildAdded,
    ChildRemoved,
    DocumentReset,
  };

  Type type;
  std::variant<NodeEvent, FlowEvent, TransitionEvent, ParentEvent, ChildEvent> data;
};

namespace maki
{
QString toString(const DocumentEvent::Type type);
}
