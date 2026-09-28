#include "transition_info.h"

#include <QJsonObject>

#include "app_configs.h"
#include "json.h"
#include "keys.h"

Q_DECLARE_METATYPE(TransitionSaveInfo)

TransitionSaveInfo::TransitionSaveInfo()
    : mId("")
    , mLabel("")
    , mEvent("")
    , mSrcId("")
    , mSrcPoint(QPointF{0, 0})
    , mSrcShift(QPointF{0, 0})
    , mDstId("")
    , mDstPoint(QPointF{0, 0})
    , mDstShift(QPointF{0, 0})
    , mSrcPort(Types::Port::OUT)
    , mDstPort(Types::Port::IN)
    , mSrcSubFlow(Constants::MAIN_SUB_FLOW)
    , mDstSubFlow(Constants::MAIN_SUB_FLOW)
{
}

QString TransitionSaveInfo::getid() const
{
  return mId;
}

QString TransitionSaveInfo::getlabel() const
{
  return mLabel;
}

QString TransitionSaveInfo::getevent() const
{
  return mEvent;
}

QString TransitionSaveInfo::getsrcId() const
{
  return mSrcId;
}

QString TransitionSaveInfo::getdstId() const
{
  return mDstId;
}

QPointF TransitionSaveInfo::srcPoint() const
{
  return mSrcPoint;
}

QPointF TransitionSaveInfo::srcShift() const
{
  return mSrcShift;
}

QPointF TransitionSaveInfo::dstPoint() const
{
  return mDstPoint;
}

QPointF TransitionSaveInfo::dstShift() const
{
  return mDstShift;
}

Types::Port TransitionSaveInfo::srcPort() const
{
  return mSrcPort;
}

QString TransitionSaveInfo::srcPortSubFlow() const
{
  return mSrcSubFlow;
}

Types::Port TransitionSaveInfo::dstPort() const
{
  return mDstPort;
}

QString TransitionSaveInfo::dstPortSubFlow() const
{
  return mDstSubFlow;
}

void TransitionSaveInfo::setId(const QString& arg)
{
  mId = arg;
}

void TransitionSaveInfo::setLabel(const QString& arg)
{
  mLabel = arg;
}

void TransitionSaveInfo::setEvent(const QString& arg)
{
  mEvent = arg;
}

void TransitionSaveInfo::setSrcId(const QString& arg)
{
  mSrcId = arg;
}

void TransitionSaveInfo::setSrcPoint(const QPointF& arg)
{
  mSrcPoint = arg;
}

void TransitionSaveInfo::setSrcShift(const QPointF& arg)
{
  mSrcShift = arg;
}

void TransitionSaveInfo::setDstId(const QString& arg)
{
  mDstId = arg;
}

void TransitionSaveInfo::setDstPoint(const QPointF& arg)
{
  mDstPoint = arg;
}

void TransitionSaveInfo::setDstShift(const QPointF& arg)
{
  mDstShift = arg;
}

void TransitionSaveInfo::setSrcPort(Types::Port port)
{
  mSrcPort = port;
}

void TransitionSaveInfo::setDstPort(Types::Port port)
{
  mDstPort = port;
}

void TransitionSaveInfo::setSrcPortSubFlow(const QString& subflow)
{
  mSrcSubFlow = subflow;
}

void TransitionSaveInfo::setDstPortSubFlow(const QString& subflow)
{
  mDstSubFlow = subflow;
}

// ==========================================================================
// JSON serialization
QJsonObject TransitionSaveInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::LABEL] = getlabel();
  data[ConfigKeys::EVENTS] = getevent();

  QJsonObject source;
  source[ConfigKeys::ID] = getsrcId();
  source[ConfigKeys::POSITION] = JSON::fromPointF(srcPoint());
  source[ConfigKeys::SHIFT] = JSON::fromPointF(srcShift());
  source[ConfigKeys::PORT] = (int)mSrcPort;
  source[ConfigKeys::PARENT_SUB_FLOW_ID] = srcPortSubFlow();
  data[ConfigKeys::SOURCE] = source;

  QJsonObject destination;
  destination[ConfigKeys::ID] = getdstId();
  destination[ConfigKeys::POSITION] = JSON::fromPointF(dstPoint());
  destination[ConfigKeys::SHIFT] = JSON::fromPointF(dstShift());
  destination[ConfigKeys::PORT] = (int)mDstPort;
  destination[ConfigKeys::PARENT_SUB_FLOW_ID] = dstPortSubFlow();
  data[ConfigKeys::DESTINATION] = destination;

  return data;
}

TransitionSaveInfo TransitionSaveInfo::fromJson(const QJsonObject& data)
{
  TransitionSaveInfo info;
  info.setId(data[ConfigKeys::ID].toString());
  info.setLabel(data[ConfigKeys::LABEL].toString());
  info.setEvent(data[ConfigKeys::EVENTS].toString());

  info.setSrcId(data[ConfigKeys::SOURCE][ConfigKeys::ID].toString());
  info.setSrcPoint(JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::POSITION].toObject()));
  info.setSrcShift(JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::SHIFT].toObject()));
  info.setSrcPort((Types::Port)data[ConfigKeys::SOURCE][ConfigKeys::PORT].toInt());
  info.setSrcPortSubFlow(data[ConfigKeys::SOURCE][ConfigKeys::PARENT_SUB_FLOW_ID].toString());

  info.setDstId(data[ConfigKeys::DESTINATION][ConfigKeys::ID].toString());
  info.setDstPoint(JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::POSITION].toObject()));
  info.setDstShift(JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::SHIFT].toObject()));
  info.setDstPort((Types::Port)data[ConfigKeys::DESTINATION][ConfigKeys::PORT].toInt());
  info.setDstPortSubFlow(data[ConfigKeys::DESTINATION][ConfigKeys::PARENT_SUB_FLOW_ID].toString());

  return info;
}

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  out << static_cast<qint32>(transitions.size());
  for (const auto& transition : transitions)
    out << *transition;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  qint32 size;
  in >> size;

  transitions.resize(size);
  for (int i = 0; i < size; ++i)
    in >> *transitions[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<ITransition>>& transitions)
{
  out << static_cast<qint32>(transitions.size());
  for (const auto& transition : transitions)
    out << *std::dynamic_pointer_cast<TransitionSaveInfo>(transition);

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<ITransition>>& transitions)
{
  qint32 size;
  in >> size;

  transitions.resize(size);
  for (int i = 0; i < size; ++i)
  {
    transitions[i] = std::make_shared<TransitionSaveInfo>();
    in >> *std::dynamic_pointer_cast<TransitionSaveInfo>(transitions[i]);
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info)
{
  out << info.getid();
  out << info.getlabel();
  out << info.getevent();

  out << info.getsrcId();
  out << info.srcPoint();
  out << info.srcShift();
  out << info.srcPort();
  out << info.srcPortSubFlow();

  out << info.getdstId();
  out << info.dstPoint();
  out << info.dstShift();
  out << info.dstPort();
  out << info.dstPortSubFlow();

  return out;
}

QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info)
{
  QString id;
  in >> id;
  info.setId(id);

  QString label;
  in >> label;
  info.setLabel(label);

  QString event;
  in >> event;
  info.setEvent(event);

  QString srcId;
  in >> srcId;
  info.setSrcId(srcId);

  QPointF srcPoint;
  in >> srcPoint;
  info.setSrcPoint(srcPoint);

  QPointF srcShift;
  in >> srcShift;
  info.setSrcShift(srcShift);

  Types::Port srcPort;
  in >> srcPort;
  info.setSrcPort(srcPort);

  QString srcSubFlow;
  in >> srcSubFlow;
  info.setSrcPortSubFlow(srcSubFlow);

  QString dstId;
  in >> dstId;
  info.setDstId(dstId);

  QPointF dstPoint;
  in >> dstPoint;
  info.setDstPoint(dstPoint);

  QPointF dstShift;
  in >> dstShift;
  info.setDstShift(dstShift);

  Types::Port dstPort;
  in >> dstPort;
  info.setDstPort(dstPort);

  QString dstSubFlow;
  in >> dstSubFlow;
  info.setDstPortSubFlow(dstSubFlow);

  return in;
}
