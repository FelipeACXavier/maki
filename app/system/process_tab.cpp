#include "process_tab.h"

#include <QScrollBar>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTimer>
#include <QVBoxLayout>

#include "compiler/pipeline.h"
#include "logging.h"

ProcessTab::ProcessTab(QWidget* parent)
    : QWidget(parent)
{
  mOutput = new QTextBrowser(this);
  mOutput->setReadOnly(true);
  mOutput->setWordWrapMode(QTextOption::NoWrap);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(mOutput);
}

void ProcessTab::setPipeline(Pipeline* pipeline)
{
  if (!pipeline)
    return;

  mPipeline = pipeline;

  connect(mPipeline, &Pipeline::startingProcess, this, &ProcessTab::onStartingProcess);
  connect(mPipeline, &Pipeline::finished, this, &ProcessTab::onFinished);
  connect(mPipeline, &Pipeline::finishedLast, this, &ProcessTab::onFinishedLast);
  connect(mPipeline, &Pipeline::readyReadStandardOutput, this, &ProcessTab::onReadyReadStandardOutput);
  connect(mPipeline, &Pipeline::readyReadStandardError, this, &ProcessTab::onReadyReadStandardError);
  connect(mPipeline, &Pipeline::errorOccurred, this, &ProcessTab::onErrorOccurred);
}

void ProcessTab::onReadyReadStandardOutput(const QByteArray& message)
{
  handleProcessData(message);
}

void ProcessTab::onReadyReadStandardError(const QByteArray& message)
{
  handleProcessData(message);
}

void ProcessTab::onFinished(const Pipeline::Info& /* info */, int exitCode, QProcess::ExitStatus status)
{
  appendText(QString("[Process finished with code %1]\n").arg(exitCode));
}

void ProcessTab::onFinishedLast(const Pipeline::Info& /* info */, int exitCode, const QString& /* message */)
{
  appendText(QString("Finished all process in the pipeline\n"));
  emit processFinished(exitCode, QProcess::ExitStatus::NormalExit);
}

void ProcessTab::onStartingProcess(const Pipeline::Info& /* info */, const QString& process, const QStringList& arguments)
{
  appendText(QString("> %1 %2\n\n").arg(process, arguments.join(' ')));

  emit processStarted();
}

void ProcessTab::onErrorOccurred(const Pipeline::Info& /* info */, QProcess::ProcessError error, const QString& message)
{
  appendText(QString("\n[Process error: %1] %2\n").arg(static_cast<int>(error)).arg(message));
  emit processFinished(1, QProcess::ExitStatus::CrashExit);
}

void ProcessTab::appendText(const QString& text)
{
  mOutput->moveCursor(QTextCursor::End);
  mOutput->append(text);
  mOutput->verticalScrollBar()->setValue(mOutput->verticalScrollBar()->maximum());
}

void ProcessTab::handleProcessData(const QByteArray& raw)
{
  if (raw.isEmpty())
    return;

  QByteArray buf = raw;

  QTextCursor cursor = mOutput->textCursor();
  cursor.movePosition(QTextCursor::End);

  int i = 0;
  // For more information on ANSI codes, see:
  // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
  while (i < buf.size())
  {
    unsigned char ch = buf.at(i);

    if (ch == 0x1b && i + 1 < buf.size() && buf.at(i + 1) == '[')
    {
      // CSI sequence: ESC [
      int start = i + 2;
      int j = start;

      // find final byte in @–~ range
      while (j < buf.size() && !(buf.at(j) >= 0x40 && buf.at(j) <= 0x7e))
        ++j;

      // incomplete sequence at end of chunk – just stop
      if (j >= buf.size())
        break;

      char finalByte = buf.at(j);
      QByteArray paramsBytes = buf.mid(start, j - start);
      QString paramsStr = QString::fromLatin1(paramsBytes);

      if (finalByte == 'F')
      {
        // Cursor to previous line
        mOverwriteNextLine = paramsStr.isEmpty() ? 1 : paramsStr.toInt();
        // cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, mOverwriteNextLine);
      }
      else if (finalByte == 'm')
      {
        // SGR (colors etc.)
        QList<int> codes;
        for (const QString& part : paramsStr.split(';', Qt::SkipEmptyParts))
          codes.append(part.toInt());

        applySgr(codes);
      }
      else if (finalByte == 'h' || finalByte == 'l')
      {
        // Often ESC[?25h / ESC[?25l – cursor show/hide. Safe to ignore.
      }
      else if (finalByte == 'J')
      {
        // Erase in display
        int mode = paramsStr.isEmpty() ? 0 : paramsStr.toInt();

        // Clear from cursor to end
        if (mode == 0)
          cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

        // Clear from cursor to start
        else if (mode == 1)
          cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

        // Clear entire "screen"
        else if (mode == 2)
          cursor.select(QTextCursor::Document);
      }
      else
      {
        // Other CSI – ignore for now
        LOG_DEBUG("Unhandled ANSI code: %s - %c", qPrintable(paramsStr), finalByte);
      }

      i = j + 1;
      continue;
    }

    // Normal text
    int textStart = i;

    // Get moving until we find the start of an escape sequence
    while (i < buf.size() && buf.at(i) != 0x1b)
      ++i;

    // deleteLastLine(cursor);

    QString text = QString::fromLocal8Bit(buf.constData() + textStart, i - textStart);
    if (!text.isEmpty())
    {
      if (mOverwriteNextLine > 0)
      {
        cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, mOverwriteNextLine);
        cursor.removeSelectedText();
        mOverwriteNextLine = 0;
      }

      cursor.insertText(text, mCurrentFormat);
    }
  }

  mOutput->setTextCursor(cursor);
  mOutput->verticalScrollBar()->setValue(mOutput->verticalScrollBar()->maximum());
}

void ProcessTab::deleteLastLine(QTextCursor& cursor)
{
  if (mOverwriteNextLine > 0)
    return;

  cursor.movePosition(QTextCursor::End);

  if (!cursor.atStart())
  {
    cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, mOverwriteNextLine);
    cursor.removeSelectedText();
  }

  cursor.movePosition(QTextCursor::End);

  mOverwriteNextLine = 0;
}

static QColor ansiBasicColor(int code, bool bright)
{
  // 0–7 (black, red, green, yellow, blue, magenta, cyan, white)
  static const QVector<QColor> base = {
      QColor(0, 0, 0),
      QColor(205, 49, 49),
      QColor(13, 188, 121),
      QColor(229, 229, 16),
      QColor(36, 114, 200),
      QColor(188, 63, 188),
      QColor(17, 168, 205),
      QColor(229, 229, 229)};

  static const QVector<QColor> brightBase = {
      QColor(102, 102, 102),
      QColor(241, 76, 76),
      QColor(35, 209, 139),
      QColor(245, 245, 67),
      QColor(59, 142, 234),
      QColor(214, 112, 214),
      QColor(41, 184, 219),
      QColor(255, 255, 255)};

  int idx = qBound(0, code, 7);
  return bright ? brightBase[idx] : base[idx];
}

void ProcessTab::applySgr(const QList<int>& codes)
{
  if (codes.isEmpty())
  {
    mCurrentFormat = QTextCharFormat{};
    return;
  }

  int i = 0;
  while (i < codes.size())
  {
    int c = codes.at(i);

    switch (c)
    {
      case 0:  // reset
        mCurrentFormat = QTextCharFormat{};
        break;
      case 1:  // bold
        mCurrentFormat.setFontWeight(QFont::Bold);
        break;
      case 22:  // normal intensity
        mCurrentFormat.setFontWeight(QFont::Normal);
        break;

      // 30–37: foreground
      case 30:
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      {
        int idx = c - 30;
        mCurrentFormat.setForeground(ansiBasicColor(idx, false));
        break;
      }

      // 40–47: background
      case 40:
      case 41:
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
      {
        int idx = c - 40;
        mCurrentFormat.setBackground(ansiBasicColor(idx, false));
        break;
      }

      // 90–97: bright foreground
      case 90:
      case 91:
      case 92:
      case 93:
      case 94:
      case 95:
      case 96:
      case 97:
      {
        int idx = c - 90;
        mCurrentFormat.setForeground(ansiBasicColor(idx, true));
        break;
      }

      // 100–107: bright background
      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
      case 105:
      case 106:
      case 107:
      {
        int idx = c - 100;
        mCurrentFormat.setBackground(ansiBasicColor(idx, true));
        break;
      }

      default:
        // 256-colour sequences: 38;5;n / 48;5;n
        if ((c == 38 || c == 48) && i + 2 < codes.size() && codes.at(i + 1) == 5)
        {
          int n = codes.at(i + 2);  // 0–255
          QColor col;

          if (n >= 232)
          {
            // greyscale ramp
            int level = 8 + (n - 232) * 10;
            col = QColor(level, level, level);
          }
          else
          {
            // 6x6x6 colour cube
            int r = (n / 36) % 6;
            int g = (n / 6) % 6;
            int b = n % 6;
            auto to255 = [](int v) { return v == 0 ? 0 : 55 + v * 40; };
            col = QColor(to255(r), to255(g), to255(b));
          }

          if (c == 38)
            mCurrentFormat.setForeground(col);
          else
            mCurrentFormat.setBackground(col);

          i += 2;  // consumed extra parameters
        }
        break;
    }

    ++i;
  }
}
