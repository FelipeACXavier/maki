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
// Call type
Types::CallType Types::StringToCallType(const QString& type)
{
  if (type == "trigger")
    return Types::CallType::TRIGGER;
  else if (type == "abort")
    return Types::CallType::ABORT;
  else if (type == "error")
    return Types::CallType::ERROR;
  else if (type == "return")
    return Types::CallType::RETURN;
  else if (type == "user")
    return Types::CallType::USER;
  else if (type == "in")
    return Types::CallType::IN;
  else if (type == "out")
    return Types::CallType::OUT;

  return Types::CallType::UNKNOWN;
}

QString Types::CallTypeToString(CallType type)
{
  if (type == Types::CallType::TRIGGER)
    return "trigger";
  else if (type == Types::CallType::ABORT)
    return "abort";
  else if (type == Types::CallType::ERROR)
    return "error";
  else if (type == Types::CallType::RETURN)
    return "return";
  else if (type == Types::CallType::USER)
    return "user";
  else if (type == Types::CallType::IN)
    return "in";
  else if (type == Types::CallType::OUT)
    return "out";

  return "unknown";
}

// =====================================================================================
// Property type
Types::PropertyTypes Types::plus(PropertyTypes type, uint16_t amount)
{
  uint16_t sum = ((uint16_t)type + amount);
  if (sum < (uint16_t)Types::PropertyTypes::UNKNOWN)
    return Types::PropertyTypes::UNKNOWN;
  if (sum >= (uint16_t)Types::PropertyTypes::END)
    return Types::PropertyTypes::UNKNOWN;

  return (PropertyTypes)sum;
}

Types::PropertyTypes Types::minus(PropertyTypes type, uint16_t amount)
{
  uint16_t sum = ((uint16_t)type - amount);
  if (sum < (uint16_t)Types::PropertyTypes::UNKNOWN)
    return Types::PropertyTypes::UNKNOWN;
  if (sum >= (uint16_t)Types::PropertyTypes::END)
    return Types::PropertyTypes::UNKNOWN;

  return (PropertyTypes)sum;
}

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
  else if (type == Types::PropertyTypes::LIST)
    return "list";
  else if (type == Types::PropertyTypes::VOID)
    return "void";
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
  else if (type == "list")
    return Types::PropertyTypes::LIST;
  else if (type == "color")
    return Types::PropertyTypes::VOID;
  else if (type == "enum")
    return Types::PropertyTypes::ENUM;

  return Types::PropertyTypes::UNKNOWN;
}

// =====================================================================================
// Control type
Types::ControlTypes Types::StringToControlTypes(const QString& type)
{
  if (type == "add event")
    return Types::ControlTypes::ADD_EVENT;
  else if (type == "add field")
    return Types::ControlTypes::ADD_FIELD;
  else if (type == "add state")
    return Types::ControlTypes::ADD_STATE;
  else if (type == "color")
    return Types::ControlTypes::COLOR;
  else if (type == "select")
    return Types::ControlTypes::SELECT;
  else if (type == "component_select")
    return Types::ControlTypes::COMPONENT_SELECT;
  else if (type == "event_select")
    return Types::ControlTypes::EVENT_SELECT;
  else if (type == "trigger_call")
    return Types::ControlTypes::TRIGGER_CALL;
  else if (type == "flow_call")
    return Types::ControlTypes::FLOW_CALL;
  else
    return Types::ControlTypes::AUTO;
}

QString Types::ControlTypesToString(Types::ControlTypes type)
{
  switch (type)
  {
    case Types::ControlTypes::ADD_FIELD:
      return "add field";
    case Types::ControlTypes::ADD_EVENT:
      return "add event";
    case Types::ControlTypes::ADD_STATE:
      return "add state";
    case Types::ControlTypes::COLOR:
      return "color";
    case Types::ControlTypes::SELECT:
      return "select";
    case Types::ControlTypes::COMPONENT_SELECT:
      return "component_select";
    case Types::ControlTypes::EVENT_SELECT:
      return "event_select";
    case Types::ControlTypes::TRIGGER_CALL:
      return "trigger_call";
    case Types::ControlTypes::FLOW_CALL:
      return "flow_call";
    case Types::ControlTypes::AUTO:
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
  else if (type == Types::LibraryTypes::PIPELINE)
    return "pipeline";

  return "unknown";
}
