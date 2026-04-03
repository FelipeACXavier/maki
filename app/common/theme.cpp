#include "theme.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStyle>

#include "app_paths.h"
#include "logging.h"
#include "style_helpers.h"

namespace Config
{

maki::ThemeVars SYSTEM_THEME = maki::ThemeVars{};
QColor FOREGROUND = QColor("#000000");
QColor HIGHLIGHT = QColor("#000000");

QString loadFile(const QString& path)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
    return {};

  return QString::fromUtf8(f.readAll());
}

QString applyThemeVars(QString qss, const maki::ThemeVars& t)
{
  for (auto it = Config::THEME_KEY_MAP.begin(); it != Config::THEME_KEY_MAP.end(); ++it)
  {
    const QString& varName = it.key();  // e.g. "@button_bg"

    // Create regex to match the whole variable only
    // (?=[^A-Za-z0-9_]|$) → next char must NOT be a valid identifier char
    QRegularExpression re(QRegularExpression::escape(varName) + "(?=[^A-Za-z0-9_]|$)");
    qss.replace(re, t.*(it.value()));
  }

  return qss;
}

void applyThemeToApp(QApplication* app, const QString& theme, const QList<Config::ThemeInfo>& availableThemes)
{
  if (!app)
    return;

  if (theme == "System theme")
  {
    app->setStyleSheet({});
    app->setPalette(app->style()->standardPalette());
    return;
  }

  const auto it = std::find_if(availableThemes.begin(), availableThemes.end(), [&](const ThemeInfo& t) { return t.id == theme; });
  if (it == availableThemes.end())
  {
    LOG_WARNING("Theme \"%s\"not found, falling back to system", qPrintable(theme));
    app->setStyleSheet({});
    return;
  }

  auto base = loadFile(":/themes/style.qss");
  Config::SYSTEM_THEME = loadThemeVarsFromFile(it->filePath);
  auto foreground = getValueFromTheme("@foreground");
  if (foreground.isValid())
    Config::FOREGROUND = QColor(foreground.toString());
  else
    LOG_WARNING("Failed to get foreground color from theme");

  auto highlight = getValueFromTheme("@highlight");
  if (highlight.isValid())
    Config::HIGHLIGHT = QColor(highlight.toString());
  else
    LOG_WARNING("Failed to get highlight color from theme");

  QString styled = applyThemeVars(base, SYSTEM_THEME);
  // LOG_DEBUG("\n%s", qPrintable(styled));
  app->setStyleSheet(styled);
}

maki::ThemeVars loadThemeVarsFromFile(const QString& filePath)
{
  maki::ThemeVars vars;  // start from defaults

  QFile themeFile(filePath);
  if (!themeFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    LOG_WARNING("Could not open theme file: %s", qPrintable(filePath));
    return vars;
  }

  QTextStream in(&themeFile);
  int lineNumber = 0;

  while (!in.atEnd())
  {
    QString line = in.readLine();
    ++lineNumber;

    line = line.trimmed();
    if (line.isEmpty())
      continue;

    // Ignore comment lines
    if (line.startsWith('#') || line.startsWith("//"))
      continue;

    const int colonIdx = line.indexOf(':');
    if (colonIdx < 0)
    {
      LOG_WARNING("Invalid theme line (no colon) at %d : %s", lineNumber, qPrintable(line));
      continue;
    }

    QString key = line.left(colonIdx).trimmed();
    QString value = line.mid(colonIdx + 1).trimmed();

    if (key.isEmpty() || value.isEmpty())
    {
      LOG_WARNING("Invalid theme line (empty key or value) at %d: %s", lineNumber, qPrintable(line));
      continue;
    }

    auto it = Config::THEME_KEY_MAP.find("@" + key);
    if (it == Config::THEME_KEY_MAP.end())
    {
      LOG_WARNING("Unknown theme key %s at line %d", qPrintable(key), lineNumber);
      continue;
    }

    // Set the corresponding member
    // LOG_DEBUG("Loading %s as value %s", qPrintable(key), qPrintable(value));
    vars.*(it.value()) = value;
  }

  return vars;
}

VoidResult saveThemeVarsToFile(const QString& name, const maki::ThemeVars& theme)
{
  const QString filePath = AppPaths::userThemes() + "/" + name + ".theme";
  QFile themeFile(filePath);
  if (!themeFile.open(QIODevice::WriteOnly | QIODevice::Text))
    return VoidResult::Failed("Could not open theme file: " + filePath.toStdString());

  LOG_DEBUG("Creating theme: %s", qPrintable(filePath));
  QTextStream out(&themeFile);
  for (auto it = Config::THEME_KEY_MAP.cbegin(); it != Config::THEME_KEY_MAP.cend(); ++it)
  {
    QString key = it.key();
    QString value = theme.*(it.value());
    out << key.replace("@", "") << ": " << value << "\n";
  }

  return VoidResult();
}

QList<ThemeInfo> discoverThemes()
{
  QStringList searchDirs = AppPaths::themes();
  for (const auto& path : searchDirs)
    LOG_DEBUG("Looking for themes in: %s", qPrintable(path));

  QMap<QString, ThemeInfo> byId;  // key = id, user themes can override builtin

  for (const QString& dirPath : searchDirs)
  {
    QDir dir(dirPath);
    if (!dir.exists())
      continue;

    const bool isUser = (dirPath == AppPaths::userThemes());
    const QFileInfoList files = dir.entryInfoList(QStringList() << "*.theme", QDir::Files);

    for (const QFileInfo& fi : files)
    {
      ThemeInfo info;
      info.id = fi.baseName();

      // Simple title-case
      info.displayName = info.id;
      info.displayName[0] = info.displayName[0].toUpper();

      info.filePath = fi.absoluteFilePath();
      info.isUser = isUser;

      // user themes override built-ins with same id
      byId.insert(info.id, info);
    }
  }

  return byId.values();
}

QVariant getValueFromTheme(const QString& key)
{
  auto it = Config::THEME_KEY_MAP.constFind(key.trimmed());
  if (it == Config::THEME_KEY_MAP.constEnd())
    return QVariant();

  QString rawValue = SYSTEM_THEME.*(it.value());

  QRegularExpression number("(\\d+)px");
  QRegularExpressionMatch numberMatch = number.match(rawValue);

  if (numberMatch.hasMatch())
    return numberMatch.captured(1).toInt();

  return rawValue;
}

}  // namespace Config
