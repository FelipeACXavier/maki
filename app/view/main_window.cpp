#include "main_window.h"

#include <QDrag>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QListWidgetItem>
#include <QMimeData>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "app_configs.h"
#include "canvas.h"
#include "library_container.h"
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

  auto configRead = JSON::fromFile(":/assets/config.json");
  if (!configRead.IsSuccess())
    return VoidResult::Failed(configRead);

  mConfig = configRead.Value();
  mConfigTable = std::make_unique<ConfigurationTable>();

  Canvas* canvas = new Canvas(mConfigTable, mUI->graphicsView);
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
  if (!libraries.isArray())
    return VoidResult::Failed("Libraries must be in a list in the format \"libraries\": []");

  mUI->splitter->widget(0)->setMinimumWidth(200);
  mUI->splitter->widget(0)->setMaximumWidth(400);
  mUI->splitter->widget(0)->setFixedWidth(200);

  mUI->splitter->widget(2)->setMinimumWidth(200);
  mUI->splitter->widget(2)->setMaximumWidth(600);
  mUI->splitter->widget(2)->setFixedWidth(400);

  for (const auto& library : libraries.toArray())
  {
    QString fileName = QStringLiteral(":/libraries/%1.json").arg(library.toString());

    auto libRead = JSON::fromFile(fileName);
    if (!libRead.IsSuccess())
      return VoidResult::Failed(QStringLiteral("Failed to open configuration: %1").arg(fileName).toStdString());

    auto libConfig = libRead.Value();

    RETURN_ON_FAILURE(loadElementLibrary(libConfig));
  }

  return VoidResult();
}

// TODO(felaze): Get this from the configuration file and convert to function
VoidResult MainWindow::loadElementLibrary(const JSON& config)
{
  if (!config.contains("name"))
    return VoidResult::Failed("Libraries must have a name");

  QString name = config["name"].toString();

  LOG_DEBUG("Loading library: %s", qPrintable(name));

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  LibraryContainer* sidebarview = LibraryContainer::create(name, mUI->leftPanel);

  auto nodes = config["nodes"];
  if (!nodes.isArray())
    return VoidResult::Failed("nodes must be in a list in the format \"nodes\": []");

  // Every library has a bunch of elements, here we add them.
  for (const auto& value : nodes.toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid node format");

    QJsonObject node = value.toObject();
    if (!node.contains("name"))
      return VoidResult::Failed("Nodes must contain a name");

    auto config = std::make_shared<NodeConfig>(node);
    auto id = QStringLiteral("%1::%2").arg(name, config->name);
    sidebarview->addNode(id, config);

    RETURN_ON_FAILURE(mConfigTable->add(id, config));
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
