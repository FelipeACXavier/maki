#pragma once

#include <QGraphicsEllipseItem>

#include "connection.h"
#include "save_info.h"

class QGraphicsSceneHoverEvent;

class Connector : public QGraphicsEllipseItem
{
public:
  enum
  {
    Type = UserType + Types::CONNECTOR
  };

  Connector(const ConnectorConfig& config, QGraphicsItem* parent);
  Connector(const ConnectorConfig& config, const QString& id, QGraphicsItem* parent);
  virtual ~Connector();

  QString id() const;
  int type() const override;

  QPointF center() const;
  QPair<QPointF, QPointF> shift() const;

  QString connectorId() const;
  Types::ConnectorType connectorType() const;
  QVector<ConnectionItem*> connections() const;
  QVector<ConnectionItem*> connectionsFromThis() const;
  QVector<ConnectionItem*> connectionsToThis() const;

  void updateConnections();
  void addConnection(ConnectionItem* connection);
  void removeConnection(ConnectionItem* connection);

  void updateColor(bool accent);

  ConnectorSaveInfo saveInfo() const;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  const QString mId;

  std::shared_ptr<ConnectorConfig> mConfig;
  QVector<ConnectionItem*> mConnections;

  Qt::GlobalColor typeToColor(Types::ConnectorType type) const;
  void initialize();
};
