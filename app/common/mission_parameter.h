#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "idocument.h"
#include "typing/type_reference.h"

namespace maki
{

class Value;

using ListValue = std::vector<Value>;
using MapValue = std::map<Value, Value>;
using RecordValue = std::map<std::string, Value>;

class Value : public IValue
{
public:
  std::variant<std::monostate, bool, int, double, std::string, RecordValue, ListValue, MapValue, QColor, QString> data;

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

  QJsonObject toJson() const;
  static Value fromJson(const QJsonObject& json);

  // All the stuff for the interface
  Kind kind() const override;
  bool isValid() const override;
  bool toBoolValue() const override;
  int toIntValue() const override;
  double toDoubleValue() const override;
  QString toStringValue() const override;
  IRecordValue toRecordValue() const override;
  IListValue toListValue() const override;
  IMapValue toMapValue() const override;
};

class MissionParameter : public IParameter
{
public:
  std::string id;
  std::string name;
  koda::types::TypeReference type;
  Value value;

  std::string getid() const override;
  std::string getname() const override;
  koda::types::TypeReference gettype() const override;
  const std::shared_ptr<IValue> getvalue() const override;

  QJsonObject toJson() const;
  static Result<MissionParameter> fromJson(const QJsonObject& json);
};

}  // namespace maki
