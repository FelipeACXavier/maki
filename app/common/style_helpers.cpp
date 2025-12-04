#include "style_helpers.h"

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QTabBar>
#include <QVariant>
#include <QWidgetAction>

#include "logging.h"
#include "string_helpers.h"
#include "theme.h"

static const char* CLASS = "class";

void addClass(QWidget* widget, const QString& className)
{
  // Append the new class if there is already a class, otherwise just add it
  QVariant variant = widget->property(CLASS);
  QString classList = variant.isValid() ? QStringLiteral("%1 %2").arg(variant.toString(), className) : className;

  widget->setProperty(CLASS, QVariant(classList));
}

QPointF snapToGrid(const QPointF& point, const int gridSize)
{
  qreal snappedX = std::round(point.x() / gridSize) * gridSize;
  qreal snappedY = std::round(point.y() / gridSize) * gridSize;

  return QPointF(snappedX, snappedY);
}

QString ToLabel(const std::string& str)
{
  return ToLabel(QString::fromStdString(str));
}

QString ToLabel(const QString& str)
{
  return QString::fromStdString(ToUpperCase(str.toStdString(), 0, 1));
}

QPixmap applyColorToIcon(const QString& path, const QColor& color)
{
  QPixmap pixmap(path);
  QPainter painter(&pixmap);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.fillRect(pixmap.rect(), color);
  painter.end();
  return pixmap;
}

QIcon addIconWithColor(const QString& path, const QColor& color)
{
  return QIcon(applyColorToIcon(path, color));
}

QString timeToQT(std::chrono::system_clock::time_point now)
{
  // const struct tm* time = localtime(&ts.tv_sec);
  // return QString::fromStdString(Format("%2d:%02d:%02d.%03ld", time->tm_hour, time->tm_min, time->tm_sec, ts.tv_nsec));

  auto secs = std::chrono::time_point_cast<std::chrono::seconds>(now);
  auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - secs).count();
  auto t = std::chrono::system_clock::to_time_t(secs);
  std::tm tm = logging::ToLocalTm(t);

  return QString::fromStdString(Format(
      "%02d:%02d:%02d.%09lld", tm.tm_hour, tm.tm_min, tm.tm_sec, static_cast<long long>(micros)));
}

QString logLevelToQT(logging::LogLevel logLevel)
{
  switch (logLevel)
  {
    case logging::LogLevel::Error:
      return QString("[<font color='red'>E</font>]");
    case logging::LogLevel::Warning:
      return QString("[<font color='yellow'>W</font>]");
    case logging::LogLevel::Info:
      return QString("[<font color='green'>I</font>]");
    case logging::LogLevel::Debugging:
      return QString("[<font color='cyan'>D</font>]");
    case logging::LogLevel::Trace:
      return QString("[<font color='blue'>T</font>]");
    default:
      return QString("[<font color='magenta'>U</font>]");
  }
}

QString toQT(std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& message)
{
  return QStringLiteral("%1 %2: %3").arg(timeToQT(ts), logLevelToQT(level), QString::fromStdString(message));
}

void addDynamicWidget(QVBoxLayout* layout, QWidget* dynamicWidget, QWidget* parent)
{
  // Add dynamic widgets above the parent but below other widgets
  for (int i = 0; i < layout->count(); ++i)
  {
    QWidget* widget = layout->itemAt(i)->widget();
    if (widget != parent)
      continue;

    layout->insertWidget(i - 1, dynamicWidget);
    break;
  }
}

QString getDirPathFor(const QString& path)
{
#ifdef Q_OS_WIN
  return QCoreApplication::applicationDirPath() + "/" + path;
#else
  return QCoreApplication::applicationDirPath() + "/../" + path;
#endif
}

void updateProperty(QWidget* widget, const QString& property, bool value)
{
  widget->setProperty(property.toStdString().c_str(), value);
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);
  widget->update();
}

void applyStyle(QWidget* widget, const QString& style)
{
  widget->setStyleSheet(style);
  widget->style()->unpolish(widget);
  widget->style()->polish(widget);
  widget->update();
}

void addSectionLabel(QMenu* menu, const QString& text)
{
  auto* label = new QLabel(text);
  label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

  auto action = new QWidgetAction(menu);
  action->setDefaultWidget(label);
  menu->addAction(action);

  // Optional: add a thin separator line below it
  menu->addSeparator();
};

void updateIconTheme(QList<WidgetWithIcon>& icons)
{
  for (auto& item : icons)
  {
    if (item.widget)
    {
      QColor color = item.color.isValid() ? item.color : Config::FOREGROUND;
      if (auto label = qobject_cast<QLabel*>(item.widget))
      {
        label->setPixmap(applyColorToIcon(item.path, color).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      }
      else if (auto button = qobject_cast<QPushButton*>(item.widget))
      {
        button->setIcon(addIconWithColor(item.path, color));
      }
      else if (auto tabBar = qobject_cast<QTabBar*>(item.widget))
      {
        if (item.index < tabBar->count())
          tabBar->setTabIcon(item.index, addIconWithColor(item.path, color));
      }
      else
      {
        LOG_WARNING("Unsupported widget");
      }
    }
  }
}
