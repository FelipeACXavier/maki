#include "main_window.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleValidator>
#include <QDrag>
#include <QIntValidator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMimeData>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "app_configs.h"
#include "canvas.h"
#include "elements/node.h"
#include "library_container.h"
#include "logging.h"
#include "string_helpers.h"
#include "style_helpers.h"
#include "ui_editor.h"
#include "widgets/dynamic_control.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
  mUI->setupUi(this);
  connect(mUI->actionGenerate, &QAction::triggered, this,
          &MainWindow::onActionGenerate);
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
  connect(canvas, &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  mUI->graphicsView->setScene(canvas);

  mGenerator = std::make_shared<Generator>();

  RETURN_ON_FAILURE(loadElements());

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
    return VoidResult::Failed(
        "Libraries must be in a list in the format \"libraries\": []");

  mUI->splitter->widget(0)->setMinimumWidth(200);
  mUI->splitter->widget(0)->setMaximumWidth(400);
  // mUI->splitter->widget(0)->setFixedWidth(200);

  mUI->splitter->widget(2)->setMinimumWidth(200);
  mUI->splitter->widget(2)->setMaximumWidth(600);
  // mUI->splitter->widget(2)->setFixedWidth(400);

  mUI->propertiesMenu->setMinimumHeight(400);
  mUI->propertiesMenu->setMaximumHeight(800);

  mUI->helpMenu->setMinimumHeight(200);
  mUI->helpMenu->setMaximumHeight(800);
  // mUI->helpMenu->setFixedHeight(400);

  for (const auto& library : libraries.toArray())
  {
    QString fileName = QStringLiteral(":/libraries/%1.json").arg(library.toString());

    auto libRead = JSON::fromFile(fileName);
    if (!libRead.IsSuccess())
      return VoidResult::Failed(
          QStringLiteral("Failed to open configuration: %1")
              .arg(fileName)
              .toStdString());

    auto libConfig = libRead.Value();

    RETURN_ON_FAILURE(loadElementLibrary(libConfig));
  }

  mUI->leftPanel->setCurrentIndex(0);

  return VoidResult();
}

// TODO(felaze): Get this from the configuration file and convert to function
VoidResult MainWindow::loadElementLibrary(const JSON& config)
{
  if (!config.contains("name"))
    return VoidResult::Failed("Libraries must have a name");

  if (!config.contains("type"))
    return VoidResult::Failed("Libraries must have a type");

  QString name = config["name"].toString();
  QString type = config["type"].toString();

  LOG_DEBUG("Loading library: %s", qPrintable(name));

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  LibraryContainer* sidebarview = LibraryContainer::create(name, type == "behaviour" ? mUI->behaviourToolbox : mUI->structureToolbox);

  auto nodes = config["nodes"];
  if (!nodes.isArray())
    return VoidResult::Failed(
        "nodes must be in a list in the format \"nodes\": []");

  // Every library has a bunch of elements, here we add them.
  for (const auto& value : nodes.toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid node format");

    QJsonObject node = value.toObject();
    if (!node.contains("name"))
      return VoidResult::Failed("Nodes must contain a name");

    // Parse config and make sure it is valid before continuing
    auto config = std::make_shared<NodeConfig>(node);
    if (!config->isValid())
      return config->result();

    auto id = QStringLiteral("%1::%2").arg(name, config->name);
    sidebarview->addNode(id, config);

    RETURN_ON_FAILURE(mConfigTable->add(id, config));
  }

  return VoidResult();
}

void MainWindow::onActionGenerate()
{
  if (!mGenerator)
    LOG_WARNING("No generator available");

  auto canvas = static_cast<Canvas*>(mUI->graphicsView->scene());
  auto ret = mGenerator->generate(canvas);
  if (!ret.IsSuccess())
    LOG_ERROR("Generation failed: %s", ret.ErrorMessage().c_str());
  else
    LOG_INFO("Generation complete");
}

void MainWindow::onNodeSelected(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was selected but no node was provided");
    return;
  }

  mUI->infoText->setText(node->help().message);
  mUI->infoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mUI->infoText->setFont(Fonts::Property);

  LOG_WARN_ON_FAILURE(mUI->propertiesFrame->onNodeSelected(node));
}
