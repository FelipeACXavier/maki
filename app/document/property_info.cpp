#include "property_info.h"

#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

#include <QJsonArray>

#include "config.h"
#include "keys.h"
#include "types.h"

PropertyInfo::PropertyInfo()
    : mId("")
    , mOptions({})
    , mType(Types::PropertyTypes::UNKNOWN)
{
}

PropertyInfo::PropertyInfo(const PropertyConfig& object)
{
  setId(object.id);
  setDefaultValue(object.defaultValue);
  setType(object.type);
  for (const auto& opt : object.options)
    addOption(std::make_shared<PropertyInfo>(opt));
}

QString PropertyInfo::getid() const
{
  return mId;
}

QVariant PropertyInfo::getdefaultValue() const
{
  return mDefaultValue;
}

QVector<std::shared_ptr<IProperty>> PropertyInfo::getoptions() const
{
  return mOptions;
}

Types::PropertyTypes PropertyInfo::gettype() const
{
  return mType;
}

void PropertyInfo::setId(const QString& arg)
{
  mId = arg;
}

void PropertyInfo::setDefaultValue(const QVariant& arg)
{
  mDefaultValue = arg;
}

void PropertyInfo::setType(Types::PropertyTypes arg)
{
  mType = arg;
}

PropertyInfo PropertyInfo::getOption(const QString& optionId)
{
  for (const auto& field : getoptions())
  {
    if (field->getid() == optionId)
      return *std::dynamic_pointer_cast<PropertyInfo>(field);
  }

  return PropertyInfo();
}

void PropertyInfo::addOption(std::shared_ptr<IProperty> option)
{
  mOptions.push_back(option);
}

void PropertyInfo::removeOption(std::shared_ptr<IProperty> option)
{
  mOptions.removeIf([option](std::shared_ptr<IProperty> info) { return info->getid() == option->getid(); });
}

// ==========================================================================
// JSON serialization
QJsonObject PropertyInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = getid();

  if (getdefaultValue().isValid() && !getdefaultValue().isNull())
  {
    if (gettype() == Types::PropertyTypes::STRING)
      data[ConfigKeys::DEFAULT] = getdefaultValue().toString();
    else if (gettype() == Types::PropertyTypes::BOOLEAN)
      data[ConfigKeys::DEFAULT] = getdefaultValue().toBool();
    else if (gettype() == Types::PropertyTypes::INTEGER)
      data[ConfigKeys::DEFAULT] = getdefaultValue().toInt();
    else if (gettype() == Types::PropertyTypes::REAL)
      data[ConfigKeys::DEFAULT] = getdefaultValue().toDouble();
    else if (gettype() == Types::PropertyTypes::ENUM)
      data[ConfigKeys::DEFAULT] = getdefaultValue().toString();
    else
      data[ConfigKeys::DEFAULT] = getdefaultValue().toJsonObject();
  }

  QJsonArray optionArray;
  for (const auto& opt : getoptions())
    optionArray.append(std::dynamic_pointer_cast<PropertyInfo>(opt)->toJson());

  if (!optionArray.isEmpty())
    data[ConfigKeys::OPTIONS] = optionArray;

  data[ConfigKeys::TYPE] = (int)gettype();

  return data;
}

PropertyInfo PropertyInfo::fromJson(const QJsonObject& data)
{
  PropertyInfo config;
  config.setId(data[ConfigKeys::ID].toString());
  config.setDefaultValue(QJsonValue(data[ConfigKeys::DEFAULT]).toVariant());

  for (const auto& value : data[ConfigKeys::OPTIONS].toArray())
    config.addOption(std::make_shared<PropertyInfo>(fromJson(value.toObject())));

  config.setType((Types::PropertyTypes)data[ConfigKeys::TYPE].toInt());

  return config;
}

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const PropertyInfo& config)
{
  out << config.getid();
  out << config.gettype();
  out << config.getdefaultValue();

  out << config.getoptions();

  return out;
}

QDataStream& operator>>(QDataStream& in, PropertyInfo& config)
{
  QString id;
  in >> id;
  config.setId(id);

  Types::PropertyTypes type;
  in >> type;
  config.setType(type);

  QVariant defaultValue;
  in >> defaultValue;
  config.setDefaultValue(defaultValue);

  QVector<std::shared_ptr<PropertyInfo>> options;
  in >> options;
  for (const auto& option : options)
    config.addOption(std::dynamic_pointer_cast<IProperty>(option));

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

  properties.resize(size);
  for (int i = 0; i < size; ++i)
    in >> *properties[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IProperty>>& properties)
{
  out << static_cast<qint32>(properties.size());
  for (const auto& prop : properties)
    out << *std::dynamic_pointer_cast<PropertyInfo>(prop);

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IProperty>>& properties)
{
  qint32 size;
  in >> size;

  properties.resize(size);
  for (int i = 0; i < size; ++i)
  {
    properties[i] = std::make_shared<PropertyInfo>();
    in >> *std::dynamic_pointer_cast<PropertyInfo>(properties[i]);
  }

  return in;
}