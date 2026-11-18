#include "document_event.h"

namespace maki
{
QString toString(const DocumentEvent::Type type)
{
  switch (type)
  {
    case DocumentEvent::Type::NodeAdded:
      return "NodeAdded";
    case DocumentEvent::Type::NodeRemoved:
      return "NodeRemoved";
    case DocumentEvent::Type::NodeModified:
      return "NodeModified";
    case DocumentEvent::Type::FlowAdded:
      return "FlowAdded";
    case DocumentEvent::Type::FlowRemoved:
      return "FlowRemoved";
    case DocumentEvent::Type::FlowModified:
      return "FlowModified";
    case DocumentEvent::Type::TransitionAdded:
      return "TransitionAdded";
    case DocumentEvent::Type::TransitionRemoved:
      return "TransitionRemoved";
    case DocumentEvent::Type::TransitionModified:
      return "TransitionModified";
    case DocumentEvent::Type::ParentAdded:
      return "ParentAdded";
    case DocumentEvent::Type::ParentRemoved:
      return "ParentRemoved";
    case DocumentEvent::Type::ChildAdded:
      return "ChildAdded";
    case DocumentEvent::Type::ChildRemoved:
      return "ChildRemoved";
    case DocumentEvent::Type::PipelineAdded:
      return "PipelineAdded";
    case DocumentEvent::Type::PipelineRemoved:
      return "PipelineRemoved";
    case DocumentEvent::Type::PipelineModified:
      return "PipelineModified";
    case DocumentEvent::Type::DocumentReplaced:
      return "DocumentReplaced";
    case DocumentEvent::Type::DocumentReset:
      return "DocumentReset";
  }

  return "Unknown";
}
}  // namespace maki
