#pragma once

#include <QColor>
#include <QMap>
#include <QPointF>
#include <QString>
#include <QVector>

#include "types.h"
#include "typing/type_definition.h"

class INode;
class ITransition;

class IProperty
{
public:
  virtual QString getid() const = 0;
  virtual QVariant getdefaultValue() const = 0;
  virtual QVector<std::shared_ptr<IProperty>> getoptions() const = 0;
  virtual Types::PropertyTypes gettype() const = 0;
  virtual Types::ControlTypes getcontrol() const = 0;
};

class IFlow
{
public:
  virtual QString getid() const = 0;
  virtual QString getname() const = 0;
  virtual QString getowner() const = 0;
  virtual bool getmodifiable() const = 0;
  virtual int getlinksTo() const = 0;

  virtual Types::CallType gettype() const = 0;
  virtual Types::PropertyTypes getreturnType() const = 0;
  virtual QVector<std::shared_ptr<IProperty>> getarguments() const = 0;

  virtual QVector<std::shared_ptr<INode>> getnodes() const = 0;
  virtual QVector<std::shared_ptr<ITransition>> gettransitions() const = 0;
  virtual QVector<std::shared_ptr<ITransition>> gettransitions(const QString& srcId) const = 0;
};

class ITransition
{
public:
  virtual QString getid() const = 0;
  virtual QString getlabel() const = 0;
  virtual QString getevent() const = 0;

  virtual QString getsrcId() const = 0;
  virtual QString getdstId() const = 0;
};

class INode
{
public:
  virtual QString getid() const = 0;
  virtual QString getnodeId() const = 0;
  virtual QPointF getposition() const = 0;
  virtual QVector<std::shared_ptr<IProperty>> getfields() const = 0;
  virtual QMap<QString, QVariant> getproperties() const = 0;
  virtual QVector<std::shared_ptr<IFlow>> getflows() const = 0;
  virtual QVector<std::shared_ptr<IFlow>> getevents() const = 0;
  virtual QString getparentId() const = 0;
  virtual QVector<std::shared_ptr<INode>> getchildren() const = 0;
};

class IValue
{
public:
  enum class Kind
  {
    Invalid,
    Bool,
    Int,
    Double,
    StdString,
    Record,
    List,
    Map,
    Color,
    QString
  };

  using IValuePtr = std::shared_ptr<IValue>;
  using IListValue = std::vector<IValuePtr>;
  using IMapValue = std::map<IValuePtr, IValuePtr>;
  using IRecordValue = std::map<std::string, IValuePtr>;

  using Data = std::variant<std::monostate, bool, int, double, std::string, IRecordValue, IListValue, IMapValue, QColor, QString>;

  virtual ~IValue() = default;

  virtual Kind kind() const = 0;
  virtual bool isValid() const = 0;

  virtual bool toBoolValue() const = 0;
  virtual int toIntValue() const = 0;
  virtual double toDoubleValue() const = 0;
  virtual QString toStringValue() const = 0;

  virtual IRecordValue toRecordValue() const = 0;
  virtual IListValue toListValue() const = 0;
  virtual IMapValue toMapValue() const = 0;
};

class IParameter
{
public:
  virtual ~IParameter() = default;

  virtual std::string getid() const = 0;
  virtual std::string getname() const = 0;
  virtual koda::types::TypeReference gettype() const = 0;
  virtual const std::shared_ptr<IValue> getvalue() const = 0;
};

class IDocument
{
public:
  virtual ~IDocument() = default;

  virtual QVector<std::shared_ptr<INode>> getnodes() const = 0;
  virtual QVector<koda::types::TypeDefinition> gettypes() const = 0;
  virtual QVector<const IParameter*> getparameters() const = 0;
};