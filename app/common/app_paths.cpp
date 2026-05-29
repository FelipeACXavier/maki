#include "app_paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QString AppPaths::appDir()
{
  return QCoreApplication::applicationDirPath();
}

QString AppPaths::userDataDir()
{
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString AppPaths::userPluginDir()
{
  return QDir(userDataDir()).filePath("plugins");
}

QString AppPaths::userAssetDir()
{
  return QDir(userDataDir()).filePath("assets");
}

QString AppPaths::userCacheDir()
{
  return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

QString AppPaths::bundledAssetDir()
{
#if defined(Q_OS_MACOS)
  return QDir(appDir()).filePath("../Resources/assets");
#elif defined(Q_OS_WIN)
  return QDir(appDir()).filePath("assets");
#else
  return QDir(appDir()).filePath("../assets");
#endif
}

QString AppPaths::bundledPluginDir()
{
#if defined(Q_OS_MACOS)
  return QDir(appDir()).filePath("../Resources/plugins");
#elif defined(Q_OS_WIN)
  return QDir(appDir()).filePath("plugins");
#else
  return QDir(appDir()).filePath("../plugins");
#endif
}

QString AppPaths::devPluginDir()
{
  QDir dir(appDir());
  if (dir.cdUp())
  {
    const QString candidate = dir.filePath("plugins");
    if (QFileInfo::exists(candidate))
      return candidate;
  }

  return QString();
}

QStringList AppPaths::assetSearchPaths()
{
  QStringList paths;

  const auto env = envOverridePaths("MAKI_ASSET_PATH");
  if (!env.isEmpty())
    paths << env;

  paths << bundledAssetDir();
  paths << userAssetDir();

  paths.removeDuplicates();
  return paths;
}

QStringList AppPaths::pluginSearchPaths()
{
  QStringList paths;

  const auto env = envOverridePaths("MAKI_PLUGIN_PATH");
  if (!env.isEmpty())
    paths << env;

  paths << bundledPluginDir();
  paths << userPluginDir();

  paths.removeDuplicates();
  return paths;
}

QString AppPaths::findAsset(const QString& relativePath)
{
  for (const QString& root : assetSearchPaths())
  {
    const QString fullPath = QDir(root).filePath(relativePath);
    if (QFileInfo::exists(fullPath))
      return fullPath;
  }
  return QString();
}

QString AppPaths::findPluginRoot(const QString& pluginIdOrFolderName)
{
  for (const QString& root : pluginSearchPaths())
  {
    const QString fullPath = QDir(root).filePath(pluginIdOrFolderName);
    if (QFileInfo::exists(fullPath))
      return fullPath;
  }
  return QString();
}

QStringList AppPaths::envOverridePaths(const QString& name)
{
  const QString raw = QString::fromLocal8Bit(qgetenv(qPrintable(name))).trimmed();
  if (raw.isEmpty())
    return {};

#ifdef Q_OS_WIN
  const QChar sep = ';';
#else
  const QChar sep = ':';
#endif

  QSet<QString> seen;
  QStringList result;

  for (const QString& part : raw.split(sep, Qt::SkipEmptyParts))
  {
    QString cleaned = QDir(part.trimmed()).absolutePath();
    if (!cleaned.isEmpty() && QDir(cleaned).exists() && !seen.contains(cleaned))
    {
      seen.insert(cleaned);
      result << cleaned;
    }
  }

  return result;
}

QString AppPaths::icon(const QString& relativePath)
{
  auto icon = findAsset("icons/" + relativePath);
  if (!icon.isEmpty())
    return icon;

  for (const QString& root : pluginSearchPaths())
  {
    const QString fullPath = QDir(root).filePath(relativePath);
    if (QFileInfo::exists(fullPath))
      return fullPath;
  }

  return QString();
}

QString AppPaths::theme(const QString& arg)
{
  return findAsset("themes/" + arg);
}

QString AppPaths::translation(const QString& arg)
{
  return findAsset("translations/" + arg);
}

QStringList AppPaths::icons()
{
  auto paths = assetSearchPaths();
  for (auto& path : paths)
    path = path + "/icons";

  return paths;
}

QStringList AppPaths::themes()
{
  auto paths = assetSearchPaths();
  for (auto& path : paths)
    path = path + "/themes";

  return paths;
}

QStringList AppPaths::translations()
{
  auto paths = assetSearchPaths();
  for (auto& path : paths)
    path = path + "/translations";

  return paths;
}

QStringList AppPaths::libraries()
{
  auto paths = assetSearchPaths();
  for (auto& path : paths)
    path = path + "/libraries";

  return paths;
}

QStringList AppPaths::fonts()
{
  auto paths = assetSearchPaths();
  for (auto& path : paths)
    path = path + "/fonts";

  return paths;
}

QString AppPaths::userThemes()
{
  return QDir(userAssetDir()).filePath("themes");
}
