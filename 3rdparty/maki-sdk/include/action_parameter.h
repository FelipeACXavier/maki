#pragma once

#include <qvariant.h>

#include "idocument.h"

namespace maki
{
class ActionParameter : public IProperty
{
public:
  ActionParameter(const QString& id, Types::PropertyTypes type, const QVariant& value);

  QString getid() const override;
  QVariant getdefaultValue() const override;
  QVector<std::shared_ptr<IProperty>> getoptions() const override;
  Types::PropertyTypes gettype() const override;

private:
  const QString mId;
  const Types::PropertyTypes mType;
  const QVariant mValue;
};
}  // namespace maki