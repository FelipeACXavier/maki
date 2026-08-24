#include "style_helpers.h"

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QTabBar>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QWidgetAction>

#include "app_configs.h"
#include "app_paths.h"
#include "logging.h"
#include "string_helpers.h"

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

QIcon iconFromTheme(const QString& name, bool fallback)
{
  if (fallback)
    return QIcon(AppPaths::icon(name + ".svg"));

  return QIcon::fromTheme(name, QIcon(AppPaths::icon(name + ".svg")));
}

QString timeToQT(std::chrono::system_clock::time_point now)
{
  // const struct tm* time = localtime(&ts.tv_sec);
  // return QString::fromStdString(Format("%2d:%02d:%02d.%03ld", time->tm_hour, time->tm_min, time->tm_sec, ts.tv_nsec));

  auto secs = std::chrono::time_point_cast<std::chrono::seconds>(now);
  auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - secs).count();
  auto t = std::chrono::system_clock::to_time_t(secs);
  std::tm tm = logging::ToLocalTm(t);

  return QString::fromStdString(Format("%02d:%02d:%02d.%09lld", tm.tm_hour, tm.tm_min, tm.tm_sec, static_cast<long long>(micros)));
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

oclero::qlementine::StatusBadge logLevelToStatusBadge(logging::LogLevel logLevel)
{
  switch (logLevel)
  {
    case logging::LogLevel::Error:
      return oclero::qlementine::StatusBadge::Error;
    case logging::LogLevel::Warning:
      return oclero::qlementine::StatusBadge::Warning;
    case logging::LogLevel::Info:
    case logging::LogLevel::Debugging:
    case logging::LogLevel::Trace:
    default:
      return oclero::qlementine::StatusBadge::Info;
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
      else if (auto action = qobject_cast<QAction*>(item.widget))
      {
        action->setIcon(addIconWithColor(item.path, color));
      }
      else if (auto tree = qobject_cast<QTreeWidget*>(item.widget))
      {
        if (QTreeWidgetItem* widget = tree->itemFromIndex(item.mindex))
          widget->setIcon(0, addIconWithColor(item.path, color));
      }
      else
      {
        LOG_WARNING("Unsupported widget: {}", item.widget->metaObject()->className());
      }
    }
  }
}

void updateTreeIconTheme(QList<TreeWidgetWithIcon>& icons)
{
  for (auto& item : icons)
  {
    if (item.widget)
    {
      QColor color = item.color.isValid() ? item.color : Config::FOREGROUND;
      item.widget->setIcon(0, addIconWithColor(item.path, color));
    }
  }
}

QString htmlEscape(const QString& s)
{
  return s.toHtmlEscaped();
}

QString createInformationMessage(const NodeConfig& node)
{
  QString out;
  out += R"(
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
  body { font-family: sans-serif; font-size: 14pt; }
  p  { margin: 0 0 0 8px; }

  table.info {
    width: 80%;
    border-collapse: collapse;
    margin: 6px 0 12px 0;
  }
  table.info td {
    border-bottom: 1px solid #999;
    padding: 6px 8px;
    vertical-align: top;
  }
  table.info th {
    font-weight: 600;
    text-align: left;
    padding: 6px 8px;
    vertical-align: top;
    border-bottom: 4px solid #999;
  }
  .section-title {
    font-size: 20pt;
    margin: 8px 0 8px 0;
  }
</style>
</head>
<body>
)";

  // Title / message
  if (!node.help.message.isEmpty())
  {
    out += "<h3>Description</h3>";
    out += "<p> Info: " + htmlEscape(node.help.message) + "</p>";
    out += "<p> Type: " + htmlEscape(node.type) + "</p>";
  }

  // Properties
  if (!node.properties.isEmpty())
  {
    out += "<h4 class='section-title'>Properties</h4>";
    out += R"(
      <table class="info">
        <thead>
          <tr>
            <th width="10%">Id</th>
            <th width="10%">Type</th>
            <th width="30%">Default</th>
            <th width="50%">Info</th>
          </tr>
        </thead>
        <tbody>
      )";

    for (const auto& prop : node.properties)
    {
      out += "<tr>";
      out += "<td>" + htmlEscape(prop.id) + "</td>";
      out += "<td>" + htmlEscape(QString::fromStdString(prop.type.toString())) + "</td>";
      out += "<td>" + htmlEscape(prop.defaultValue.isValid() ? prop.defaultValue.toString() : "\"\"") + "</td>";
      out += "<td>" + htmlEscape(prop.info) + "</td>";
      out += "</tr>";
    }

    out += "</tbody></table>";
  }

  // Events
  if (!node.events.isEmpty())
  {
    out += "<h4 class='section-title'>Events</h4>";
    out += R"(
      <table class="info">
        <thead>
          <tr>
            <th width="20%">Id</th>
            <th width="10%">Type</th>
            <th width="20%">Return Type</th>
            <th width="50%">Description</th>
          </tr>
        </thead>
        <tbody>
      )";

    for (const auto& e : node.events)
    {
      out += "<tr>";
      out += "<td>" + htmlEscape(e.name) + "</td>";
      out += "<td>" + htmlEscape(Types::CallTypeToString(e.type)) + "</td>";
      out += "<td>" + htmlEscape(QString::fromStdString(e.returnType.toString())) + "</td>";
      out += "<td>" + htmlEscape(e.info) + "</td>";
      out += "</tr>";
    }

    out += "</tbody></table>";
  }

  out += "</body></html>";
  return out;
}

QString createDefaultMessage()
{
  return QString("<table width='100%'><tr>"
                 "  <td style='vertical-align:top;'>"
                 "    <h2>Welcome to " +
                 Config::APPLICATION_NAME +
                 "</h2>"
                 "    <p>MAKI is a low-code platform that helps you design and orchestrate your systems.</p>"
                 "    <p><b>To get started:</b></p>"
                 "    <ul>"
                 "      <li>Drag a structural block from the palette onto the Canvas.</li>"
                 "      <li>Right-click and select \"New Flow\".</li>"
                 "      <li>Connect blocks to define the data and control flow.</li>"
                 "      <li>Double-click a block to inspect or adjust its properties.</li>"
                 "      <li>You can run your system with the buttons on the top of the Canvas.</li>"
                 "    </ul>"
                 "    <p>For extra help, hover over any control to see a tooltip, or explore the <b>Help</b> menu.</p>"
                 "    <p>"
                 "      Tip: You can always reset the layout or theme from the <b>Settings</b> menu if things get messy."
                 "    </p>"
                 "  </td>"
                 // IMAGE COLUMN
                 "  <td style='text-align: right; vertical-align: top; padding-top: 10px; padding-right: 10px'>"
                 "    <img src=':/app_icons/maki.png' width='100' style='border-radius:80px; box-shadow: 0px 3px 8px rgba(0,0,0,0.25);' />"
                 "  </td>"
                 "</tr></table>");
}

QString elideLeft(const QString& text, const QWidget* const component)
{
  QFontMetrics fm(component->font());
  return fm.elidedText(text, Qt::ElideLeft, component->maximumWidth());
}

QString elideRight(const QString& text, const QWidget* const component)
{
  QFontMetrics fm(component->font());
  return fm.elidedText(text, Qt::ElideRight, component->maximumWidth());
}

void clearLayout(QLayout* layout, int start)
{
  if (!layout)
    return;

  while (layout->count() > start)
  {
    QLayoutItem* item = layout->takeAt(start);
    if (!item)
      break;

    if (QLayout* childLayout = item->layout())
    {
      clearLayout(childLayout);
      delete item;  // do NOT delete childLayout separately
      continue;
    }

    if (QWidget* widget = item->widget())
    {
      widget->deleteLater();  // or delete widget;
      delete item;
      continue;
    }

    delete item;  // spacer etc.
  }
}
