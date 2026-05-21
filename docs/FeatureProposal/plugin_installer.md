@page plugin_installer Plugin installer

This class is responsible for installing plugin from different sources.
At the start, we will support installing plugins from github

```cpp
#pragma once

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QPointer>
#include <QTemporaryDir>
#include <QUrl>

class QNetworkReply;

struct PluginManifest
{
  QString id;
  QString name;
  QString version;
  QString makiVersion;
  QString entryPoint;
  QStringList platforms;
  QJsonObject raw;
};

class PluginInstaller : public QObject
{
  Q_OBJECT

public:
  explicit PluginInstaller(QObject* parent = nullptr);

  void installFromUrl(const QUrl& url);

  static QString userPluginRoot();
  static QString platformTag();
  static bool isSupportedUrl(const QUrl& url);

signals:
  void progressMessage(const QString& message);
  void downloadProgress(qint64 received, qint64 total);
  void installSucceeded(const QString& pluginId, const QString& installPath);
  void installFailed(const QString& errorMessage);

private:
  struct GitHubRepoRef
  {
    QString owner;
    QString repo;
  };

  struct ReleaseAsset
  {
    QString name;
    QUrl downloadUrl;
    qint64 size = 0;
  };

  struct ReleaseInfo
  {
    QString tagName;
    QList<ReleaseAsset> assets;
  };

  void fail(const QString& message);
  void emitProgress(const QString& message);

  bool parseGitHubRepoUrl(const QUrl& url, GitHubRepoRef& outRepo) const;
  QUrl buildLatestReleaseApiUrl(const GitHubRepoRef& repo) const;

  void requestLatestRelease(const GitHubRepoRef& repo);
  void onLatestReleaseFinished(QNetworkReply* reply, const GitHubRepoRef& repo);

  bool parseReleaseInfo(const QByteArray& jsonData, ReleaseInfo& outRelease, QString& error) const;
  bool selectBestAsset(const ReleaseInfo& release, ReleaseAsset& outAsset, QString& error) const;

  void downloadAsset(const ReleaseAsset& asset);
  void onAssetDownloadFinished(QNetworkReply* reply, const QString& assetName);

  bool extractArchive(const QString& archivePath, const QString& destinationDir, QString& error) const;

  bool findManifestFile(const QString& rootDir, QString& outManifestPath) const;
  bool loadManifest(const QString& manifestPath, PluginManifest& outManifest, QString& error) const;
  bool validateManifest(const PluginManifest& manifest, const QString& extractedRoot, QString& error) const;

  bool installExtractedPlugin(const QString& extractedRoot, const PluginManifest& manifest, QString& outInstallPath, QString& error) const;
  bool copyDirectoryRecursively(const QString& srcPath, const QString& dstPath, QString& error) const;
  bool removeDirectoryRecursively(const QString& path, QString& error) const;

private:
  QNetworkAccessManager mNetwork;
  std::unique_ptr<QTemporaryDir> mTempDir;
};
```

```cpp
#include "plugin_installer.h"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QStringConverter>

PluginInstaller::PluginInstaller(QObject* parent)
  : QObject(parent)
{
}

void PluginInstaller::installFromUrl(const QUrl& url)
{
  if (!url.isValid() || url.isEmpty())
  {
    fail("Invalid plugin URL.");
    return;
  }

  if (!isSupportedUrl(url))
  {
    fail("Only GitHub repository URLs are currently supported.");
    return;
  }

  GitHubRepoRef repo;
  if (!parseGitHubRepoUrl(url, repo))
  {
    fail("Could not parse GitHub repository URL.");
    return;
  }

  mTempDir = std::make_unique<QTemporaryDir>();
  if (!mTempDir || !mTempDir->isValid())
  {
    fail("Could not create temporary directory.");
    return;
  }

  emitProgress(QString("Fetching latest release metadata for %1/%2...").arg(repo.owner, repo.repo));
  requestLatestRelease(repo);
}

QString PluginInstaller::userPluginRoot()
{
  const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return QDir(base).filePath("plugins");
}

QString PluginInstaller::platformTag()
{
#if defined(Q_OS_WIN)
  return "windows-x64";
#elif defined(Q_OS_MACOS)
  return "macos";
#elif defined(Q_OS_LINUX)
  return "linux-x64";
#else
  return "unknown";
#endif
}

bool PluginInstaller::isSupportedUrl(const QUrl& url)
{
  const QString host = url.host().toLower();
  return host == "github.com" || host == "www.github.com";
}

void PluginInstaller::fail(const QString& message)
{
  emit installFailed(message);
}

void PluginInstaller::emitProgress(const QString& message)
{
  emit progressMessage(message);
}

bool PluginInstaller::parseGitHubRepoUrl(const QUrl& url, GitHubRepoRef& outRepo) const
{
  QStringList parts = url.path().split('/', Qt::SkipEmptyParts);
  if (parts.size() < 2)
    return false;

  outRepo.owner = parts[0];
  outRepo.repo = parts[1];

  if (outRepo.repo.endsWith(".git"))
    outRepo.repo.chop(4);

  return !outRepo.owner.isEmpty() && !outRepo.repo.isEmpty();
}

QUrl PluginInstaller::buildLatestReleaseApiUrl(const GitHubRepoRef& repo) const
{
  return QUrl(QString("https://api.github.com/repos/%1/%2/releases/latest")
                .arg(repo.owner, repo.repo));
}

void PluginInstaller::requestLatestRelease(const GitHubRepoRef& repo)
{
  QNetworkRequest request(buildLatestReleaseApiUrl(repo));
  request.setHeader(QNetworkRequest::UserAgentHeader, "MAKI Plugin Installer");
  request.setRawHeader("Accept", "application/vnd.github+json");

  QNetworkReply* reply = mNetwork.get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply, repo]() {
    onLatestReleaseFinished(reply, repo);
  });
}

void PluginInstaller::onLatestReleaseFinished(QNetworkReply* reply, const GitHubRepoRef&)
{
  reply->deleteLater();

  if (reply->error() != QNetworkReply::NoError)
  {
    fail(QString("Failed to fetch release metadata: %1").arg(reply->errorString()));
    return;
  }

  ReleaseInfo release;
  QString error;
  if (!parseReleaseInfo(reply->readAll(), release, error))
  {
    fail(QString("Failed to parse release metadata: %1").arg(error));
    return;
  }

  ReleaseAsset asset;
  if (!selectBestAsset(release, asset, error))
  {
    fail(QString("No compatible plugin asset found: %1").arg(error));
    return;
  }

  emitProgress(QString("Downloading %1...").arg(asset.name));
  downloadAsset(asset);
}

bool PluginInstaller::parseReleaseInfo(const QByteArray& jsonData, ReleaseInfo& outRelease, QString& error) const
{
  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject())
  {
    error = parseError.errorString();
    return false;
  }

  const QJsonObject root = doc.object();
  outRelease.tagName = root.value("tag_name").toString();

  const QJsonArray assets = root.value("assets").toArray();
  for (const QJsonValue& value : assets)
  {
    const QJsonObject obj = value.toObject();
    ReleaseAsset asset;
    asset.name = obj.value("name").toString();
    asset.downloadUrl = QUrl(obj.value("browser_download_url").toString());
    asset.size = static_cast<qint64>(obj.value("size").toDouble());
    if (!asset.name.isEmpty() && asset.downloadUrl.isValid())
      outRelease.assets.append(asset);
  }

  if (outRelease.assets.isEmpty())
  {
    error = "Latest release has no downloadable assets.";
    return false;
  }

  return true;
}

bool PluginInstaller::selectBestAsset(const ReleaseInfo& release, ReleaseAsset& outAsset, QString& error) const
{
  Q_UNUSED(release);

  const QString currentPlatform = platformTag();

  for (const ReleaseAsset& asset : release.assets)
  {
    const QString name = asset.name.toLower();

#if defined(Q_OS_WIN)
    const bool archiveOk = name.endsWith(".zip");
#elif defined(Q_OS_LINUX)
    const bool archiveOk = name.endsWith(".tar.gz") || name.endsWith(".tgz") || name.endsWith(".zip");
#elif defined(Q_OS_MACOS)
    const bool archiveOk = name.endsWith(".zip") || name.endsWith(".tar.gz") || name.endsWith(".tgz");
#else
    const bool archiveOk = false;
#endif

    if (!archiveOk)
      continue;

    if (name.contains(currentPlatform) || name.contains("universal"))
    {
      outAsset = asset;
      return true;
    }
  }

  error = QString("No release asset matched platform '%1'.").arg(currentPlatform);
  return false;
}

void PluginInstaller::downloadAsset(const ReleaseAsset& asset)
{
  QNetworkRequest request(asset.downloadUrl);
  request.setHeader(QNetworkRequest::UserAgentHeader, "MAKI Plugin Installer");

  QNetworkReply* reply = mNetwork.get(request);

  connect(reply, &QNetworkReply::downloadProgress, this, &PluginInstaller::downloadProgress);
  connect(reply, &QNetworkReply::finished, this, [this, reply, asset]() {
    onAssetDownloadFinished(reply, asset.name);
  });
}

void PluginInstaller::onAssetDownloadFinished(QNetworkReply* reply, const QString& assetName)
{
  reply->deleteLater();

  if (reply->error() != QNetworkReply::NoError)
  {
    fail(QString("Failed to download plugin archive: %1").arg(reply->errorString()));
    return;
  }

  const QString archivePath = QDir(mTempDir->path()).filePath(assetName);
  QFile archiveFile(archivePath);
  if (!archiveFile.open(QIODevice::WriteOnly))
  {
    fail(QString("Could not write temporary archive: %1").arg(archivePath));
    return;
  }

  archiveFile.write(reply->readAll());
  archiveFile.close();

  const QString extractedDir = QDir(mTempDir->path()).filePath("extracted");
  QDir().mkpath(extractedDir);

  emitProgress("Extracting plugin archive...");

  QString error;
  if (!extractArchive(archivePath, extractedDir, error))
  {
    fail(QString("Failed to extract plugin archive: %1").arg(error));
    return;
  }

  QString manifestPath;
  if (!findManifestFile(extractedDir, manifestPath))
  {
    fail("plugin.json was not found inside the plugin package.");
    return;
  }

  PluginManifest manifest;
  if (!loadManifest(manifestPath, manifest, error))
  {
    fail(QString("Failed to read plugin manifest: %1").arg(error));
    return;
  }

  const QString extractedRoot = QFileInfo(manifestPath).absolutePath();
  if (!validateManifest(manifest, extractedRoot, error))
  {
    fail(QString("Plugin validation failed: %1").arg(error));
    return;
  }

  QString installPath;
  if (!installExtractedPlugin(extractedRoot, manifest, installPath, error))
  {
    fail(QString("Plugin installation failed: %1").arg(error));
    return;
  }

  emit installSucceeded(manifest.id, installPath);
}

bool PluginInstaller::extractArchive(const QString& archivePath, const QString& destinationDir, QString& error) const
{
#if defined(Q_OS_WIN)
  QProcess process;
  QStringList args;
  args << "-NoProfile"
       << "-Command"
       << QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force")
             .arg(QDir::toNativeSeparators(archivePath),
                  QDir::toNativeSeparators(destinationDir));
  process.start("powershell", args);
  process.waitForFinished(-1);

  if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
  {
    error = QString::fromUtf8(process.readAllStandardError());
    if (error.trimmed().isEmpty())
      error = QString::fromUtf8(process.readAllStandardOutput());
    return false;
  }

  return true;
#else
  QProcess process;

  if (archivePath.endsWith(".zip"))
  {
    process.start("unzip", {"-o", archivePath, "-d", destinationDir});
  }
  else if (archivePath.endsWith(".tar.gz") || archivePath.endsWith(".tgz"))
  {
    process.start("tar", {"-xzf", archivePath, "-C", destinationDir});
  }
  else
  {
    error = "Unsupported archive format.";
    return false;
  }

  process.waitForFinished(-1);

  if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
  {
    error = QString::fromUtf8(process.readAllStandardError());
    if (error.trimmed().isEmpty())
      error = QString::fromUtf8(process.readAllStandardOutput());
    return false;
  }

  return true;
#endif
}

bool PluginInstaller::findManifestFile(const QString& rootDir, QString& outManifestPath) const
{
  QDirIterator it(rootDir, {"plugin.json"}, QDir::Files, QDirIterator::Subdirectories);
  if (!it.hasNext())
    return false;

  outManifestPath = it.next();
  return true;
}

bool PluginInstaller::loadManifest(const QString& manifestPath, PluginManifest& outManifest, QString& error) const
{
  QFile file(manifestPath);
  if (!file.open(QIODevice::ReadOnly))
  {
    error = "Could not open plugin.json.";
    return false;
  }

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject())
  {
    error = parseError.errorString();
    return false;
  }

  const QJsonObject obj = doc.object();
  outManifest.raw = obj;
  outManifest.id = obj.value("id").toString();
  outManifest.name = obj.value("name").toString();
  outManifest.version = obj.value("version").toString();
  outManifest.makiVersion = obj.value("makiVersion").toString();
  outManifest.entryPoint = obj.value("entryPoint").toString();

  const QJsonArray platforms = obj.value("platforms").toArray();
  for (const QJsonValue& value : platforms)
    outManifest.platforms.append(value.toString());

  return true;
}

bool PluginInstaller::validateManifest(const PluginManifest& manifest, const QString& extractedRoot, QString& error) const
{
  if (manifest.id.trimmed().isEmpty())
  {
    error = "Manifest field 'id' is missing.";
    return false;
  }

  if (manifest.name.trimmed().isEmpty())
  {
    error = "Manifest field 'name' is missing.";
    return false;
  }

  if (manifest.version.trimmed().isEmpty())
  {
    error = "Manifest field 'version' is missing.";
    return false;
  }

  if (manifest.entryPoint.trimmed().isEmpty())
  {
    error = "Manifest field 'entryPoint' is missing.";
    return false;
  }

  if (!manifest.platforms.isEmpty() && !manifest.platforms.contains(platformTag()))
  {
    error = QString("Plugin does not support platform '%1'.").arg(platformTag());
    return false;
  }

  const QString entryPointPath = QDir(extractedRoot).filePath(manifest.entryPoint);
  if (!QFileInfo::exists(entryPointPath))
  {
    error = QString("Plugin entry point not found: %1").arg(manifest.entryPoint);
    return false;
  }

  return true;
}

bool PluginInstaller::installExtractedPlugin(const QString& extractedRoot,
                                             const PluginManifest& manifest,
                                             QString& outInstallPath,
                                             QString& error) const
{
  const QString pluginRoot = userPluginRoot();
  if (!QDir().mkpath(pluginRoot))
  {
    error = QString("Could not create plugin root: %1").arg(pluginRoot);
    return false;
  }

  const QString destination = QDir(pluginRoot).filePath(manifest.id);

  if (QFileInfo::exists(destination))
  {
    QString removeError;
    if (!removeDirectoryRecursively(destination, removeError))
    {
      error = QString("Could not replace existing plugin: %1").arg(removeError);
      return false;
    }
  }

  if (!copyDirectoryRecursively(extractedRoot, destination, error))
    return false;

  outInstallPath = destination;
  return true;
}

bool PluginInstaller::copyDirectoryRecursively(const QString& srcPath, const QString& dstPath, QString& error) const
{
  QDir srcDir(srcPath);
  if (!srcDir.exists())
  {
    error = QString("Source directory does not exist: %1").arg(srcPath);
    return false;
  }

  QDir dstDir;
  if (!dstDir.mkpath(dstPath))
  {
    error = QString("Could not create destination directory: %1").arg(dstPath);
    return false;
  }

  const QFileInfoList entries = srcDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
  for (const QFileInfo& entry : entries)
  {
    const QString srcItem = entry.absoluteFilePath();
    const QString dstItem = QDir(dstPath).filePath(entry.fileName());

    if (entry.isDir())
    {
      if (!copyDirectoryRecursively(srcItem, dstItem, error))
        return false;
    }
    else
    {
      QFile::remove(dstItem);
      if (!QFile::copy(srcItem, dstItem))
      {
        error = QString("Failed to copy file '%1' to '%2'.").arg(srcItem, dstItem);
        return false;
      }
    }
  }

  return true;
}

bool PluginInstaller::removeDirectoryRecursively(const QString& path, QString& error) const
{
  QDir dir(path);
  if (!dir.exists())
    return true;

  if (!dir.removeRecursively())
  {
    error = QString("Failed to remove directory: %1").arg(path);
    return false;
  }

  return true;
}
```

Example:

```cpp
auto* installer = new PluginInstaller(this);

connect(installer, &PluginInstaller::progressMessage, this, [](const QString& msg) {
  qDebug() << "[plugin]" << msg;
});

connect(installer, &PluginInstaller::downloadProgress, this, [](qint64 current, qint64 total) {
  qDebug() << "Download:" << current << "/" << total;
});

connect(installer, &PluginInstaller::installSucceeded, this, [](const QString& pluginId, const QString& path) {
  qDebug() << "Installed plugin" << pluginId << "to" << path;
});

connect(installer, &PluginInstaller::installFailed, this, [](const QString& error) {
  qWarning() << "Plugin install failed:" << error;
});

installer->installFromUrl(QUrl("https://github.com/example/maki-myplugin"));
```
