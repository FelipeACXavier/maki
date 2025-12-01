#include "process_tab.h"

#include <QScrollBar>
#include <QTextBrowser>
#include <QVBoxLayout>

#include "compiler/pipeline.h"

ProcessTab::ProcessTab(Pipeline* pipeline, QWidget* parent)
    : QWidget(parent)
    , mOutput(new QTextBrowser(this))
    , mPipeline(pipeline)
{
  mOutput->setReadOnly(true);
  // mOutput->setWordWrapMode(QTextOption::NoWrap);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mOutput);

  // Merge stdout + stderr into one stream if you prefer
  // mPipeline->setProcessChannelMode(QProcess::SeparateChannels);

  connect(mPipeline, &Pipeline::startingProcess, this, &ProcessTab::onStartingProcess);
  connect(mPipeline, &Pipeline::finished, this, &ProcessTab::onFinished);
  connect(mPipeline, &Pipeline::finishedLast, this, &ProcessTab::onFinishedLast);
  connect(mPipeline, &Pipeline::readyReadStandardOutput, this, &ProcessTab::onReadyReadStandardOutput);
  connect(mPipeline, &Pipeline::readyReadStandardOutput, this, &ProcessTab::onReadyReadStandardOutput);
  connect(mPipeline, &Pipeline::errorOccurred, this, &ProcessTab::onErrorOccurred);
}

void ProcessTab::onReadyReadStandardOutput(const QString& message)
{
  // const QString text = QString::fromLocal8Bit(mPipeline->readAllStandardOutput());
  appendText(message);
}

void ProcessTab::onReadyReadStandardError(const QString& message)
{
  // const QString text = QString::fromLocal8Bit(mPipeline->readAllStandardError());
  appendText(message);
}

void ProcessTab::onFinished(int exitCode, QProcess::ExitStatus status)
{
  appendText(QString("[Process finished with code %1]\n").arg(exitCode));
}

void ProcessTab::onFinishedLast()
{
  appendText(QString("Finished all process in the pipeline\n"));
  emit processFinished(0, QProcess::ExitStatus::NormalExit);
}

void ProcessTab::onStartingProcess(const QString& process, const QStringList& arguments)
{
  appendText(QString("> %1 %2\n").arg(process, arguments.join(' ')));
}

void ProcessTab::onErrorOccurred(QProcess::ProcessError error)
{
  appendText(QString("\n[Process error: %1]\n").arg(static_cast<int>(error)));
  emit processFinished(1, QProcess::ExitStatus::CrashExit);
}

void ProcessTab::appendText(const QString& text)
{
  mOutput->moveCursor(QTextCursor::End);
  mOutput->append(text);
  mOutput->verticalScrollBar()->setValue(mOutput->verticalScrollBar()->maximum());
}
