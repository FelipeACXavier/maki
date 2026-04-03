#pragma once

#include <QString>
#include <QStringList>

class AppPaths
{
public:
  static QString appDir();
  static QString userDataDir();
  static QString userPluginDir();
  static QString userCacheDir();
  static QString userAssetDir();

  static QString bundledAssetDir();
  static QString bundledPluginDir();

  static QString devAssetDir();
  static QString devPluginDir();

  static QString icon(const QString& arg);
  static QStringList icons();

  static QString translation(const QString& arg);
  static QStringList translations();

  static QString theme(const QString& arg);
  static QString userThemes();
  static QStringList themes();

  static QStringList fonts();
  static QStringList libraries();

  static QStringList assetSearchPaths();
  static QStringList pluginSearchPaths();

  static QString findAsset(const QString& relativePath);
  static QString findPluginRoot(const QString& pluginIdOrFolderName);

private:
  static QStringList envOverridePaths(const QString& name);
};
