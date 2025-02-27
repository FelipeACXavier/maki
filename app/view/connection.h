#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

// class ConnectionItem : public QGraphicsLineItem
// {
// public:
//   ConnectionItem();

//   void setStart(QString id, QPointF point);
//   void setEnd(QString id, QPointF point);

//   void move(QString id, QPointF pos);

// private:
//   QPointF mSrcPoint;
//   QPointF mDstPoint;

//   QString mSrcId;
//   QString mDstId;
// };

class ConnectionItem : public QGraphicsPathItem
{
public:
  ConnectionItem();

  void setStart(QString id, QPointF point);
  void setEnd(QString id, QPointF point);

  void move(QString id, QPointF pos);

private:
  QPointF mSrcPoint;
  QPointF mDstPoint;

  QString mSrcId;
  QString mDstId;
};
