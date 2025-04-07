#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

#include "elements/node.h"
#include "inode.h"
#include "types.h"

class Connector;
struct ConnectionSaveInfo;

class ConnectionItem : public QGraphicsPathItem, public IConnection
{
public:
  enum
  {
    Type = Types::CONNECTION
  };

  ConnectionItem();
  virtual ~ConnectionItem();

  QString id() const override;
  int type() const override;

  void setStart(const QString& id, const QPointF& point, const QPointF& controlShift);
  void setEnd(const QString& id, const QPointF& point, const QPointF& controlShift);
  void done(Connector* source, Connector* destination);

  IConnector* source() const override;
  IConnector* destination() const override;

  void move(const QString& id, QPointF pos);

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  ConnectionSaveInfo saveInfo() const;

private:
  const QString mId;
  bool mComplete;

  QPointF mSrcPoint;
  QPointF mDstPoint;

  QPointF mSrcShift;
  QPointF mDstShift;

  Connector* mSource;
  Connector* mDestination;

  QString mSrcId;
  QString mDstId;
};
