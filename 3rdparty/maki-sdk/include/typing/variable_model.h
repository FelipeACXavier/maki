#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "type_reference.h"

namespace koda
{
class Value
{
public:
  using Variant = std::variant<bool, int64_t, double, std::string, std::unordered_map<std::string, Value>, std::vector<Value>>;

  Value(Variant data);
  Variant data() const;

private:
  Variant mData;
};

class Variable
{
public:
  Variable(const std::string& name, const std::string& type, const Value& value);

  std::string name() const;
  types::TypeReference type() const;

  static Variable boolean(const std::string& name, const bool value);
  static Variable integer(const std::string& name, const int value);
  static Variable real(const std::string& name, const double value);
  static Variable string(const std::string& name, const std::string& value);

private:
  std::string mName;
  types::TypeReference mType;
  Value mValue;
};
}  // namespace koda