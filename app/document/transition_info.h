#pragma once

#include <QPointF>
#include <QString>

#include "idocument.h"

class QJsonObject;

class TransitionSaveInfo : public ITransition
{
public:
  TransitionSaveInfo();

  QString getid() const override;
  QString getlabel() const override;
  QString getevent() const override;
  QString getsrcId() const override;
  QString getdstId() const override;

  QPointF srcPoint() const;
  QPointF srcShift() const;
  QPointF dstPoint() const;
  QPointF dstShift() const;

  void setId(const QString& arg);
  void setLabel(const QString& arg);
  void setEvent(const QString& arg);

  void setSrcId(const QString& arg);
  void setSrcPoint(const QPointF& arg);
  void setSrcShift(const QPointF& arg);

  void setDstId(const QString& arg);
  void setDstPoint(const QPointF& arg);
  void setDstShift(const QPointF& arg);

  QJsonObject toJson() const;
  static TransitionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info);

private:
  QString mId;
  QString mLabel;
  QString mEvent;

  QString mSrcId;
  QPointF mSrcPoint;
  QPointF mSrcShift;

  QString mDstId;
  QPointF mDstPoint;
  QPointF mDstShift;
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);