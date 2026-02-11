#pragma once

#include <QJsonObject>
#include <QList>

#include "idocument.h"
#include "types.h"

class PropertiesConfig : public IProperty
{
public:
  PropertiesConfig();
  PropertiesConfig(const QJsonObject& object);

  QString id = "";
  QVariant defaultValue;
  QList<std::shared_ptr<PropertiesConfig>> options = {};
  Types::PropertyTypes type = Types::PropertyTypes::UNKNOWN;

  bool isValid() const;
  void setInvalid(const QString& message);

  // Inherited --------------------------------------
  QString getid() const override
  {
    return id;
  }
  QVariant getdefaultValue() const override
  {
    return defaultValue;
  }
  QList<std::shared_ptr<const IProperty>> getoptions() const override
  {
    QList<std::shared_ptr<const IProperty>> out;
    out.reserve(options.size());
    for (const auto& f : options)
      out.push_back(std::static_pointer_cast<PropertiesConfig>(f));

    return out;
  }
  Types::PropertyTypes gettype() const override
  {
    return type;
  }
  // ------------------------------------------------

  QJsonObject toJson() const;
  static PropertiesConfig fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const PropertiesConfig& config);
  friend QDataStream& operator>>(QDataStream& in, PropertiesConfig& config);

  bool isConfigValid;
  QString errorMessage;

private:
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);
};

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<PropertiesConfig>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<PropertiesConfig>>& properties);
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IProperty>>& properties);
QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IProperty>>& properties);
