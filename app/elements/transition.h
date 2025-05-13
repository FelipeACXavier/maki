#pragma once

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

#include "inode.h"
#include "types.h"

class NodeItem;
struct TransitionSaveInfo;

class TransitionItem : public QGraphicsPathItem
{
public:
  enum
  {
    Type = Types::TRANSITION
  };

  TransitionItem();
  virtual ~TransitionItem();

  QString id() const;
  int type() const override;

  void setStart(const QString& id, const QPointF& point, const QPointF& controlShift);
  void setEnd(const QString& id, const QPointF& point, const QPointF& controlShift);
  void done(NodeItem* source, NodeItem* destination);

  NodeItem* source() const;
  NodeItem* destination() const;

  void move(const QString& id, QPointF pos);
  void updatePath();

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  TransitionSaveInfo saveInfo() const;

private:
  const QString mId;
  bool mComplete;

  QPointF mSrcPoint;
  QPointF mDstPoint;

  QPointF mSrcShift;
  QPointF mDstShift;

  NodeItem* mSource;
  NodeItem* mDestination;

  QString mSrcId;
  QString mDstId;
};
