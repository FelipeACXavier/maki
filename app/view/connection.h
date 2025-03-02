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

  void setStart(const QString& id, const QPointF& point, const QPointF& controlShift);
  void setEnd(const QString& id, const QPointF& point, const QPointF& controlShift);
  void done();

  void move(const QString& id, QPointF pos);

private:
  bool mComplete;

  QPointF mSrcPoint;
  QPointF mDstPoint;

  QPointF mSrcShift;
  QPointF mDstShift;

  QString mSrcId;
  QString mDstId;
};
