#include "process_tab.h"

#include <QScrollBar>
#include <QTextBrowser>
#include <QVBoxLayout>

ProcessTab::ProcessTab(QWidget* parent)
    : QWidget(parent)
    , m_output(new QTextBrowser(this))
    , m_process(new QProcess(this))
{
  m_output->setReadOnly(true);
  // m_output->setWordWrapMode(QTextOption::NoWrap);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_output);

  // Merge stdout + stderr into one stream if you prefer
  m_process->setProcessChannelMode(QProcess::SeparateChannels);

  connect(m_process, &QProcess::readyReadStandardOutput, this, &ProcessTab::onReadyReadStandardOutput);
  connect(m_process, &QProcess::readyReadStandardError, this, &ProcessTab::onReadyReadStandardError);
  connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ProcessTab::onFinished);
  connect(m_process, &QProcess::errorOccurred, this, &ProcessTab::onErrorOccurred);
}

void ProcessTab::startProcess(const QString& program, const QStringList& arguments)
{
  appendText(QString("> %1 %2\n\n").arg(program, arguments.join(' ')));

  m_process->start(program, arguments);
}

void ProcessTab::onReadyReadStandardOutput()
{
  const QString text = QString::fromLocal8Bit(m_process->readAllStandardOutput());
  appendText(text);
}

void ProcessTab::onReadyReadStandardError()
{
  const QString text = QString::fromLocal8Bit(m_process->readAllStandardError());
  appendText(text);
}

void ProcessTab::onFinished(int exitCode, QProcess::ExitStatus status)
{
  appendText(QString("\n[Process finished with code %1]\n").arg(exitCode));
  emit processFinished(exitCode, status);
}

void ProcessTab::onErrorOccurred(QProcess::ProcessError error)
{
  appendText(QString("\n[Process error: %1]\n").arg(static_cast<int>(error)));
}

void ProcessTab::appendText(const QString& text)
{
  m_output->moveCursor(QTextCursor::End);
  m_output->append(text);
  // m_output->verticalScrollBar()->setValue(m_output->verticalScrollBar()->maximum());
}
