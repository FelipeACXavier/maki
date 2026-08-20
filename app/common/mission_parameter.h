#pragma once

#include <QColor>
#include <QString>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "typing/type_reference.h"

namespace maki
{

struct Value;

using MapValue = std::map<Value, Value>;
using RecordValue = std::map<std::string, Value>;
using ListValue = std::vector<Value>;

struct Value
{
  std::variant<std::monostate, bool, int, double, std::string, RecordValue, ListValue, MapValue, QColor, QString> data;

  bool isValid() const;
  bool toBool() const;
  int toInt() const;
  double toDouble() const;
  QString toString() const;
  RecordValue toRecord() const;
  ListValue toList() const;
  MapValue toMap() const;

  QString toReadable() const;

  static Value defaultValue(const koda::types::TypeReference& ref);

  // So Value can be used in sets and maps
  bool operator<(const Value& other) const;
};

struct MissionParameter
{
  std::string id;
  std::string name;
  koda::types::TypeReference type;
  Value value;
};

}  // namespace maki
