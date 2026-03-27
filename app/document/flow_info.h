#pragma once

#include <QString>
#include <QVector>

#include "config.h"
#include "idocument.h"

class NodeSaveInfo;

class FlowSaveInfo : public IFlow
{
public:
  FlowSaveInfo();
  FlowSaveInfo(const FlowConfig& config);

  QString getid() const override;
  QString getname() const override;
  QString getowner() const override;
  bool getmodifiable() const override;
  Types::CallType gettype() const override;
  Types::PropertyTypes getreturnType() const override;
  QVector<std::shared_ptr<IProperty>> getarguments() const override;
  QVector<std::shared_ptr<INode>> getnodes() const override;

  void setId(const QString& arg);
  void setName(const QString& arg);
  void setOwner(const QString& arg);
  void setModifiable(bool arg);
  void setType(Types::CallType arg);
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

  Types::CallType mType;
  Types::PropertyTypes mReturnType;
  QVector<std::shared_ptr<INode>> mNodes;
  QVector<std::shared_ptr<IProperty>> mArguments;
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& nodes);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IFlow>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IFlow>>& nodes);