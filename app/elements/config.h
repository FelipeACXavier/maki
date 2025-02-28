#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QVector>

class ConnectorConfig
{
public:
  ConnectorConfig();
  ConnectorConfig(const QJsonObject& object);

  QPointF getPosition(const QRectF& bounds) const;
  QString position = "";

  friend QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config);
  friend QDataStream& operator>>(QDataStream& in, ConnectorConfig& config);
};

QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config);
QDataStream& operator>>(QDataStream& in, ConnectorConfig& config);

class BodyConfig
{
public:
  BodyConfig();
  BodyConfig(const QJsonObject& object);

  QString shape = "Rectangle";
  QColor textColor = Qt::black;
  QColor backgroundColor = Qt::lightGray;
  QColor borderColor = Qt::black;

  int width = 100;
  int height = 50;

  friend QDataStream& operator<<(QDataStream& out, const BodyConfig& config);
  friend QDataStream& operator>>(QDataStream& in, BodyConfig& config);
};

QDataStream& operator<<(QDataStream& out, const BodyConfig& config);
QDataStream& operator>>(QDataStream& in, BodyConfig& config);

class NodeConfig
{
public:
  NodeConfig(const QJsonObject& object);

  QString name;
  BodyConfig body;
  QVector<ConnectorConfig> connectors;

  friend QDataStream& operator<<(QDataStream& out, const NodeConfig& config);
  friend QDataStream& operator>>(QDataStream& in, NodeConfig& config);
};
