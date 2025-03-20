#pragma once

#include <QGraphicsEllipseItem>

#include "connection.h"
#include "inode.h"
#include "save_info.h"

class QGraphicsSceneHoverEvent;

class Connector : public QGraphicsEllipseItem, public IConnector
{
public:
  enum
  {
    Type = Types::CONNECTOR
  };

  Connector(const ConnectorConfig& config, QGraphicsItem* parent);
  Connector(const ConnectorConfig& config, const QString& id, QGraphicsItem* parent);
  virtual ~Connector();

  QString id() const override;
  int type() const override;

  QPointF center() const;
  QPair<QPointF, QPointF> shift() const;

  QString connectorName() const override;
  Types::ConnectorType connectorType() const override;
  QVector<IConnection*> connections() const override;
  QVector<IConnection*> connectionsFromThis() const override;
  QVector<IConnection*> connectionsToThis() const override;

  void updateConnections();
  void addConnection(ConnectionItem* connection);
  void removeConnection(ConnectionItem* connection);

  void updateColor(bool accent);
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  ConnectorSaveInfo saveInfo() const;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  const QString mId;

  std::shared_ptr<ConnectorConfig> mConfig;
  QVector<IConnection*> mConnections;

  Qt::GlobalColor typeToColor(Types::ConnectorType type) const;
  void initialize();
};
