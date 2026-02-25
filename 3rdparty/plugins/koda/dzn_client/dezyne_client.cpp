#include "dezyne_client.h"

#include "logging.h"

DezyneClient::DezyneClient(QObject* parent)
    : QObject(parent)
{
  mReconnectTimer.setSingleShot(true);
  mPingTimer.setSingleShot(false);
  wireSignals();
}

DezyneClient::~DezyneClient()
{
  disconnectFromServer();
}

void DezyneClient::wireSignals()
{
  connect(&mSocket, &QWebSocket::connected, this, &DezyneClient::onConnected);
  connect(&mSocket, &QWebSocket::disconnected, this, &DezyneClient::onDisconnected);
  connect(&mSocket, &QWebSocket::textMessageReceived, this, &DezyneClient::onTextMessageReceived);

  connect(&mSocket, &QWebSocket::stateChanged, this, [this](QAbstractSocket::SocketState s) {
    emit stateChanged(s);
  });

  connect(&mSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &DezyneClient::onError);

  connect(&mReconnectTimer, &QTimer::timeout, this, &DezyneClient::tryReconnect);
  connect(&mPingTimer, &QTimer::timeout, this, &DezyneClient::sendPing);
}

void DezyneClient::setUrl(const QUrl& url)
{
  mUrl = url;
}

QUrl DezyneClient::url() const
{
  return mUrl;
}

void DezyneClient::setAutoReconnect(bool enabled)
{
  mAutoReconnect = enabled;
  if (!mAutoReconnect)
    stopReconnectTimer();
}

bool DezyneClient::autoReconnect() const
{
  return mAutoReconnect;
}

void DezyneClient::setReconnectIntervalMs(int ms)
{
  mReconnectIntervalMs = qMax(250, ms);
}
int DezyneClient::reconnectIntervalMs() const
{
  return mReconnectIntervalMs;
}

void DezyneClient::setPingIntervalMs(int ms)
{
  mPingIntervalMs = qMax(0, ms);
  if (mPingIntervalMs == 0)
    stopPingTimer();
  else if (state() == QAbstractSocket::ConnectedState)
    startPingTimer();
}
int DezyneClient::pingIntervalMs() const
{
  return mPingIntervalMs;
}

void DezyneClient::connectToServer()
{
  if (!mUrl.isValid())
  {
    emit errorOccurred(QStringLiteral("WebSocket URL is not valid."));
    return;
  }

  // Avoid double-open
  if (state() == QAbstractSocket::ConnectingState ||
      state() == QAbstractSocket::ConnectedState)
  {
    return;
  }

  stopReconnectTimer();
  mSocket.open(mUrl);
}

void DezyneClient::disconnectFromServer()
{
  stopReconnectTimer();
  stopPingTimer();
  mSocket.close();
}

QAbstractSocket::SocketState DezyneClient::state() const
{
  return mSocket.state();
}

bool DezyneClient::sendJson(const QJsonObject& obj)
{
  if (state() != QAbstractSocket::ConnectedState)
  {
    emit errorOccurred(QStringLiteral("Cannot send: WebSocket is not connected."));
    return false;
  }

  const QJsonDocument doc(obj);
  const QString text = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

  mSocket.sendTextMessage(text);
  emit sent(text);
  return true;
}

bool DezyneClient::sendText(const QString& text)
{
  if (state() != QAbstractSocket::ConnectedState)
  {
    emit errorOccurred(QStringLiteral("Cannot send: WebSocket is not connected."));
    return false;
  }
  mSocket.sendTextMessage(text);
  emit sent(text);
  return true;
}

void DezyneClient::onConnected()
{
  stopReconnectTimer();
  if (mPingIntervalMs > 0)
    startPingTimer();

  emit connected();
}

void DezyneClient::onDisconnected()
{
  stopPingTimer();
  emit disconnected();
  if (mAutoReconnect)
    startReconnectTimer();
}

void DezyneClient::onTextMessageReceived(const QString& message)
{
  emit messageReceivedRaw(message);

  QJsonParseError err{};
  const auto doc = QJsonDocument::fromJson(message.toUtf8(), &err);
  if (err.error != QJsonParseError::NoError || !doc.isObject())
    return;

  emit messageReceivedJson(doc.object());
}

void DezyneClient::onError(QAbstractSocket::SocketError error)
{
  emit errorOccurred(mSocket.errorString());

  // Some servers close immediately if auth fails, etc.
  // Reconnect only if enabled and we are not connected.
  if (mAutoReconnect && state() != QAbstractSocket::ConnectedState)
  {
    startReconnectTimer();
  }
}

void DezyneClient::startReconnectTimer()
{
  if (!mAutoReconnect)
    return;
  if (mReconnectTimer.isActive())
    return;
  mReconnectTimer.start(mReconnectIntervalMs);
}

void DezyneClient::stopReconnectTimer()
{
  if (mReconnectTimer.isActive())
    mReconnectTimer.stop();
}

void DezyneClient::tryReconnect()
{
  // Don’t reconnect if user already connected manually
  if (state() == QAbstractSocket::ConnectedState ||
      state() == QAbstractSocket::ConnectingState)
  {
    return;
  }
  connectToServer();
}

void DezyneClient::startPingTimer()
{
  if (mPingIntervalMs <= 0)
    return;
  if (mPingTimer.isActive())
    mPingTimer.stop();
  mPingTimer.start(mPingIntervalMs);
}

void DezyneClient::stopPingTimer()
{
  if (mPingTimer.isActive())
    mPingTimer.stop();
}

void DezyneClient::sendPing()
{
  if (state() != QAbstractSocket::ConnectedState)
    return;

  // Minimal ping; payload optional.
  mSocket.ping("ping");
}
