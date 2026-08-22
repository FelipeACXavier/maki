#include "property_info.h"

#include <QJsonArray>

#include "config.h"
#include "keys.h"
#include "type_helpers.h"
#include "types.h"

PropertyInfo::PropertyInfo()
{
}

PropertyInfo::PropertyInfo(const PropertyConfig& object)
{
  setId(object.id);
  setDefaultValue(object.defaultValue);
  setType(object.type);
  setControl(object.control);
}

QString PropertyInfo::getid() const
{
  return mId;
}

QString PropertyInfo::getname() const
{
  return mId;
}

const IValue* PropertyInfo::getvalue() const
{
  return &mDefaultValue;
}

koda::types::TypeReference PropertyInfo::gettype() const
{
  return mType;
}

Types::ControlTypes PropertyInfo::getcontrol() const
{
  return mControlType;
}

void PropertyInfo::setId(const QString& arg)
{
  mId = arg;
}

void PropertyInfo::setDefaultValue(const maki::Value& arg)
{
  mDefaultValue = arg;
}

void PropertyInfo::setType(const koda::types::TypeReference& arg)
{
  mType = arg;
}

void PropertyInfo::setControl(Types::ControlTypes arg)
{
  mControlType = arg;
}

// ==========================================================================
// JSON serialization
QJsonObject PropertyInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::DEFAULT] = mDefaultValue.toJson();
  data[ConfigKeys::TYPE] = maki::typeReferenceToJson(mType);

  return data;
}

PropertyInfo PropertyInfo::fromJson(const QJsonObject& data)
{
  PropertyInfo config;
  config.setId(data[ConfigKeys::ID].toString());
  config.setDefaultValue(maki::Value::fromJson(data[ConfigKeys::DEFAULT].toObject()));
  auto ref = maki::typeReferenceFromJson(data[ConfigKeys::TYPE].toObject());
  if (ref.IsSuccess())
    config.setType(ref.Value());

  return config;
}

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const PropertyInfo& config)
{
  out << config.getid();
  out << config.gettype();
  out << config.getcontrol();

  auto value = config.getvalue();
  if (!value)
  {
    out << maki::Value{};
    return out;
  }

  const auto* concrete = dynamic_cast<const maki::Value*>(config.getvalue());
  if (!concrete)
  {
    out << maki::Value{};
    return out;
  }

  out << *concrete;
  return out;
}

QDataStream& operator>>(QDataStream& in, PropertyInfo& config)
{
  QString id;
  in >> id;
  config.setId(id);

  koda::types::TypeReference type;
  in >> type;
  config.setType(type);

  Types::ControlTypes control;
  in >> control;
  config.setControl(control);

  maki::Value defaultValue;
  in >> defaultValue;
  config.setDefaultValue(defaultValue);

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<PropertyInfo>>& properties)
{
  out << static_cast<qint32>(properties.size());
  for (const auto& prop : properties)
    out << *prop;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<PropertyInfo>>& properties)
{
  qint32 size;
  in >> size;

  properties.clear();
  properties.reserve(size);

  for (qint32 i = 0; i < size; ++i)
  {
    auto property = std::make_shared<PropertyInfo>();
    in >> *property;
    properties.push_back(std::move(property));
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IParameter>>& properties)
{
  out << static_cast<qint32>(properties.size());
  for (const auto& prop : properties)
    out << *std::dynamic_pointer_cast<PropertyInfo>(prop);

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IParameter>>& properties)
{
  qint32 size;
  in >> size;

  properties.clear();
  properties.reserve(size);

  for (qint32 i = 0; i < size; ++i)
  {
    auto property = std::make_shared<PropertyInfo>();
    in >> *property;
    properties.push_back(std::move(property));
  }

  return in;
}
