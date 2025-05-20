#pragma once

#include <QGraphicsItem>
#include <QString>

#include "property_config.h"
#include "types.h"

class IConnector;

class IConnection
{
public:
  virtual ~IConnection() = default;

  virtual QString id() const = 0;
  virtual IConnector* source() const = 0;
  virtual IConnector* destination() const = 0;
};

class IConnector
{
public:
  virtual ~IConnector() = default;

  virtual QString id() const = 0;
  virtual QString connectorName() const = 0;
  virtual Types::ConnectorType connectorType() const = 0;
  virtual QVector<IConnection*> connections() const = 0;
  virtual QVector<IConnection*> connectionsFromThis() const = 0;
  virtual QVector<IConnection*> connectionsToThis() const = 0;
};

class INode
{
public:
  virtual ~INode() = default;

  virtual QString nodeType() const = 0;
  virtual QMap<QString, QVariant> properties() const = 0;
  virtual QVector<std::shared_ptr<IConnector>> connectors() const = 0;

  virtual Types::LibraryTypes function() const = 0;

  virtual QVariant getProperty(const QString& key) const = 0;

  virtual QVector<PropertiesConfig> fields() const = 0;
  virtual PropertiesConfig getField(const QString& key) const = 0;

  virtual INode* parentNode() const = 0;
  virtual QVector<INode*> children() const = 0;
};
