// language_manager.cpp
#include "language_manager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include "logging.h"
#include "style_helpers.h"
#include "app_paths.h"

LanguageManager::LanguageManager(QObject* parent)
    : QObject(parent)
{
  for (const auto& path : AppPaths::translations())
  {
    LOG_DEBUG("Looking for languages in: %s", qPrintable(path));

    // Ensure user dir exists so users know where to drop files
    QDir dir(path);
    if (!dir.exists())
      return;

    const QFileInfoList files = dir.entryInfoList(QStringList() << "*.qm", QDir::Files);

    for (const QFileInfo& fi : files)
    {
      LanguageManager::LanguageOption option;
      auto name = fi.baseName().remove("maki_");
      option.code = name;
      option.label = name;

      mAvailableLanguages.push_back(option);
    }
  }
}

QList<LanguageManager::LanguageOption> LanguageManager::availableLanguages() const
{
  return mAvailableLanguages;
}

QString LanguageManager::currentLanguage() const
{
  return mCurrentLanguage;
}

QString LanguageManager::qmFileForCode(const QString& code) const
{
  // Example filenames:
  auto paths = AppPaths::translations();
  if (paths.isEmpty())
    return QString();

  // maki_nl_NL.qm, maki_pt_BR.qm, maki_fr_FR.qm
  return paths.at(0) + "/maki_" + code + ".qm";
}

bool LanguageManager::setLanguage(const QString& code)
{
  if (code == mCurrentLanguage)
    return true;

  LOG_DEBUG("Setting language %s, current: %s", qPrintable(code), qPrintable(mCurrentLanguage));
  // English as source language: remove translator and use source strings
  if (code == "en_US")
  {
    qApp->removeTranslator(&mTranslator);
    mCurrentLanguage = code;
    emit languageChanged(code);
    return true;
  }

  const QString qmPath = qmFileForCode(code);
  if (!QFileInfo::exists(qmPath))
    return false;

  qApp->removeTranslator(&mTranslator);

  if (!mTranslator.load(qmPath))
    return false;

  qApp->installTranslator(&mTranslator);
  mCurrentLanguage = code;
  emit languageChanged(code);
  return true;
}
