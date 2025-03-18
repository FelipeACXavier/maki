#include "main_window.h"

#include <QDrag>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidgetItem>
#include <QShortcut>
#include <QString>
#include <QTextBlock>
#include <QWidget>

#include "app_configs.h"
#include "canvas.h"
#include "elements/node.h"
#include "library_container.h"
#include "logging.h"
#include "plugin_manager.h"
#include "save_handler.h"
#include "style_helpers.h"
#include "ui_editor.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
  mUI->setupUi(this);
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  // Bind the logging function
  mUI->logText->setStyleSheet("QTextBrowser { font-family: monospace; }");

  logging::gLogToStream = [this](struct timespec ts, logging::LogLevel level, const std::string& filename, const uint32_t& line, const std::string& message) {
    if (!mUI->logText)
      return;

    if (level > mLogLevel)
      return;

    mUI->logText->append(toQT(ts, level, message));

    // Check if the number of lines exceeds the maximum limit
    QTextDocument* doc = mUI->logText->document();
    if (doc->blockCount() > 100)
    {
      // Remove the first block (top line) to limit the number of lines
      QTextBlock block = doc->begin();
      mUI->logText->textCursor().setPosition(block.position());
      mUI->logText->textCursor().removeSelectedText();
    }
  };

  auto configRead = JSON::fromFile(":/assets/config.json");
  if (!configRead.IsSuccess())
    return VoidResult::Failed(configRead);

  mConfig = configRead.Value();
  mConfigTable = std::make_unique<ConfigurationTable>();

  LOG_DEBUG("Starting the main window");

  mGenerator = std::make_shared<Generator>();
  mSaveHandler = std::make_unique<SaveHandler>(this);
  mPluginManager = std::make_unique<PluginManager>(this);

  startUI();
  bind();

  RETURN_ON_FAILURE(loadElements());

  // Set initial tabs
  mUI->leftPanel->setCurrentIndex(0);
  mUI->helpMenu->setCurrentIndex(0);
  mUI->propertiesMenu->setCurrentIndex(0);

  LOG_DEBUG("Main window started");

  return VoidResult();
}

void MainWindow::startUI()
{
  Canvas* canvas = new Canvas(mConfigTable, mUI->graphicsView);
  mUI->graphicsView->setScene(canvas);

  mUI->splitter->widget(0)->setMinimumWidth(200);
  mUI->splitter->widget(0)->setMaximumWidth(300);

  mUI->splitter->widget(2)->setMinimumWidth(250);
  mUI->splitter->widget(2)->setMaximumWidth(400);

  mUI->propertiesMenu->setMinimumHeight(400);
  mUI->propertiesMenu->setMaximumHeight(800);

  mUI->helpMenu->setMinimumHeight(200);
  mUI->helpMenu->setMaximumHeight(800);

  mUI->leftPanel->setTabIcon(0, addIconWithColor(":/icons/cubes.svg", Qt::white));
  mUI->leftPanel->setTabToolTip(0, "Structure");
  mUI->leftPanel->setTabText(0, "Structure");

  mUI->leftPanel->setTabIcon(1, addIconWithColor(":/icons/vector-square.svg", Qt::white));
  mUI->leftPanel->setTabToolTip(1, "Cross-component behaviour");
  mUI->leftPanel->setTabText(1, "External");

  mUI->leftPanel->setTabIcon(2, addIconWithColor(":/icons/code-branch.svg", Qt::white));
  mUI->leftPanel->setTabToolTip(2, "Component behaviour");
  mUI->leftPanel->setTabText(2, "Internal");
  mUI->leftPanel->tabBar()->setExpanding(true);

  mPluginManager->start(mUI->menuGenerator);
}

void MainWindow::bind()
{
  LOG_DEBUG("Binding UI callbacks");

  // File actions =============================================================
  connect(mUI->actionSave, &QAction::triggered, this,
          &MainWindow::onActionSave);
  mUI->actionSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

  connect(mUI->actionSave_As, &QAction::triggered, this,
          &MainWindow::onActionSaveAs);
  mUI->actionSave_As->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

  connect(mUI->actionOpen, &QAction::triggered, this,
          &MainWindow::onActionLoad);
  mUI->actionOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

  // Diagram actions =============================================================
  connect(mUI->actionGenerate, &QAction::triggered, this,
          &MainWindow::onActionGenerate);

  // Setting actions =============================================================
  connect(mUI->actionError, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Error; });
  connect(mUI->actionWarning, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Warning; });
  connect(mUI->actionInfo, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Info; });
  connect(mUI->actionDebug, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Debugging; });

  // Internal actions =============================================================
  connect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  connect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  connect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  connect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);

  connect(mUI->treeWidget, &TreeMenu::nodeRemoved, canvas(), &Canvas::onRemoveNode);
  connect(mUI->treeWidget, &TreeMenu::nodeSelected, canvas(), &Canvas::onSelectNode);
  connect(mUI->treeWidget, &TreeMenu::nodeRenamed, canvas(), &Canvas::onRenameNode);
  connect(mUI->treeWidget, &TreeMenu::nodeFocused, canvas(), &Canvas::onFocusNode);

  // Shortcuts =============================================================
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this, [this] {
    if (canvas())
      canvas()->copySelectedItems();
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this, [this] {
    if (canvas())
      canvas()->pasteCopiedItems();
  });
}

Canvas* MainWindow::canvas() const
{
  return static_cast<Canvas*>(mUI->graphicsView->scene());
}

VoidResult MainWindow::loadElements()
{
  LOG_DEBUG("Loading the elements");

  if (!mConfig.contains("libraries"))
    return VoidResult::Failed("Not a single library was specified");

  auto libraries = mConfig["libraries"];
  if (!libraries.isArray())
    return VoidResult::Failed("Libraries must be in a list in the format \"libraries\": []");

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

  return VoidResult();
}

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
  QToolBox* toolbox = nullptr;
  if (type == "structure")
    toolbox = mUI->structureToolbox;
  else if (type == "internal behaviour")
    toolbox = mUI->internalBehaviourToolbox;
  else
    toolbox = mUI->externalBehaviourToolbox;

  LibraryContainer* sidebarview = LibraryContainer::create(name, toolbox);

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

    // Parse config and make sure it is valid before continuing
    auto config = std::make_shared<NodeConfig>(node);
    if (!config->isValid())
      return VoidResult::Failed(config->errorMessage.toStdString());

    // Initialize the library type
    if (type == "structure")
      config->libraryType = Types::LibraryTypes::STRUCTURAL;
    else if (type == "internal behaviour")
      config->libraryType = Types::LibraryTypes::INTERNAL_BEHAVIOUR;
    else
      config->libraryType = Types::LibraryTypes::EXTERNAL_BEHAVIOUR;

    auto id = QStringLiteral("%1::%2").arg(name, config->type);
    sidebarview->addNode(id, config);

    RETURN_ON_FAILURE(mConfigTable->add(id, config));
  }

  return VoidResult();
}

void MainWindow::onActionGenerate()
{
  if (!mGenerator)
    LOG_WARNING("No generator available");

  mGenerator->generate(mPluginManager->currentPlugin(), canvas());
}

void MainWindow::onActionSave()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  mSaveHandler->save(canvas());
}

void MainWindow::onActionSaveAs()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  mSaveHandler->saveFileAs(canvas());
}

void MainWindow::onActionLoad()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  auto loaded = mSaveHandler->load();
  if (!loaded.IsSuccess())
  {
    LOG_ERROR(loaded.ErrorMessage());
    return;
  }

  // Repopulate the canvas
  auto info = loaded.Value();
  canvas()->loadFromSave(info);
}

void MainWindow::onNodeSelected(NodeItem* node)
{
  if (node)
  {
    mUI->infoText->setText(node->help().message);
    mUI->infoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    mUI->infoText->setFont(Fonts::Property);
  }

  LOG_WARN_ON_FAILURE(mUI->propertiesFrame->onNodeSelected(node));
  LOG_WARN_ON_FAILURE(mUI->fieldsFrame->onNodeSelected(node));
}

void MainWindow::onNodeAdded(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was added but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mUI->treeWidget->onNodeAdded(node));
}

void MainWindow::onNodeRemoved(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mUI->treeWidget->onNodeRemoved(node));
  LOG_WARN_ON_FAILURE(mUI->propertiesFrame->onNodeRemoved(node));
  LOG_WARN_ON_FAILURE(mUI->fieldsFrame->onNodeRemoved(node));
}

void MainWindow::onNodeModified(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mUI->treeWidget->onNodeModified(node));
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
  if (event->type() == QEvent::KeyPress)
  {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    LOG_INFO("Event detected: %d", keyEvent->key());
    if (keyEvent->key() == Qt::Key_Delete)
    {
      // Forward the event to the QGraphicsView
      if (mUI->graphicsView->hasFocus())
      {
        QCoreApplication::sendEvent(mUI->graphicsView, keyEvent);
        return true;
      }
    }
  }

  return QMainWindow::eventFilter(watched, event);
}
