#include "canvas_info.h"

#include <QJsonObject>

#include "json.h"
#include "keys.h"

Q_DECLARE_METATYPE(CanvasSaveInfo)

CanvasSaveInfo::CanvasSaveInfo()
    : mId("")
    , mScale(1.0)
    , mCenter(QPointF{0, 0})
{
}

CanvasSaveInfo::CanvasSaveInfo(const QString& id, const QPointF& position, qreal scale)
    : mId(id)
    , mScale(scale)
    , mCenter(position)
{
}

QString CanvasSaveInfo::id() const
{
  return mId;
}

qreal CanvasSaveInfo::scale() const
{
  return mScale;
}

QPointF CanvasSaveInfo::center() const
{
  return mCenter;
}

void CanvasSaveInfo::setId(const QString& id)
{
  mId = id;
}

void CanvasSaveInfo::setScale(qreal scale)
{
  mScale = scale;
}

void CanvasSaveInfo::setCenter(const QPointF& center)
{
  mCenter = center;
}

// ==========================================================================
// JSON serialization
QJsonObject CanvasSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::ID] = id();
  data[ConfigKeys::POSITION] = JSON::fromPointF(center());
  data[ConfigKeys::SCALE] = scale();

  return data;
}

CanvasSaveInfo CanvasSaveInfo::fromJson(const QJsonObject& data)
{
  CanvasSaveInfo info;

  if (data.contains(ConfigKeys::ID))
    info.setId(data[ConfigKeys::ID].toString());

  info.setCenter(JSON::toPointF(data[ConfigKeys::POSITION].toObject()));
  info.setScale(data[ConfigKeys::SCALE].toDouble());

  return info;
}

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info)
{
  out << info.id();
  out << info.scale();
  out << info.center();

  return out;
}

QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info)
{
  QString id;
  qreal scale;
  QPointF center;

  in >> id;
  in >> scale;
  in >> center;

  info.setId(id);
  info.setScale(scale);
  info.setCenter(center);

  return in;
}
