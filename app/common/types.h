#pragma once

#include <QGraphicsItem>
#include <QJsonObject>
#include <QJsonValueConstRef>
#include <QString>

namespace Types
{
static const int NODE = QGraphicsItem::UserType + 1;
static const int CONNECTOR = QGraphicsItem::UserType + 2;
static const int CONNECTION = QGraphicsItem::UserType + 3;
static const int DRAGGABLE = QGraphicsItem::UserType + 4;
static const int BASE_NODE = QGraphicsItem::UserType + 5;
static const int TRANSITION = QGraphicsItem::UserType + 6;
static const int FLOW = QGraphicsItem::UserType + 7;

static const char* PIXMAP = "PNG";

enum class Shape
{
  UNKNOWN = 0,
  RECTANGLE,
  ROUNDED_RECTANGLE,
  ELLIPSE,
  DIAMOND
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
  STATE_SELECT,
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

typedef QJsonValueConstRef Config;
}  // namespace Types
