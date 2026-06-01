#pragma once
#include <QString>
#include <QVariant>

#include "result.h"
#include "types.h"

namespace maki
{

struct PluginVersion;

class SettingField
{
public:
  SettingField() = default;

  QString getKey() const
  {
    return mKey;
  }
  QString getLabel() const
  {
    return mLabel;
  }
  QString getDescription() const
  {
    return mDescription;
  }
  QVariant getValue() const
  {
    return mValue;
  }
  QVariant getDefaultValue() const
  {
    return mDefaultValue;
  }
  Types::PropertyTypes getType() const
  {
    return mType;
  }
  QVariantMap getMetadata() const
  {
    return mMetadata;
  }

  void setKey(const QString& key)
  {
    mKey = key;
  }
  void setLabel(const QString& label)
  {
    mLabel = label;
  }
  void setDescription(const QString& description)
  {
    mDescription = description;
  }
  void setValue(const QVariant& value)
  {
    mValue = value;
  }
  void setDefaultValue(const QVariant& defaultValue)
  {
    mDefaultValue = defaultValue;
  }
  void setType(Types::PropertyTypes type)
  {
    mType = type;
  }
  void setMetadata(const QVariantMap& metadata)
  {
    mMetadata = metadata;
  }

  bool operator==(const SettingField& s) const
  {
    return getKey() == s.getKey() &&
           getLabel() == s.getLabel() &&
           getDescription() == s.getDescription() &&
           getValue() == s.getValue() &&
           getDefaultValue() == s.getDefaultValue() &&
           getType() == s.getType() &&
           getMetadata() == s.getMetadata();
  }

  bool operator!=(const SettingField& s) const
  {
    return !(*this == s);
  }

private:
  QString mKey;
  QString mLabel;
  QString mDescription;
  QVariant mValue;
  QVariant mDefaultValue;
  Types::PropertyTypes mType;
  QVariantMap mMetadata;
};

class ISettings
{
public:
  virtual ~ISettings() = default;

  virtual VoidResult registerSettings(const QString& id,
                                      const maki::PluginVersion version,
                                      const QString& iconPath,
                                      const QVector<SettingField>& settings) = 0;
  virtual QVector<maki::SettingField> getPluginSettings(const QString& id) const = 0;
};

}  // namespace maki
