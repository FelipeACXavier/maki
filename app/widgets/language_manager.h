// language_manager.h
#pragma once

#include <QObject>
#include <QStringList>
#include <QTranslator>

/**
 * @class LanguageManager
 * @brief Manages application language switching and translation loading.
 *
 * This class provides functionality to:
 * - Retrieve available languages
 * - Query and change the current language
 * - Load and install Qt translation files (.qm)
 *
 * It emits a signal whenever the active language changes.
 */
class LanguageManager : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a LanguageManager instance.
   * @param parent The parent QObject.
   */
  explicit LanguageManager(QObject* parent = nullptr);

  /**
   * @struct LanguageOption
   * @brief Represents a selectable language option.
   */
  struct LanguageOption
  {
    QString code;   ///< Language code (e.g., "en_US", "pt_BR").
    QString label;  ///< Human-readable label (e.g., "English", "Português (Brasil)").
  };

  /**
   * @brief Returns the list of available languages.
   * @return A list of LanguageOption entries.
   */
  QList<LanguageOption> availableLanguages() const;

  /**
   * @brief Returns the currently active language code.
   * @return The current language code.
   */
  QString currentLanguage() const;

  /**
   * @brief Sets the active language.
   *
   * Loads the corresponding translation file and installs it in the application.
   *
   * @param code The language code to activate (e.g., "en_US").
   * @return True if the language was successfully changed, false otherwise.
   */
  bool setLanguage(const QString& code);

signals:
  /**
   * @brief Emitted when the application language changes.
   * @param code The newly selected language code.
   */
  void languageChanged(const QString& code);

private:
  /**
   * @brief Returns the directory where translation files are stored.
   * @return The absolute or relative path to the translation directory.
   */
  QString translationDir() const;

  /**
   * @brief Returns the full path to the .qm file for a given language code.
   * @param code The language code (e.g., "en_US").
   * @return The path to the corresponding .qm file.
   */
  QString qmFileForCode(const QString& code) const;

private:
  QTranslator mTranslator;             ///< Qt translator used to load and apply translations.
  QString mCurrentLanguage = "en_US";  ///< Currently active language code.

  QList<LanguageOption> mAvailableLanguages;  ///< List of supported languages.

  void loadFromDir(const QString& dir);
};