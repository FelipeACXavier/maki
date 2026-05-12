#pragma once

#include <QGraphicsItem>

namespace Types
{
static const int NODE = QGraphicsItem::UserType + 1;
static const int CONNECTOR = QGraphicsItem::UserType + 2;
static const int CONNECTION = QGraphicsItem::UserType + 3;
static const int DRAGGABLE = QGraphicsItem::UserType + 4;
static const int BASE_NODE = QGraphicsItem::UserType + 5;
static const int TRANSITION = QGraphicsItem::UserType + 6;
static const int FLOW = QGraphicsItem::UserType + 7;
static const int PORT = QGraphicsItem::UserType + 8;

static const QString PIXMAP = "PNG";

}  // namespace Types