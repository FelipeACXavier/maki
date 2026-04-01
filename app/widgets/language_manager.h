// language_manager.h
#pragma once

#include <QObject>
#include <QStringList>
#include <QTranslator>

class LanguageManager : public QObject
{
  Q_OBJECT

public:
  explicit LanguageManager(QObject* parent = nullptr);

  struct LanguageOption
  {
    QString code;   // e.g. "en_US"
    QString label;  // e.g. "English"
  };

  QList<LanguageOption> availableLanguages() const;
  QString currentLanguage() const;

  bool setLanguage(const QString& code);

signals:
  void languageChanged(const QString& code);

private:
  QString translationDir() const;
  QString qmFileForCode(const QString& code) const;

private:
  QTranslator mTranslator;
  QString mCurrentLanguage = "en_US";

  QList<LanguageOption> mAvailableLanguages;
};