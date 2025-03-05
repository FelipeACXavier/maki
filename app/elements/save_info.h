#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"

struct SaveInfo
{
  QString nodeId;
  QPointF position;
  QPixmap pixmap;
  QSizeF size;
  QVector<PropertiesConfig> fields;
  QMap<QString, QVariant> properties;

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);
};
