#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

#include "types.h"

class ConnectionItem : public QGraphicsPathItem
{
public:
  enum
  {
    Type = UserType + Type::CONNECTION
  };

  ConnectionItem();

  void setStart(const QString& id, QPointF point);
  void setEnd(const QString& id, QPointF point);

  void move(const QString& id, QPointF pos);

private:
  QPointF mSrcPoint;
  QPointF mDstPoint;

  QString mSrcId;
  QString mDstId;
};
