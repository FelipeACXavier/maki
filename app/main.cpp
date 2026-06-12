// #include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QMessageBox>
#include <QUrl>
#include <oclero/qlementine.hpp>

#include "app_paths.h"
#include "common/app_configs.h"
#include "common/style_helpers.h"
#include "logging.h"
#include "system/main_window.h"
#include "widgets/settings_manager.h"
#ifdef DEBUG_ADDRESS
#include "system/object_info.h"
#endif

using namespace Qt::StringLiterals;

class MakiApplication : public QApplication
{
public:
  using QApplication::QApplication;
#ifdef DEBUG_ADDRESS
  bool notify(QObject* receiver, QEvent* event) override
  {
    notify_debug(receiver, event);
    return QApplication::notify(receiver, event);
  }
#endif
};

int main(int argc, char* argv[])
{
// TODO: Check how this behaves in different distributions
#ifdef Q_OS_LINUX
  // This makes sure that the system file picker is used
  if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
    qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");
#endif

  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QCoreApplication::setOrganizationName(Config::ORGANIZATION_NAME);
  QCoreApplication::setApplicationVersion(Config::VERSION);
  QCoreApplication::setApplicationName(Config::APPLICATION_NAME);
  QGuiApplication::setDesktopFileName(Config::APPLICATION_NAME);
  QGuiApplication::setApplicationDisplayName(Config::APPLICATION_NAME);
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs, false);

  MakiApplication app(argc, argv);
  app.setApplicationDisplayName(Config::APPLICATION_NAME);
  app.setApplicationName(Config::APPLICATION_NAME);
  app.setApplicationVersion(Config::VERSION);
  app.setOrganizationName(Config::ORGANIZATION_NAME);

  auto* style = new oclero::qlementine::QlementineStyle(&app);
  style->setAnimationsEnabled(true);
  style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
  app.setStyle(style);

  app.setWindowIcon(QIcon(":/app_icons/maki.png"));

  QGuiApplication::setDesktopFileName(Config::APPLICATION_NAME);

  // QApplication::setFont(Fonts::Main);
  auto* themeManager = new oclero::qlementine::ThemeManager(style);

  MainWindow system(&app, themeManager);
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
