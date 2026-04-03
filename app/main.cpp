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
#include "app_paths.h"

using namespace Qt::StringLiterals;

void loadApplicationFonts()
{
  auto fontPaths = AppPaths::fonts();
  for (const auto& path : fontPaths)
  {
    QDir fontDir(path);
    const QStringList files = fontDir.entryList({"*.ttf", "*.otf"}, QDir::Files);

    for (const QString& file : files)
    {
      const QString fullPath = fontDir.filePath(file);
      if (QFontDatabase::addApplicationFont(fullPath) == -1)
        LOG_WARNING("Failed to load font %s", qPrintable(fullPath));
    }
  }

  // Uncomment if you need to know what fonts are available
  // QFontDatabase db;
  // for (const QString& family : db.families())
  //   qDebug() << family;
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  app.setApplicationDisplayName(Config::APPLICATION_NAME);
  app.setApplicationName(Config::APPLICATION_NAME);
  app.setApplicationVersion(Config::VERSION);
  app.setOrganizationName(Config::ORGANIZATION_NAME);

  QCoreApplication::setOrganizationName(Config::ORGANIZATION_NAME);
  QCoreApplication::setApplicationVersion(Config::VERSION);
  QCoreApplication::setApplicationName(Config::APPLICATION_NAME);        // internal id-ish
  QGuiApplication::setDesktopFileName(Config::APPLICATION_NAME);         // matches maki.desktop
  QGuiApplication::setApplicationDisplayName(Config::APPLICATION_NAME);  // human name

  app.setWindowIcon(QIcon(":/app_icons/maki.png"));

  QGuiApplication::setDesktopFileName(Config::APPLICATION_NAME);

  loadApplicationFonts();

  QApplication::setFont(Fonts::Main);
  MainWindow system(&app);
  system.setWindowTitle(Config::APPLICATION_NAME);

  auto started = system.start();
  if (!started.IsSuccess())
  {
    LOG_ERROR("Failed to start main window: %s", started.ErrorMessage().c_str());
    return -1;
  }

  system.show();

  return app.exec();
}
