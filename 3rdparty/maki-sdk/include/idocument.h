#pragma once

#include <QList>
#include <QMap>
#include <QPointF>
#include <QString>
#include <QVector>

#include "types.h"

class INode;

class IProperty
{
public:
  virtual QString getid() const = 0;
  virtual QVariant getdefaultValue() const = 0;
  virtual QList<std::shared_ptr<IProperty>> getoptions() const = 0;
  virtual Types::PropertyTypes gettype() const = 0;
};

class IFlow
{
public:
  virtual QString getid() const = 0;
  virtual QString getname() const = 0;
  virtual QString getowner() const = 0;
  virtual bool getmodifiable() const = 0;

  virtual Types::ConnectorType gettype() const = 0;
  virtual Types::PropertyTypes getreturnType() const = 0;
  virtual QVector<std::shared_ptr<IProperty>> getarguments() const = 0;

  virtual QVector<std::shared_ptr<INode>> getnodes() const = 0;
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
  virtual QVector<std::shared_ptr<ITransition>> gettransitions() const = 0;
  virtual QVector<std::shared_ptr<IFlow>> getflows() const = 0;
  virtual std::shared_ptr<IFlow> getbehaviour() const = 0;
  virtual QString getparentId() const = 0;
  virtual QVector<std::shared_ptr<INode>> getchildren() const = 0;
};

class IDocument
{
public:
  virtual ~IDocument() = default;

  virtual QVector<std::shared_ptr<INode>> getnodes() const = 0;
};