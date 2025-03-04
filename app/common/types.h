#pragma once

#include <QGraphicsItem>
#include <QJsonObject>
#include <QJsonValueConstRef>

namespace Types
{
static const int NODE = 1;
static const int CONNECTOR = 2;
static const int CONNECTION = 3;
static const int DRAGGABLE = 4;
static const int BASE_NODE = 5;

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
  SELECT
};

enum class ControlTypes
{
  UNKNOWN = 0,
  ADD_CONTROL,
  REMOVE_CONTROL,
};

typedef QJsonValueConstRef Config;
}  // namespace Types
