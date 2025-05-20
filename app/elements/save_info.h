#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"
#include "inode.h"

struct NodeSaveInfo;

struct FlowSaveInfo
{
  QString id = "";
  QString name = "";

  QVector<std::shared_ptr<NodeSaveInfo>> nodes;

  FlowSaveInfo() = default;

  QJsonObject toJson() const;
  static FlowSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info);
};

struct TransitionSaveInfo
{
  QString id = "";

  QString srcId = "";
  QPointF srcPoint{0, 0};
  QPointF srcShift{0, 0};

  QString dstId = "";
  QPointF dstPoint{0, 0};
  QPointF dstShift{0, 0};

  TransitionSaveInfo() = default;

  QJsonObject toJson() const;
  static TransitionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info);
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
  QVector<EventConfig> events;
  QMap<QString, QVariant> properties;
  QVector<std::shared_ptr<TransitionSaveInfo>> transitions;
  QVector<std::shared_ptr<FlowSaveInfo>> flows;

  QString parentId = "";
  QVector<std::shared_ptr<NodeSaveInfo>> children = {};

  NodeSaveInfo() = default;

  QJsonObject toJson() const;
  static NodeSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info);
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
  QVector<std::shared_ptr<NodeSaveInfo>> structuralNodes = {};
  QVector<std::shared_ptr<NodeSaveInfo>> behaviouralNodes = {};

  QJsonObject toJson() const;
  static SaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
