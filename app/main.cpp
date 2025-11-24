// #include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QMessageBox>
#include <QUrl>

#include "common/app_configs.h"
#include "common/style_helpers.h"
#include "common/theme.h"
#include "logging.h"
#include "system/main_window.h"
#include "widgets/settings_manager.h"

using namespace Qt::StringLiterals;

void loadApplicationFonts()
{
  QDir fontDir(getDirPathFor("share/fonts"));
  const QStringList files = fontDir.entryList({"*.ttf", "*.otf"}, QDir::Files);

  for (const QString& file : files)
  {
    const QString fullPath = fontDir.filePath(file);
    if (QFontDatabase::addApplicationFont(fullPath) == -1)
      LOG_WARNING("Failed to load font %s", qPrintable(fullPath));
  }

  // Uncomment if you need to know what fonts are available
  // QFontDatabase db;
  // for (const QString& family : db.families())
  //   qDebug() << family;
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName(Config::ORGANIZATION_NAME);
  QCoreApplication::setApplicationName(Config::APPLICATION_NAME);
  QCoreApplication::setApplicationVersion(Config::VERSION);

  loadApplicationFonts();

  QApplication::setFont(Fonts::Main);
  MainWindow system;

  auto started = system.start();
  if (!started.IsSuccess())
  {
    LOG_ERROR("Failed to start main window: %s", started.ErrorMessage().c_str());
    return -1;
  }

  auto settingsManager = system.settingsManager();
  if (settingsManager)
  {
    Config::applyThemeToApp(app, settingsManager->appearance().theme, settingsManager->availableThemes());
    QObject::connect(settingsManager.get(), &SettingsManager::themeChanged, &app,
                     [&app](const QString& t, const QList<Config::ThemeInfo>& at) { Config::applyThemeToApp(app, t, at); });
  }

  system.show();

  return app.exec();
}
