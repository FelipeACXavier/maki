#pragma once

#include <QString>
#include <QStringList>

/**
 * @brief Provides paths for various application components.
 */
class AppPaths
{
public:
  /**
   * @brief Returns the directory where the application is installed.
   * @return QString The application directory path.
   */
  static QString appDir();

  /**
   * @brief Returns the user data directory.
   * @return QString The user data directory path.
   */
  static QString userDataDir();

  /**
   * @brief Returns the user plugin directory.
   * @return QString The user plugin directory path.
   */
  static QString userPluginDir();

  /**
   * @brief Returns the user cache directory.
   * @return QString The user cache directory path.
   */
  static QString userCacheDir();

  /**
   * @brief Returns the user asset directory.
   * @return QString The user asset directory path.
   */
  static QString userAssetDir();

  /**
   * @brief Returns the bundled asset directory.
   * @return QString The bundled asset directory path.
   */
  static QString bundledAssetDir();

  /**
   * @brief Returns the bundled plugin directory.
   * @return QString The bundled plugin directory path.
   */
  static QString bundledPluginDir();

  /**
   * @brief Returns the development asset directory.
   * @return QString The development asset directory path.
   */
  static QString devAssetDir();

  /**
   * @brief Returns the development plugin directory.
   * @return QString The development plugin directory path.
   */
  static QString devPluginDir();

  /**
   * @brief Returns a specific icon based on the provided argument.
   * @param arg The identifier for the icon.
   * @return QString The icon file path.
   */
  static QString icon(const QString& arg);

  /**
   * @brief Returns a list of all available icons.
   * @return QStringList A list of icon file paths.
   */
  static QStringList icons();

  /**
   * @brief Returns a specific translation based on the provided argument.
   * @param arg The identifier for the translation.
   * @return QString The translation file path.
   */
  static QString translation(const QString& arg);

  /**
   * @brief Returns a list of all available translations.
   * @return QStringList A list of translation file paths.
   */
  static QStringList translations();

  /**
   * @brief Returns a specific theme based on the provided argument.
   * @param arg The identifier for the theme.
   * @return QString The theme directory path.
   */
  static QString theme(const QString& arg);

  /**
   * @brief Returns the user themes directory.
   * @return QString The user themes directory path.
   */
  static QString userThemes();

  /**
   * @brief Returns a list of all available themes.
   * @return QStringList A list of theme directory paths.
   */
  static QStringList themes();

  /**
   * @brief Returns a list of all available fonts.
   * @return QStringList A list of font file paths.
   */
  static QStringList fonts();

  /**
   * @brief Returns a list of all available libraries.
   * @return QStringList A list of library file paths.
   */
  static QStringList libraries();

  /**
   * @brief Returns a list of directories to search for assets.
   * @return QStringList A list of asset directory paths.
   */
  static QStringList assetSearchPaths();

  /**
   * @brief Returns a list of directories to search for plugins.
   * @return QStringList A list of plugin directory paths.
   */
  static QStringList pluginSearchPaths();

  /**
   * @brief Finds an asset file based on the relative path.
   * @param relativePath The relative path to the asset file.
   * @return QString The absolute path to the asset file.
   */
  static QString findAsset(const QString& relativePath);

  /**
   * @brief Finds the root directory of a plugin based on its ID or folder name.
   * @param pluginIdOrFolderName The identifier or folder name of the plugin.
   * @return QString The absolute path to the plugin root directory.
   */
  static QString findPluginRoot(const QString& pluginIdOrFolderName);

private:
  /**
   * @brief Returns a list of environment override paths for the given name.
   * @param name The name of the environment variable.
   * @return QStringList A list of environment override paths.
   */
  static QStringList envOverridePaths(const QString& name);
};
