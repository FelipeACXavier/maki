#include "local_server_tab.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineView>

LocalServerTab::LocalServerTab(QWidget* parent)
    : QWidget(parent)
{
  mUrlEdit = new QLineEdit(this);
  mReloadButton = new QPushButton(tr("Reload"), this);
  mView = new QWebEngineView(this);

  const QUrl url("http://localhost:80");
  mView->load(url);

  // auto* topBarLayout = new QHBoxLayout();
  // topBarLayout->setContentsMargins(0, 0, 0, 0);
  // topBarLayout->addWidget(new QLabel(tr("URL:"), this));
  // topBarLayout->addWidget(mUrlEdit);
  // topBarLayout->addWidget(mReloadButton);

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  // mainLayout->addLayout(topBarLayout);
  mainLayout->addWidget(mView);

  // connect(mReloadButton, &QPushButton::clicked, this, &LocalServerTab::onReloadClicked);
  // connect(mUrlEdit, &QLineEdit::returnPressed, this, &LocalServerTab::onReloadClicked);
  connect(mView, &QWebEngineView::loadStarted, this, &LocalServerTab::onLoadStarted);
  connect(mView, &QWebEngineView::loadFinished, this, &LocalServerTab::onLoadFinished);
}

void LocalServerTab::connectToServer(const QString& host)
{
  // const QUrl url(QStringLiteral("http://%1:%2").arg(host).arg(port));
  setUrl(host);
}

void LocalServerTab::setUrl(const QUrl& url)
{
  // mUrlEdit->setText(url.toString());
  mView->load(url);
}

void LocalServerTab::onReloadClicked()
{
  // QUrl url = QUrl::fromUserInput(mUrlEdit->text());
  // if (!url.isEmpty())
  // {
  //   mView->load(url);
  // }
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
    // emit pageTitleChanged(mView->title());
  }
  else
  {
    // Could show an error or status message
  }
}
