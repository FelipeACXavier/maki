#pragma once

#include <QGraphicsItem>
#include <QString>

#include "property_config.h"
#include "types.h"

class INode
{
public:
  virtual ~INode() = default;

  virtual QString nodeName() const = 0;
  virtual QString nodeType() const = 0;
  virtual QMap<QString, QVariant> properties() const = 0;

  virtual Types::LibraryTypes function() const = 0;

  virtual QVariant getProperty(const QString& key) const = 0;

  virtual QVector<PropertiesConfig> fields() const = 0;
  virtual PropertiesConfig getField(const QString& key) const = 0;

  virtual INode* parentNode() const = 0;
  virtual QVector<INode*> children() const = 0;
};
