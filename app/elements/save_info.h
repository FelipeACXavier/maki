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

class FlowSaveInfo : public IFlow
{
public:
  FlowSaveInfo();
  FlowSaveInfo(const FlowConfig& config);

  QString getid() const override;
  QString getname() const override;
  QString getowner() const override;
  bool getmodifiable() const override;
  Types::ConnectorType gettype() const override;
  Types::PropertyTypes getreturnType() const override;
  QVector<std::shared_ptr<IProperty>> getarguments() const override;
  QVector<std::shared_ptr<INode>> getnodes() const override;

  void setId(const QString& arg);
  void setName(const QString& arg);
  void setOwner(const QString& arg);
  void setModifiable(bool arg);
  void setType(Types::ConnectorType arg);
  void setReturnType(Types::PropertyTypes arg);
  void addArgument(std::shared_ptr<IProperty> arg);
  void setArgument(uint32_t index, std::shared_ptr<IProperty> arg);
  std::shared_ptr<IProperty> getArgument(uint32_t index);
  void removeArgument(std::shared_ptr<IProperty> arg);
  void addNode(std::shared_ptr<INode> arg);
  void removeNode(std::shared_ptr<INode> arg);

  QJsonObject toJson() const;
  static FlowSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info);

private:
  QString mId;
  QString mName;
  QString mOwner;
  bool mModifiable;

  Types::ConnectorType mType;
  Types::PropertyTypes mReturnType;
  QVector<std::shared_ptr<INode>> mNodes;
  QVector<std::shared_ptr<IProperty>> mArguments;
};

class TransitionSaveInfo : public ITransition
{
public:
  TransitionSaveInfo();

  QString getid() const override;
  QString getlabel() const override;
  QString getevent() const override;
  QString getsrcId() const override;
  QString getdstId() const override;

  QPointF srcPoint() const;
  QPointF srcShift() const;
  QPointF dstPoint() const;
  QPointF dstShift() const;

  void setId(const QString& arg);
  void setLabel(const QString& arg);
  void setEvent(const QString& arg);

  void setSrcId(const QString& arg);
  void setSrcPoint(const QPointF& arg);
  void setSrcShift(const QPointF& arg);

  void setDstId(const QString& arg);
  void setDstPoint(const QPointF& arg);
  void setDstShift(const QPointF& arg);

  QJsonObject toJson() const;
  static TransitionSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info);

private:
  QString mId;
  QString mLabel;
  QString mEvent;

  QString mSrcId;
  QPointF mSrcPoint;
  QPointF mSrcShift;

  QString mDstId;
  QPointF mDstPoint;
  QPointF mDstShift;
};

struct NodeSaveInfo : public INode
{
  QString id = "";
  QString nodeId = "";
  QPointF position{0, 0};
  QPixmap pixmap;
  QSizeF size{0, 0};
  qreal scale{1.0};
  // TODO(felaze): Remove fields
  QVector<std::shared_ptr<PropertiesConfig>> fields = {};
  // TODO(felaze): Use Properties class instead of map
  QMap<QString, QVariant> properties = {};
  QVector<std::shared_ptr<TransitionSaveInfo>> transitions = {};
  QVector<std::shared_ptr<FlowSaveInfo>> flows = {};

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
      args.emplace_back(std::static_pointer_cast<IProperty>(arg));

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

class CanvasSaveInfo
{
public:
  CanvasSaveInfo() = default;

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

class SaveInfo : public IDocument
{
public:
  SaveInfo() = default;

  CanvasSaveInfo canvasInfo() const;
  void setCanvasInfo(const CanvasSaveInfo& info);

  QVector<std::shared_ptr<INode>> getnodes() const override;
  void addNode(std::shared_ptr<NodeSaveInfo> node);
  void removeNode(std::shared_ptr<NodeSaveInfo> node);

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
  CanvasSaveInfo mCanvasInfo;
  QVector<std::shared_ptr<INode>> mStructuralNodes;

  QVector<std::shared_ptr<NodeSaveInfo>> findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;
  std::shared_ptr<NodeSaveInfo> findParentOfConstruct(const QString& nodeId, const std::shared_ptr<INode> node) const;
  void findStatesOfConstruct(QVector<std::shared_ptr<NodeSaveInfo>>& toReturn, QVector<std::shared_ptr<INode>> nodes) const;

  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;
  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes);
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<INode>>& nodes);
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& nodes);

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
