#pragma once

#include <QAbstractSocket>
#include <QJsonObject>
#include <QUrl>
#include <QWebSocketServer>
#include <QtWebSockets/QtWebSockets>

class DezyneClient : public QObject
{
  Q_OBJECT
public:
  DezyneClient(QObject* parent);
  virtual ~DezyneClient();

  void setUrl(const QUrl& url);
  QUrl url() const;

  // Auto-reconnect behaviour
  void setAutoReconnect(bool enabled);
  bool autoReconnect() const;

  void setReconnectIntervalMs(int ms);  // default 2000
  int reconnectIntervalMs() const;

  // Optional ping
  void setPingIntervalMs(int ms);
  int pingIntervalMs() const;

  // Connection control
  void connectToServer();
  void disconnectFromServer();

  // Sending
  bool sendJson(const QJsonObject& obj);
  bool sendText(const QString& text);

  QAbstractSocket::SocketState state() const;

signals:
  void connected();
  void disconnected();
  void stateChanged(QAbstractSocket::SocketState state);

  // Incoming messages
  void messageReceivedRaw(const QString& text);
  void messageReceivedJson(const QJsonObject& obj);
  void simulationUpdated(const QJsonObject& obj);

  // For diagnostics/UI
  void errorOccurred(const QString& error);
  void sent(const QString& text);

private slots:
  void onConnected();
  void onDisconnected();
  void onTextMessageReceived(const QString& message);
  void onError(QAbstractSocket::SocketError error);
  void tryReconnect();
  void sendPing();

private:
  void wireSignals();
  void startReconnectTimer();
  void stopReconnectTimer();
  void startPingTimer();
  void stopPingTimer();

private:
  QUrl mUrl;
  QWebSocket mSocket;

  bool mAutoReconnect = true;
  int mReconnectIntervalMs = 2000;
  QTimer mReconnectTimer;

  int mPingIntervalMs = 0;
  QTimer mPingTimer;
};
