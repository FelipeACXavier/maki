#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"

struct ConnectorSaveInfo
{
  QString connectorId = "";
  QString configId = "";

  ConnectorSaveInfo() = default;

  friend QDataStream& operator<<(QDataStream& out, const ConnectorSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, ConnectorSaveInfo& info);
};

struct NodeSaveInfo
{
  QString id = "";
  QString nodeId = "";
  QPointF position{0, 0};
  QPixmap pixmap;
  QSizeF size{0, 0};
  QVector<PropertiesConfig> fields;
  QMap<QString, QVariant> properties;
  QVector<ConnectorSaveInfo> connectors;

  QString parentId = "";

  NodeSaveInfo() = default;

  friend QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info);
};

struct ConnectionSaveInfo
{
  QString srcId = "";
  QPointF srcPoint{0, 0};
  QPointF srcShift{0, 0};

  QString dstId = "";
  QPointF dstPoint{0, 0};
  QPointF dstShift{0, 0};

  ConnectionSaveInfo() = default;

  friend QDataStream& operator<<(QDataStream& out, const ConnectionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, ConnectionSaveInfo& info);
};

struct SaveInfo
{
  QVector<NodeSaveInfo> structuralNodes;
  QVector<NodeSaveInfo> behaviouralNodes;
  QVector<ConnectionSaveInfo> connections;

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);
};

QDataStream& operator<<(QDataStream& out, const QVector<NodeSaveInfo>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<NodeSaveInfo>& nodes);
