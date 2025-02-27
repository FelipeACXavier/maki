#include "main_window.h"

#include <QDrag>
#include <QLabel>
#include <QListWidgetItem>
#include <QMimeData>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "app_configs.h"
#include "canvas.h"
#include "logging.h"
#include "ui_editor.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
  mUI->setupUi(this);
  // connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
  // connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onActionAboutTriggered);
  // connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::onActionAboutQtTriggered);
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  LOG_INFO("Starting the main window");

  QFile configFile(QStringLiteral(":/assets/config.json"));
  if (!configFile.open(QFile::ReadOnly))
    return VoidResult::Failed("Failed to open configuration");

  mConfig = nlohmann::json::parse(configFile.readAll().toStdString());

  Canvas* canvas = new Canvas(mUI->graphicsView);
  mUI->graphicsView->setScene(canvas);

  loadElements();

  LOG_INFO("Main window started");

  return VoidResult();
}

VoidResult MainWindow::loadElements()
{
  LOG_INFO("Loading the elements");

  if (!mConfig.contains("libraries"))
    return VoidResult::Failed("Not a single library was specified");

  auto libraries = mConfig["libraries"];
  if (!libraries.is_array())
    return VoidResult::Failed("Libraries must be in a list in the format \"libraries\": []");

  for (const auto& library : libraries)
  {
    QString libraryName = QString::fromStdString(library.template get<std::string>());

    QFile libraryFile(QStringLiteral(":/libraries/%1.json").arg(libraryName));

    if (!libraryFile.open(QFile::ReadOnly))
      return VoidResult::Failed("Failed to open configuration");

    auto libConfig = nlohmann::json::parse(libraryFile.readAll().toStdString());

    RETURN_ON_FAILURE(loadElementLibrary(libConfig));
  }

  return VoidResult();
}

// TODO(felaze): Get this from the configuration file and convert to function
VoidResult MainWindow::loadElementLibrary(const nlohmann::json& config)
{
  if (!config.contains("name"))
    return VoidResult::Failed("Libraries must have a name");

  QString name = QString::fromStdString(config["name"].template get<std::string>());

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  QListWidget* listWidget = new QListWidget();

  QVBoxLayout* listLayout = new QVBoxLayout(listWidget);
  listWidget->setLayout(listLayout);

  mUI->leftPanel->addItem(listWidget, name);

  // Every library has a bunch of elements, here we add them.
  for (int i = 0; i < 5; ++i)
  {
    // Create a custom widget
    QWidget* widget = new QWidget;
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setAlignment(Qt::AlignCenter);

    // TODO(felaze): Pass the element specific configurations
    QPushButton* element = new QPushButton(QStringLiteral("Click Me"), widget);
    connect(element, &QPushButton::pressed, this, &MainWindow::startDrag);

    layout->addWidget(element);
    widget->setLayout(layout);

    QListWidgetItem* item = new QListWidgetItem(listWidget);

    // Set the size hint to ensure proper display
    item->setSizeHint(widget->sizeHint());

    // Add the custom widget to the list
    listWidget->setItemWidget(item, widget);
  }

  return VoidResult();
}

// TODO(felaze): I dont think this is the place for this
void MainWindow::startDrag()
{
  QPushButton* button = qobject_cast<QPushButton*>(sender());
  if (!button)
    return;

  QMimeData* mimeData = new QMimeData();
  mimeData->setData(Constants::TYPE_NODE, QByteArray());

  QDrag* drag = new QDrag(button);
  drag->setMimeData(mimeData);

  // Create a preview pixmap
  QPixmap pixmap(100, 50);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setBrush(Qt::lightGray);
  painter.drawRect(0, 0, 100, 50);
  painter.setBrush(Qt::red);
  painter.drawEllipse(-5, 20, 10, 10);
  painter.setBrush(Qt::green);
  painter.drawEllipse(95, 20, 10, 10);
  painter.end();

  drag->setPixmap(pixmap);
  drag->setHotSpot(QPoint(50, 25));  // Center the preview on the cursor

  drag->exec(Qt::CopyAction);
}
