#pragma once

#include <QGraphicsItem>
#include <QJsonObject>
#include <QJsonValueConstRef>

namespace Types
{
static const int NODE = QGraphicsItem::UserType + 1;
static const int CONNECTOR = QGraphicsItem::UserType + 2;
static const int CONNECTION = QGraphicsItem::UserType + 3;
static const int DRAGGABLE = QGraphicsItem::UserType + 4;
static const int BASE_NODE = QGraphicsItem::UserType + 5;

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
};

enum class PropertyTypes
{
  UNKNOWN = 0,
  STRING,
  INTEGER,
  REAL,
  BOOLEAN,
  SELECT,
  LIST,
  COLOR
};

enum class ControlTypes
{
  UNKNOWN = 0,
  ADD_FIELD,
  ADD_CONTROL,
  REMOVE_CONTROL,
};

enum class LibraryTypes
{
  UNKNOWN = 0,
  STRUCTURAL,
  EXTERNAL_BEHAVIOUR,
  INTERNAL_BEHAVIOUR
};

typedef QJsonValueConstRef Config;
}  // namespace Types
