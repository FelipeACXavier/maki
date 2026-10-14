#include "style_helpers.h"

#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QSet>
#include <QStyle>
#include <QSvgRenderer>
#include <QTabBar>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QWidgetAction>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "app_paths.h"
#include "keys.h"
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

QString iconPathFromTheme(const QString& name, bool useLocal)
{
  auto iconName = QString(name).replace(".svg", "") + ".svg";
#ifndef __EMSCRIPTEN__
  if (!useLocal)
    return AppPaths::icon(iconName);
#endif
  if (name.contains("plugin"))
    return AppPaths::icon(iconName);

  return AppPaths::icon(iconName);
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

QColor blendColors(const QColor& foreground, const QColor& background, qreal opacity)
{
  opacity = qBound(0.0, opacity, 1.0);
  return QColor::fromRgbF(foreground.redF() * opacity + background.redF() * (1.0 - opacity),
                          foreground.greenF() * opacity + background.greenF() * (1.0 - opacity),
                          foreground.blueF() * opacity + background.blueF() * (1.0 - opacity));
}

QString maki::textStyleCss(maki::Styles styles)
{
  QStringList css;
  if (styles.testFlag(Bold))
    css << "font-weight: 600;";

  if (styles.testFlag(Italic))
    css << "font-style: italic;";

  if (styles.testFlag(Monospace))
    css << "font-family: monospace;";

  if (styles.testFlag(Underline) || styles.testFlag(Strikeout))
  {
    QStringList decorations;
    if (styles.testFlag(Underline))
      decorations << "underline";

    if (styles.testFlag(Strikeout))
      decorations << "line-through";

    css << "text-decoration: " + decorations.join(' ') + ";";
  }

  return css.join(' ');
}

QString referenceHtml(const QString& text, const QString& foreground, const QString& background, const QString& browserBackground)
{
  return QString("<span style='font-family: monospace; color: %1; background-color: %2; padding: 1px 4px;'>"
                 "  %3"
                 "</span>")
      .arg(foreground, blendColors(QColor(background), QColor(browserBackground), 0.3).name(), htmlEscape(text));
}

QString formatDescriptionHtml(const QString& description, const FlowConfig& event)
{
  const auto theme = oclero::qlementine::appStyle()->theme();
  const QString textColor = theme.secondaryColor.name();
  const QString argumentBackground = theme.primaryColorHovered.name();
  const QString eventBackground = theme.semiTransparentColor1.name();
  const QString background = theme.backgroundColorMain1.name();

  static const QRegularExpression pattern(R"((@[A-Za-z_][A-Za-z0-9_]*|\$[A-Za-z_][A-Za-z0-9_]*))");
  static const QSet<QString> eventReferences = {"trigger", "return", "error", "abort"};

  QString result;
  qsizetype lastPos = 0;

  auto matches = pattern.globalMatch(description);
  while (matches.hasNext())
  {
    const auto match = matches.next();

    // Escape the ordinary text preceding the reference.
    result += htmlEscape(description.mid(lastPos, match.capturedStart() - lastPos));

    const QString reference = match.captured(0);
    const QString name = reference.mid(1);

    if (reference.startsWith('@'))
      result += referenceHtml(name, textColor, argumentBackground, background);
    else if (eventReferences.contains(name))
      result += referenceHtml(name, textColor, eventBackground, background);
    else
      result += htmlEscape(reference);

    lastPos = match.capturedEnd();
  }

  result += htmlEscape(description.mid(lastPos));

  return result;
}

QString textHtml(const QString& text, const QString& color, maki::Styles styles)
{
  return QString("<p style='color: %1; margin: 4px 0; %2'>"
                 "%3"
                 "</p>")
      .arg(color, maki::textStyleCss(styles), text);
}

QString fieldHtml(const QString& title, const QString& field, const QString& titleColor, const QString& fieldColor, maki::Styles styles)
{
  return QString("<p style='margin: 0 0 4px 0;'>"
                 "  <span style='color: %1;'>%2</span>"
                 "  &nbsp;&nbsp;"
                 "  <span style='color: %3; %5'>%4</span>"
                 "</p>")
      .arg(titleColor, title, fieldColor, field, maki::textStyleCss(styles));
}

QString hlineHtml(const QString& foreground, const QString& background, qreal opacity)
{
  const QString color = blendColors(QColor(foreground), QColor(background), opacity).name();
  return QString("<hr style='background-color: %1; border: none; height: 1px;'/>").arg(color);
}

QString spacerHtml(int height)
{
  return QString("<p style='margin: 0; font-size: %1px;'>&nbsp;</p>").arg(height);
}

QString indentHtml(const QString& content, int indentation)
{
  return QString("<table width='100%' cellspacing='0' cellpadding='0' "
                 "style='border: none; margin: 0;'>"
                 "  <tr>"
                 "    <td width='%1' style='border: none;'></td>"
                 "    <td style='border: none;'>%2</td>"
                 "  </tr>"
                 "</table>")
      .arg(indentation)
      .arg(content);
}

QString typeLinkHtml(const QString& typeName, bool clickable)
{
  const QString label = htmlEscape(typeName);
  if (!clickable)
    return label;

  const QString url = QString("%1://%2/").arg(ConfigKeys::MAKI, ConfigKeys::TYPE) + QString::fromLatin1(QUrl::toPercentEncoding(typeName));
  return QString("<a href='%1' title='Open type definition'; font-family: monospace;"
                 "font-weight: 600; text-decoration: none;'>%3</a>")
      .arg(htmlEscape(url), label);
}

QString typeFromLink(const QUrl& url)
{
  if (url.scheme() != ConfigKeys::MAKI || url.host() != ConfigKeys::TYPE)
    return QString();

  QString typeId = url.path();
  if (typeId.startsWith('/'))
    typeId.remove(0, 1);

  return QUrl::fromPercentEncoding(typeId.toUtf8());
}

QString createInformationMessage(const NodeConfig& node)
{
  const auto theme = oclero::qlementine::appStyle()->theme();
  const QString textColor = theme.secondaryColor.name();
  const QString mutedColor = theme.secondaryAlternativeColor.name();
  const QString borderColor = theme.borderColor.name();
  const QString accentColor = theme.primaryColor.name();
  const QString background = theme.backgroundColorMain1.name();

  QString out = QString("<html>"
                        "<body style='font-family: sans-serif; font-size: 10pt; color: %1;'>")
                    .arg(textColor);

  // Title / message
  out += textHtml("Description", textColor, maki::Bold);
  out += hlineHtml(borderColor, background, 0.8);
  out += spacerHtml();
  auto description = fieldHtml("Type", node.type, textColor, accentColor, maki::Bold | maki::Monospace);

  if (!node.help.message.isEmpty())
    description += fieldHtml("Info", formatDescriptionHtml(node.help.message, FlowConfig()), textColor, mutedColor);

  out += indentHtml(description, 16);

  // Properties
  if (!node.properties.isEmpty())
  {
    out += textHtml("Properties", textColor, maki::Bold);
    out += spacerHtml();
    for (const auto& arg : node.properties)
    {
      const QString name = htmlEscape(arg.id);
      const QString argType = htmlEscape(QString::fromStdString(arg.type.toString()));

      QString prop = fieldHtml("Name", name, textColor, textColor, maki::Bold);
      prop += fieldHtml("Type", typeLinkHtml(argType, true), textColor, accentColor, maki::Bold | maki::Monospace);
      if (!arg.info.isEmpty())
        prop += fieldHtml("Info", formatDescriptionHtml(arg.info, FlowConfig()), textColor, mutedColor);

      prop += hlineHtml(borderColor, background, 0.6);
      out += indentHtml(prop, 16);
    }
  }

  // Events
  if (!node.events.isEmpty())
  {
    out += textHtml("Events", textColor, maki::Bold);
    out += spacerHtml();
    for (const auto& e : node.events)
    {
      auto event = createInformationMessage(e, false);
      event += hlineHtml(borderColor, background, 0.6);
      out += indentHtml(event, 16);
    }
  }

  out += "</body></html>";
  return out;
}

QString createInformationMessage(const FlowConfig& event, bool defineBody)
{
  const auto theme = oclero::qlementine::appStyle()->theme();
  const QString textColor = theme.secondaryColor.name();
  const QString mutedColor = theme.secondaryAlternativeColor.name();
  const QString borderColor = theme.borderColor.name();
  const QString accentColor = theme.primaryColor.name();
  const QString background = theme.backgroundColorMain1.name();

  const QString type = htmlEscape(Types::CallTypeToString(event.type));
  const QString returnType = htmlEscape(QString::fromStdString(event.returnType.toString()));

  QString out = "";
  if (defineBody)
    out += QString("<html>"
                   "<body style='font-family: sans-serif; font-size: 10pt; color: %1;'>")
               .arg(textColor);

  out += fieldHtml("Name", event.name, textColor, mutedColor, maki::Bold);
  out += fieldHtml("Type", type, textColor, accentColor, maki::Bold);
  out += fieldHtml("Returns", typeLinkHtml(returnType, true), textColor, mutedColor, maki::Bold | maki::Monospace);

  if (!event.info.isEmpty())
    out += fieldHtml("Info", formatDescriptionHtml(event.info, event), textColor, mutedColor);

  if (!event.arguments.isEmpty())
  {
    out += textHtml("Arguments", textColor, maki::Bold);
    out += spacerHtml(2);
    for (const auto& arg : event.arguments)
    {
      const QString name = htmlEscape(arg.id);
      const QString argType = htmlEscape(QString::fromStdString(arg.type.toString()));

      auto argument = hlineHtml(borderColor, background, 0.4);
      argument += fieldHtml("Name", name, textColor, textColor, maki::Bold);
      argument += fieldHtml("Type", typeLinkHtml(argType, true), textColor, accentColor, maki::Bold | maki::Monospace);
      if (!arg.info.isEmpty())
        argument += fieldHtml("Info", formatDescriptionHtml(arg.info, event), textColor, mutedColor);

      out += indentHtml(argument, 16);
    }
  }

  if (defineBody)
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

void paintSvg(QSvgRenderer* renderer, QPainter* painter, const QPointF& center, qreal width, qreal height)
{
  if (!painter || width <= 0.0)
    return;

  if (!renderer || !renderer->isValid())
    return;

  const QRectF target(center.x() - (width * 0.5), center.y() - (height * 0.5), width, height);
  painter->setRenderHint(QPainter::Antialiasing, true);

  renderer->render(painter, target);
}

QString nameFromNodeId(const QString& id)
{
  auto index = id.indexOf("::");
  if (index < 0)
    return id;

  return id.right(id.size() - (index + 2));
}