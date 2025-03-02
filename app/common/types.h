#pragma once

#include <QGraphicsItem>
#include <QJsonObject>
#include <QJsonValueConstRef>

namespace Type
{
static const int NODE = 1;
static const int CONNECTOR = 2;
static const int CONNECTION = 3;
static const int DRAGGABLE = 4;

enum class Shape
{
  RECTANGLE = 0,
  ROUNDED_RECTANGLE,
  ELLIPSE,
  DIAMOND
};

typedef QJsonValueConstRef Config;
}  // namespace Type
