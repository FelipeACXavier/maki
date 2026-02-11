#pragma once

#include <QJsonObject>
#include <QList>

#include "idocument.h"
#include "types.h"

class PropertyConfig;

class PropertyInfo : public IProperty
{
public:
  PropertyInfo();
  PropertyInfo(const PropertyConfig& object);

  QString getid() const override;
  QVariant getdefaultValue() const override;
  QVector<std::shared_ptr<IProperty>> getoptions() const override;
  Types::PropertyTypes gettype() const override;

  void setId(const QString& arg);
  void setDefaultValue(const QVariant& arg);
  void setType(Types::PropertyTypes arg);

  PropertyInfo getOption(const QString& optionId);
  void addOption(std::shared_ptr<IProperty> option);
  void removeOption(std::shared_ptr<IProperty> option);

  QJsonObject toJson() const;
  static PropertyInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const PropertyInfo& config);
  friend QDataStream& operator>>(QDataStream& in, PropertyInfo& config);

private:
  QString mId;
  QVariant mDefaultValue;
  QVector<std::shared_ptr<IProperty>> mOptions;
  Types::PropertyTypes mType;
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<PropertyInfo>>& properties);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IProperty>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IProperty>>& properties);
