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
using ValueMap = std::map<QString, Value>;

class Value : public IValue
{
public:
  std::variant<std::monostate, bool, int, double, std::string, RecordValue, ListValue, MapValue, QColor, QString> data;

  static Value createBool(bool value);
  static Value createInt(int value);
  static Value createReal(double value);
  static Value createString(const QString& value);
  static Value createList(const ListValue& value);
  static Value createRecord(const RecordValue& value);
  static Value createMap(const MapValue& value);

  bool isBool() const;
  bool isInt() const;
  bool isReal() const;
  bool isString() const;
  bool isList() const;
  bool isRecord() const;
  bool isMap() const;

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

  QString getid() const override;
  QString getname() const override;
  koda::types::TypeReference gettype() const override;
  const IValue* getvalue() const override;
  Types::ControlTypes getcontrol() const override;

  QJsonObject toJson() const;
  static Result<MissionParameter> fromJson(const QJsonObject& json);
};

QDataStream& operator<<(QDataStream& out, const maki::Value& value);
QDataStream& operator>>(QDataStream& in, maki::Value& value);

// Helpers
const maki::Value* asValue(const IValue* value);
maki::Value parameterValue(const IParameter* parameter);
maki::RecordValue parameterRecord(const IParameter* parameter);
QString recordString(const maki::RecordValue& record, const QString& key);
maki::ListValue recordList(const maki::RecordValue& record, const QString& key);

}  // namespace maki
