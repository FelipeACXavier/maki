#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"
#include "idocument.h"

struct NodeSaveInfo;

struct FlowSaveInfo : public IFlow
{
  QString id = "";
  QString name = "";
  QString owner = "";
  bool modifiable = true;

  Types::ConnectorType type = Types::ConnectorType::UNKNOWN;
  Types::PropertyTypes returnType = Types::PropertyTypes::UNKNOWN;
  QVector<PropertiesConfig> arguments = {};

  QVector<std::shared_ptr<NodeSaveInfo>> nodes = {};

  // Inherited --------------------------------------
  QString getid() const override
  {
    return id;
  }
  QString getname() const override
  {
    return name;
  }
  QString getowner() const override
  {
    return owner;
  }
  bool getmodifiable() const override
  {
    return modifiable;
  }
  Types::ConnectorType gettype() const override
  {
    return type;
  }
  Types::PropertyTypes getreturnType() const override
  {
    return returnType;
  }
  QVector<std::shared_ptr<IProperty>> getarguments() const override
  {
    QVector<std::shared_ptr<IProperty>> args;
    for (auto arg : arguments)
      args.emplace_back(std::make_shared<PropertiesConfig>(arg));
    return args;
  }
  QVector<std::shared_ptr<INode>> getnodes() const override
  {
    QVector<std::shared_ptr<INode>> out;
    out.reserve(nodes.size());
    for (const auto& f : nodes)
      out.push_back(std::static_pointer_cast<INode>(f));

    return out;
  }

  // ------------------------------------------------

  FlowSaveInfo() = default;
  FlowSaveInfo(const FlowConfig& config);

  QJsonObject toJson() const;
  static FlowSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info);
};

struct TransitionSaveInfo : public ITransition
{
  QString id = "";
  QString label = "";
  QString event = "";

  QString srcId = "";
  QPointF srcPoint{0, 0};
  QPointF srcShift{0, 0};

  QString dstId = "";
  QPointF dstPoint{0, 0};
  QPointF dstShift{0, 0};

  // Inherited --------------------------------------
  QString getid() const override
  {
    return id;
  }
  QString getlabel() const override
  {
    return label;
  }
  QString getevent() const override
  {
    return event;
  }
  QString getsrcId() const override
  {
    return srcId;
  }
  QString getdstId() const override
  {
    return dstId;
  }
  // ------------------------------------------------

  TransitionSaveInfo() = default;

  QJsonObject toJson() const;
  static TransitionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info);
};

struct NodeSaveInfo : public INode
{
  QString id = "";
  QString nodeId = "";
  QPointF position{0, 0};
  QPixmap pixmap;
  QSizeF size{0, 0};
  qreal scale{1.0};
  QVector<PropertiesConfig> fields;
  QMap<QString, QVariant> properties;
  QVector<std::shared_ptr<TransitionSaveInfo>> transitions;
  QVector<std::shared_ptr<FlowSaveInfo>> flows;
  std::shared_ptr<FlowSaveInfo> behaviour;

  QString parentId = "";
  QVector<std::shared_ptr<NodeSaveInfo>> children = {};

  // Inherited --------------------------------------
  QString getid() const override
  {
    return id;
  }
  QString getnodeId() const override
  {
    return nodeId;
  }
  QPointF getposition() const override
  {
    return position;
  }
  QVector<std::shared_ptr<IProperty>> getfields() const override
  {
    QVector<std::shared_ptr<IProperty>> args;
    for (auto arg : fields)
      args.emplace_back(std::make_shared<PropertiesConfig>(arg));
    return args;
  }
  QMap<QString, QVariant> getproperties() const override
  {
    return properties;
  }
  QVector<std::shared_ptr<ITransition>> gettransitions() const override
  {
    QVector<std::shared_ptr<ITransition>> out;
    out.reserve(transitions.size());
    for (const auto& f : transitions)
      out.push_back(std::static_pointer_cast<ITransition>(f));

    return out;
  }
  QVector<std::shared_ptr<IFlow>> getflows() const override
  {
    QVector<std::shared_ptr<IFlow>> out;
    out.reserve(flows.size());
    for (const auto& f : flows)
      out.push_back(std::static_pointer_cast<IFlow>(f));

    return out;
  }
  std::shared_ptr<IFlow> getbehaviour() const override
  {
    return std::static_pointer_cast<IFlow>(behaviour);
  }
  QString getparentId() const override
  {
    return parentId;
  }
  QVector<std::shared_ptr<INode>> getchildren() const override
  {
    QVector<std::shared_ptr<INode>> out;
    out.reserve(children.size());
    for (const auto& f : children)
      out.push_back(std::static_pointer_cast<INode>(f));

    return out;
  }
  // ------------------------------------------------

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

struct SaveInfo : public IDocument
{
  CanvasSaveInfo canvasInfo;
  QVector<std::shared_ptr<NodeSaveInfo>> structuralNodes = {};
  QVector<std::shared_ptr<NodeSaveInfo>> behaviouralNodes = {};

  // Inherited --------------------------------------
  QVector<std::shared_ptr<INode>> getnodes() const override
  {
    QVector<std::shared_ptr<INode>> out;
    out.reserve(structuralNodes.size());
    for (const auto& f : structuralNodes)
      out.push_back(std::static_pointer_cast<INode>(f));

    return out;
  };
  // ------------------------------------------------

  QJsonObject toJson() const;
  static SaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);

  QVector<std::shared_ptr<NodeSaveInfo>> getPossibleStates(const QString& nodeId) const;
  QVector<std::shared_ptr<NodeSaveInfo>> getPossibleCallers(const QString& nodeId) const;
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId) const;

  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId);
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId);

private:
  QVector<std::shared_ptr<NodeSaveInfo>> findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<NodeSaveInfo>> nodes) const;
  std::shared_ptr<NodeSaveInfo> findParentOfConstruct(const QString& nodeId, const std::shared_ptr<NodeSaveInfo> node) const;
  void findStatesOfConstruct(QVector<std::shared_ptr<NodeSaveInfo>>& toReturn, QVector<std::shared_ptr<NodeSaveInfo>> nodes) const;

  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<NodeSaveInfo>> nodes) const;
  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
