#pragma once

#include <QPointF>
#include <QString>

namespace Types
{
enum class Shape
{
  UNKNOWN = 0,
  RECTANGLE,
  ROUNDED_RECTANGLE,
  ELLIPSE,
  DIAMOND
};

enum class AlignmentMode
{
  VERTICAL,
  HORIZONTAL
};

enum class AlignmentDirection
{
  CENTER,
  START,
  END
};

struct AlignmentNode
{
  QString id;
  QPointF pos;
};

enum class ConnectorType
{
  UNKNOWN = 0,
  IN,
  OUT,
  IN_AND_OUT,
  END
};

ConnectorType StringToConnectorType(const QString& type);
QString ConnectorTypeToString(ConnectorType type);

enum class CallType
{
  UNKNOWN = 0,
  TRIGGER,
  ABORT,
  ERROR,
  RETURN,
  USER,
  IN,
  OUT,
  END
};

CallType StringToCallType(const QString& type);
QString CallTypeToString(CallType type);

enum class PropertyTypes
{
  UNKNOWN = 0,
  STRING,
  INTEGER,
  REAL,
  BOOLEAN,
  LIST,
  VOID,  // Should always be the separattor bewtween "normal" types and MAKI types
  ENUM,
  COLOR,
  SELECT,
  COMPONENT_SELECT,
  EVENT_SELECT,
  TRIGGER_CALL,
  FLOW_CALL,
  END
};

PropertyTypes plus(PropertyTypes type, uint16_t amount);
PropertyTypes minus(PropertyTypes type, uint16_t amount);
PropertyTypes StringToPropertyTypes(const QString& type);
QString PropertyTypesToString(PropertyTypes type);

enum class ControlTypes
{
  UNKNOWN = 0,
  ADD_EVENT,
  ADD_FIELD,
  ADD_CONTROL,
  REMOVE_CONTROL,
  ADD_STATE
};

ControlTypes StringToControlTypes(const QString& type);
QString ControlTypesToString(ControlTypes type);

enum class LibraryTypes
{
  UNKNOWN = 0,
  STRUCTURAL,
  BEHAVIOUR,
  PIPELINE,
  DATA
};

QString LibraryTypeToString(LibraryTypes type);

// typedef QJsonValueConstRef Config;
}  // namespace Types
