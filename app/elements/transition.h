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

  TransitionItem(std::shared_ptr<TransitionSaveInfo> storage);
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
  std::shared_ptr<TransitionSaveInfo> storage() const;

private:
  const QString mId;
  bool mComplete;

  NodeItem* mSource;
  NodeItem* mDestination;

  std::shared_ptr<TransitionSaveInfo> mStorage;
};
