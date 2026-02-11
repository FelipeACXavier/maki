#include "canvas_info.h"

#include <QJsonObject>

#include "json.h"
#include "keys.h"

Q_DECLARE_METATYPE(CanvasSaveInfo)

CanvasSaveInfo::CanvasSaveInfo()
    : mScale(1.0)
    , mCenter(QPointF{0, 0})
{
}

qreal CanvasSaveInfo::scale() const
{
  return mScale;
}

QPointF CanvasSaveInfo::center() const
{
  return mCenter;
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

  data[ConfigKeys::POSITION] = JSON::fromPointF(center());
  data[ConfigKeys::SCALE] = scale();

  return data;
}

CanvasSaveInfo CanvasSaveInfo::fromJson(const QJsonObject& data)
{
  CanvasSaveInfo info;

  info.setCenter(JSON::toPointF(data[ConfigKeys::POSITION].toObject()));
  info.setScale(data[ConfigKeys::SCALE].toDouble());

  return info;
}

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info)
{
  // out << info.scale();
  // out << info.center();

  return out;
}

QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info)
{
  // in >> info.scale;
  // in >> info.center;

  return in;
}
