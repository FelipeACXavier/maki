#include "action_parameter.h"

namespace maki
{

ActionParameter::ActionParameter(const QString& id, Types::PropertyTypes type, const QVariant& value)
    : mId(id)
    , mType(type)
    , mValue(value)
{
}

QString ActionParameter::getid() const
{
  return mId;
}

QVariant ActionParameter::getdefaultValue() const
{
  return mValue;
}

Types::PropertyTypes ActionParameter::gettype() const
{
  return mType;
}

QVector<std::shared_ptr<IProperty>> ActionParameter::getoptions() const
{
  return {};
}

Types::ControlTypes ActionParameter::getcontrol() const
{
  return Types::ControlTypes::AUTO;
}

}  // namespace maki