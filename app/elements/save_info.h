#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"
#include "inode.h"

struct ConnectorSaveInfo
{
  QString connectorId = "";
  QString configId = "";

  ConnectorSaveInfo() = default;

  QJsonObject toJson() const;
  static ConnectorSaveInfo fromJson(const QJsonObject& data);

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
  qreal scale{1.0};
  QVector<PropertiesConfig> fields;
  QMap<QString, QVariant> properties;
  QVector<ConnectorSaveInfo> connectors;

  QString parentId = "";
  QVector<NodeSaveInfo> children = {};

  NodeSaveInfo() = default;

  QJsonObject toJson() const;
  static NodeSaveInfo fromJson(const QJsonObject& data);

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

  QJsonObject toJson() const;
  static ConnectionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const ConnectionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, ConnectionSaveInfo& info);
};

struct CanvasSaveInfo
{
  qreal scale{1.0};
  QPointF center{0, 0};

  QJsonObject toJson() const;
  static CanvasSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info);
};

struct SaveInfo
{
  CanvasSaveInfo canvasInfo;
  QVector<NodeSaveInfo> structuralNodes;
  QVector<NodeSaveInfo> behaviouralNodes;
  QVector<ConnectionSaveInfo> connections;

  QJsonObject toJson() const;
  static SaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);
};

QDataStream& operator<<(QDataStream& out, const QVector<NodeSaveInfo>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<NodeSaveInfo>& nodes);
