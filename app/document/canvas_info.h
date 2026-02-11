#pragma once

#include <QPointF>
#include <QSizeF>
#include <QString>

class QJsonObject;

class CanvasSaveInfo
{
public:
  CanvasSaveInfo();

  qreal scale() const;
  QPointF center() const;

  void setScale(qreal scale);
  void setCenter(const QPointF& center);

  QJsonObject toJson() const;
  static CanvasSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info);

private:
  qreal mScale = {1.0};
  QPointF mCenter = {0, 0};
};
