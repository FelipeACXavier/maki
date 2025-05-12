#pragma once

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
    return "real";
  else if (type == Types::PropertyTypes::BOOLEAN)
    return "boolean";
  else if (type == Types::PropertyTypes::SELECT)
    return "select";
  else if (type == Types::PropertyTypes::LIST)
    return "list";
  else if (type == Types::PropertyTypes::COLOR)
    return "color";
  else if (type == Types::PropertyTypes::VOID)
    return "void";

  return "unknown";
}

Types::PropertyTypes Types::StringToPropertyTypes(const QString& config)
{
  const auto type = config;
  if (type == "string")
    return Types::PropertyTypes::STRING;
  else if (type == "integer" || type == "int")
    return Types::PropertyTypes::INTEGER;
  else if (type == "real")
    return Types::PropertyTypes::REAL;
  else if (type == "boolean")
    return Types::PropertyTypes::BOOLEAN;
  else if (type == "select")
    return Types::PropertyTypes::SELECT;
  else if (type == "list")
    return Types::PropertyTypes::LIST;
  else if (type == "color")
    return Types::PropertyTypes::COLOR;
  else if (type == "void")
    return Types::PropertyTypes::VOID;

  return Types::PropertyTypes::UNKNOWN;
}
