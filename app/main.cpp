// #include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QUrl>

#include "common/app_configs.h"
#include "logging.h"
#include "view/main_window.h"

using namespace Qt::StringLiterals;

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName(Savant::Config::ORGANIZATION_NAME);
  QCoreApplication::setApplicationName(Savant::Config::APPLICATION_NAME);
  QCoreApplication::setApplicationVersion(Savant::Config::VERSION);

  // QCommandLineParser parser;
  // parser.setApplicationDescription(
  //     QApplication::translate("main", "A viewer for JSON, PDF and text files"));
  // parser.addHelpOption();
  // parser.addVersionOption();
  // parser.addPositionalArgument("File"_L1, QApplication::translate("main", "JSON, PDF or text file to open"));
  // parser.process(app);

  // const QStringList& positionalArguments = parser.positionalArguments();
  // const QString& fileName = (positionalArguments.count() > 0) ? positionalArguments.at(0) : QString();

  QFile file(QStringLiteral(":/assets/style.css"));
  if (!file.open(QFile::ReadOnly))
  {
    LOG_ERROR("Failed to open stylesheet");
    return -1;
  }

  MainWindow system;
  system.setStyleSheet(QLatin1String(file.readAll()));

  file.close();

  system.start();
  system.show();

  return app.exec();
}
