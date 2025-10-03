#include "types.h"

// =====================================================================================
// Connector type
Types::ConnectorType Types::StringToConnectorType(const QString& type)
{
  if (type == "in")
    return Types::ConnectorType::IN;
  else if (type == "out")
    return Types::ConnectorType::OUT;
  else if (type == "inout")
    return Types::ConnectorType::IN_AND_OUT;

  return Types::ConnectorType::UNKNOWN;
}

QString Types::ConnectorTypeToString(Types::ConnectorType type)
{
  if (type == Types::ConnectorType::IN)
    return "in";
  else if (type == Types::ConnectorType::OUT)
    return "out";
  else if (type == Types::ConnectorType::IN_AND_OUT)
    return "inout";

  return "unknown";
}

// =====================================================================================
// Property type
QString Types::PropertyTypesToString(Types::PropertyTypes type)
{
  if (type == Types::PropertyTypes::STRING)
    return "string";
  else if (type == Types::PropertyTypes::INTEGER)
    return "integer";
  else if (type == Types::PropertyTypes::REAL)
    return "float";
  else if (type == Types::PropertyTypes::BOOLEAN)
    return "bool";
  else if (type == Types::PropertyTypes::SELECT)
    return "select";
  else if (type == Types::PropertyTypes::LIST)
    return "list";
  else if (type == Types::PropertyTypes::COLOR)
    return "color";
  else if (type == Types::PropertyTypes::VOID)
    return "void";
  else if (type == Types::PropertyTypes::COMPONENT_SELECT)
    return "component_select";
  else if (type == Types::PropertyTypes::EVENT_SELECT)
    return "event_select";
  else if (type == Types::PropertyTypes::STATE_SELECT)
    return "state_select";
  else if (type == Types::PropertyTypes::SET_STATE)
    return "set_state";
  else if (type == Types::PropertyTypes::ENUM)
    return "enum";

  return "unknown";
}

Types::PropertyTypes Types::StringToPropertyTypes(const QString& type)
{
  if (type == "string")
    return Types::PropertyTypes::STRING;
  else if (type == "integer" || type == "int")
    return Types::PropertyTypes::INTEGER;
  else if (type == "real" || type == "float" || type == "double")
    return Types::PropertyTypes::REAL;
  else if (type == "boolean" || type == "bool")
    return Types::PropertyTypes::BOOLEAN;
  else if (type == "select")
    return Types::PropertyTypes::SELECT;
  else if (type == "list")
    return Types::PropertyTypes::LIST;
  else if (type == "color")
    return Types::PropertyTypes::COLOR;
  else if (type == "void")
    return Types::PropertyTypes::VOID;
  else if (type == "event_select")
    return Types::PropertyTypes::EVENT_SELECT;
  else if (type == "component_select")
    return Types::PropertyTypes::COMPONENT_SELECT;
  else if (type == "state_select")
    return Types::PropertyTypes::STATE_SELECT;
  else if (type == "set_state")
    return Types::PropertyTypes::SET_STATE;
  else if (type == "enum")
    return Types::PropertyTypes::ENUM;

  return Types::PropertyTypes::UNKNOWN;
}

// =====================================================================================
// Control type
Types::ControlTypes Types::StringToControlTypes(const QString& type)
{
  if (type == "add field")
    return Types::ControlTypes::ADD_FIELD;
  else if (type == "add control")
    return Types::ControlTypes::ADD_CONTROL;
  else if (type == "remove control")
    return Types::ControlTypes::REMOVE_CONTROL;
  else if (type == "add event")
    return Types::ControlTypes::ADD_EVENT;
  else if (type == "add state")
    return Types::ControlTypes::ADD_STATE;
  else
    return Types::ControlTypes::UNKNOWN;
}

QString Types::ControlTypesToString(Types::ControlTypes type)
{
  switch (type)
  {
    case Types::ControlTypes::ADD_FIELD:
      return "add field";
    case Types::ControlTypes::ADD_CONTROL:
      return "add control";
    case Types::ControlTypes::REMOVE_CONTROL:
      return "remove control";
    case Types::ControlTypes::ADD_EVENT:
      return "add event";
    case Types::ControlTypes::ADD_STATE:
      return "add state";
    case Types::ControlTypes::UNKNOWN:
    default:
      return "unknown";
  }
}

// =====================================================================================
// Library type
QString Types::LibraryTypeToString(Types::LibraryTypes type)
{
  if (type == Types::LibraryTypes::STRUCTURAL)
    return "structural";
  else if (type == Types::LibraryTypes::BEHAVIOUR)
    return "behavioural";

  return "unknown";
}
