#pragma once

// #include <QJsonObject>
// #include <QJsonValueConstRef>
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

enum class PropertyTypes
{
  UNKNOWN = 0,
  STRING,
  INTEGER,
  REAL,
  BOOLEAN,
  SELECT,
  LIST,
  COLOR,
  VOID,
  COMPONENT_SELECT,
  EVENT_SELECT,
  ENUM,
  END
};

PropertyTypes StringToPropertyTypes(const QString& type);
QString PropertyTypesToString(PropertyTypes type);

enum class ControlTypes
{
  UNKNOWN = 0,
  ADD_FIELD,
  ADD_CONTROL,
  REMOVE_CONTROL,
  ADD_EVENT,
  ADD_STATE
};

ControlTypes StringToControlTypes(const QString& type);
QString ControlTypesToString(ControlTypes type);

enum class LibraryTypes
{
  UNKNOWN = 0,
  STRUCTURAL,
  BEHAVIOUR
};

QString LibraryTypeToString(LibraryTypes type);

// typedef QJsonValueConstRef Config;
}  // namespace Types
