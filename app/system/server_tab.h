#pragma once

#include <QUrl>
#include <QWidget>

class QLineEdit;
class QPushButton;
class QWebEngineView;

class LocalServerTab : public QWidget
{
  Q_OBJECT

public:
  explicit LocalServerTab(QWidget* parent = nullptr);

  // Convenience: set host/port directly
  void connectToServer(const QString& host, quint16 port);

  // Or pass a full URL if you prefer
  void setUrl(const QUrl& url);

private slots:
  void onReloadClicked();
  void onLoadStarted();
  void onLoadFinished(bool ok);

private:
  QLineEdit* m_urlEdit;
  QPushButton* m_reloadButton;
  QWebEngineView* m_view;
};
