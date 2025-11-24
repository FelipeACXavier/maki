// #include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QMessageBox>
#include <QUrl>

#include "common/app_configs.h"
#include "logging.h"
#include "system/main_window.h"

using namespace Qt::StringLiterals;

void loadApplicationFonts()
{
  QString fontDirPath;

#ifdef Q_OS_WIN
  fontDirPath = QCoreApplication::applicationDirPath() + "/share/fonts";
#else
  fontDirPath = QCoreApplication::applicationDirPath() + "/../share/fonts";
#endif

  QDir fontDir(fontDirPath);
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

  QFile file(QStringLiteral(":/assets/style.css"));
  if (!file.open(QFile::ReadOnly))
  {
    LOG_ERROR("Failed to open stylesheet");
    return -1;
  }

  QApplication::setFont(Fonts::Main);

  MainWindow system;
  system.setStyleSheet(QLatin1String(file.readAll()));

  file.close();

  auto started = system.start();
  if (!started.IsSuccess())
  {
    LOG_ERROR("Failed to start main window: %s", started.ErrorMessage().c_str());
    return -1;
  }

  system.show();

  return app.exec();
}
