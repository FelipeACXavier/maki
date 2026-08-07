#include "variable_model.h"

namespace koda
{
Value::Value(Variant data)
    : mData(data)
{
}

Value::Variant Value::data() const
{
  return mData;
}

Variable::Variable(const std::string& name, const std::string& type, const Value& value)
    : mName(name)
    , mType(types::TypeReference::named(type))
    , mValue(value)
{
}

std::string Variable::name() const
{
  return mName;
}

types::TypeReference Variable::type() const
{
  return mType;
}

Variable Variable::boolean(const std::string& name, const bool value)
{
  return Variable(name, "builtin.Boolean", Value(value));
}

Variable Variable::integer(const std::string& name, const int value)
{
  return Variable(name, "builtin.Integer", Value(value));
}

Variable Variable::real(const std::string& name, const double value)
{
  return Variable(name, "builtin.Real", Value(value));
}

Variable Variable::string(const std::string& name, const std::string& value)
{
  return Variable(name, "builtin.String", Value(value));
}
}  // namespace koda