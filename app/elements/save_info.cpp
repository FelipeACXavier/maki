#include "save_info.h"

#include <QBuffer>

#include "logging.h"

Q_DECLARE_METATYPE(ConnectorSaveInfo)
Q_DECLARE_METATYPE(ConnectionSaveInfo)
Q_DECLARE_METATYPE(NodeSaveInfo)
Q_DECLARE_METATYPE(SaveInfo)

QDataStream& operator<<(QDataStream& out, const ConnectorSaveInfo& info)
{
  out << info.connectorId;
  out << info.configId;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectorSaveInfo& info)
{
  in >> info.connectorId;
  in >> info.configId;

  return in;
}

QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info)
{
  out << info.id;
  out << info.size;
  out << info.scale;
  out << info.nodeId;
  out << info.fields;
  out << info.position;
  out << info.connectors;
  out << info.properties;
  out << info.parentId;

  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  info.pixmap.save(&buffer, "PNG");

  out << pixmapData;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info)
{
  in >> info.id;
  in >> info.size;
  in >> info.scale;
  in >> info.nodeId;
  in >> info.fields;
  in >> info.position;
  in >> info.connectors;
  in >> info.properties;
  in >> info.parentId;

  QByteArray pixmapData;
  in >> pixmapData;
  info.pixmap.loadFromData(pixmapData, "PNG");

  return in;
}

QDataStream& operator<<(QDataStream& out, const ConnectionSaveInfo& info)
{
  out << info.srcId;
  out << info.srcPoint;
  out << info.srcShift;

  out << info.dstId;
  out << info.dstPoint;
  out << info.dstShift;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectionSaveInfo& info)
{
  in >> info.srcId;
  in >> info.srcPoint;
  in >> info.srcShift;

  in >> info.dstId;
  in >> info.dstPoint;
  in >> info.dstShift;

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<NodeSaveInfo>& nodes)
{
  out << static_cast<qint32>(nodes.size());
  for (const auto& node : nodes)
    out << node;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<NodeSaveInfo>& nodes)
{
  qint32 size;
  in >> size;

  nodes.resize(size);
  for (int i = 0; i < size; ++i)
    in >> nodes[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const SaveInfo& info)
{
  out << info.structuralNodes << info.behaviouralNodes << info.connections;
  return out;
}

QDataStream& operator>>(QDataStream& in, SaveInfo& info)
{
  in >> info.structuralNodes;
  in >> info.behaviouralNodes;
  in >> info.connections;

  return in;
}

QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info)
{
  out << info.scale;
  LOG_INFO("Saving scale: %f", info.scale);
  return out;
}

QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info)
{
  in >> info.scale;

  LOG_INFO("Loading scale: %f", info.scale);

  return in;
}
