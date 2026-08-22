#include "action_parameter.h"

namespace maki
{

ActionParameter::ActionParameter(const QString& id, const koda::types::TypeReference& type, const Value& value)
    : mId(id)
    , mType(type)
    , mValue(value)
{
}

QString ActionParameter::getid() const
{
  return mId;
}

QString ActionParameter::getname() const
{
  return mId;
}

const IValue* ActionParameter::getvalue() const
{
  return &mValue;
}

koda::types::TypeReference ActionParameter::gettype() const
{
  return mType;
}

Types::ControlTypes ActionParameter::getcontrol() const
{
  return Types::ControlTypes::AUTO;
}

}  // namespace maki