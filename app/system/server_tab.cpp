#include "server_tab.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineView>

LocalServerTab::LocalServerTab(QWidget* parent)
    : QWidget(parent)
{
  m_urlEdit = new QLineEdit(this);
  m_reloadButton = new QPushButton(tr("Reload"), this);
  m_view = new QWebEngineView(this);

  auto* topBarLayout = new QHBoxLayout();
  topBarLayout->setContentsMargins(0, 0, 0, 0);
  topBarLayout->addWidget(new QLabel(tr("URL:"), this));
  topBarLayout->addWidget(m_urlEdit);
  topBarLayout->addWidget(m_reloadButton);

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addLayout(topBarLayout);
  mainLayout->addWidget(m_view);

  connect(m_reloadButton, &QPushButton::clicked, this, &LocalServerTab::onReloadClicked);
  connect(m_urlEdit, &QLineEdit::returnPressed, this, &LocalServerTab::onReloadClicked);
  connect(m_view, &QWebEngineView::loadStarted, this, &LocalServerTab::onLoadStarted);
  connect(m_view, &QWebEngineView::loadFinished, this, &LocalServerTab::onLoadFinished);
}

void LocalServerTab::connectToServer(const QString& host, quint16 port)
{
  const QUrl url(QStringLiteral("http://%1:%2").arg(host).arg(port));
  setUrl(url);
}

void LocalServerTab::setUrl(const QUrl& url)
{
  m_urlEdit->setText(url.toString());
  m_view->load(url);
}

void LocalServerTab::onReloadClicked()
{
  QUrl url = QUrl::fromUserInput(m_urlEdit->text());
  if (!url.isEmpty())
  {
    m_view->load(url);
  }
}

void LocalServerTab::onLoadStarted()
{
  // Optional: change tab text to “Loading…”
}

void LocalServerTab::onLoadFinished(bool ok)
{
  // Optional: update tab title based on page title
  if (ok)
  {
    // Example: set the QTabWidget tab text from outside via signal, if you like
    // emit pageTitleChanged(m_view->title());
  }
  else
  {
    // Could show an error or status message
  }
}
