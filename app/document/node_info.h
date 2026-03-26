#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>
#include <QString>
#include <QVector>

#include "config.h"
#include "idocument.h"
#include "property_info.h"

class NodeSaveInfo : public INode
{
public:
  NodeSaveInfo();

  QString getid() const override;
  QString getnodeId() const override;
  QPointF getposition() const override;
  QVector<std::shared_ptr<IProperty>> getfields() const override;
  QMap<QString, QVariant> getproperties() const override;
  QVector<std::shared_ptr<ITransition>> gettransitions() const override;
  QVector<std::shared_ptr<IFlow>> getflows() const override;
  QString getparentId() const override;
  QVector<std::shared_ptr<INode>> getchildren() const override;

  qreal getScale() const;
  QSizeF getSize() const;
  QPixmap getPixmap() const;
  QString getIcon() const;

  void setId(const QString& arg);
  void setNodeId(const QString& arg);
  void setParentId(const QString& arg);
  void setPosition(const QPointF& arg);
  void setPixmap(const QPixmap& arg);
  void setIcon(const QString& iconPath);
  void setSize(const QSizeF& arg);
  void setScale(qreal arg);

  QVariant getProperty(const QString& key) const;
  void addProperty(const QString& key, const QVariant& value);
  void removeProperty(const QString& key);

  PropertyInfo getField(const QString& key) const;
  void setField(const QString& key, std::shared_ptr<IProperty> property);
  void addField(std::shared_ptr<IProperty> property);
  void removeField(const QString& key);

  void addTransition(std::shared_ptr<ITransition> transition);
  void removeTransition(std::shared_ptr<ITransition> transition);

  void addFlow(std::shared_ptr<IFlow> flow);
  void removeFlow(std::shared_ptr<IFlow> flow);
  void removeFlow(const QString& flowId);

  void addChild(std::shared_ptr<INode> child);
  void removeChild(const QString& childId);
  void clearChildren();

  QJsonObject toJson() const;
  static NodeSaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info);

private:
  QString mId;
  QString mNodeId;
  QString mParentId;

  QPointF mPosition;
  QSizeF mSize;
  qreal mScale;

  QPixmap mPixmap;
  QString mIconPath;

  QMap<QString, QVariant> mProperties;
  QVector<std::shared_ptr<INode>> mChildren;
  QVector<std::shared_ptr<IFlow>> mFlows;
  QVector<std::shared_ptr<ITransition>> mTransitions;
  QVector<std::shared_ptr<IProperty>> mFields;
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<INode>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<INode>>& nodes);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes);