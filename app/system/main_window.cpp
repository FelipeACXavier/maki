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
    : MainWindowlayout(parent)
{
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  logging::gLogToStream = [this](struct timespec ts, logging::LogLevel level, const std::string& filename, const uint32_t& line, const std::string& message) {
    if (!mLogText)
      return;

    if (level > mLogLevel)
      return;

    mLogText->append(toQT(ts, level, message));

    // Check if the number of lines exceeds the maximum limit
    QTextDocument* doc = mLogText->document();
    if (doc->blockCount() > 100)
    {
      // Remove the first block (top line) to limit the number of lines
      QTextBlock block = doc->begin();
      mLogText->textCursor().setPosition(block.position());
      mLogText->textCursor().removeSelectedText();
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
  bindShortcuts();

  RETURN_ON_FAILURE(loadElements());

  return VoidResult();

  // Set initial tabs
  mLeftPanel->setCurrentIndex(0);
  mNavigationTab->setCurrentIndex(0);
  mPropertiesTab->setCurrentIndex(0);

  LOG_DEBUG("Main window started");

  return VoidResult();
}

void MainWindow::startUI()
{
  Canvas* canvas = new Canvas(mConfigTable, mCanvasView);
  mCanvasView->setScene(canvas);

  mLeftPanel->setMinimumWidth(200);
  mLeftPanel->setMaximumWidth(300);

  mRightPanel->setMinimumWidth(250);
  mRightPanel->setMaximumWidth(400);

  mPropertiesTab->setMinimumHeight(400);
  mPropertiesTab->setMaximumHeight(800);

  mNavigationTab->setMinimumHeight(200);
  mNavigationTab->setMaximumHeight(800);

  mLeftPanel->setTabIcon(0, addIconWithColor(":/icons/cubes.svg", Qt::white));
  mLeftPanel->setTabToolTip(0, "Structure");

  mLeftPanel->setTabIcon(1, addIconWithColor(":/icons/code-branch.svg", Qt::white));
  mLeftPanel->setTabToolTip(1, "Component behaviour");
  mLeftPanel->tabBar()->setExpanding(true);

  mPluginManager->start(mGeneratorMenu);
}

void MainWindow::bind()
{
  LOG_DEBUG("Binding UI callbacks");

  // File actions =============================================================
  connect(mActionNew, &QAction::triggered, this, &MainWindow::onActionNew);
  mActionNew->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));

  connect(mActionOpen, &QAction::triggered, this, &MainWindow::onActionLoad);
  mActionOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

  connect(mActionSave, &QAction::triggered, this, &MainWindow::onActionSave);
  mActionSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

  connect(mActionSaveAs, &QAction::triggered, this, &MainWindow::onActionSaveAs);
  mActionSaveAs->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

  // Diagram actions =============================================================
  connect(mActionGenerate, &QAction::triggered, this, &MainWindow::onActionGenerate);

  // Setting actions =============================================================
  connect(mActionSetErrorLevel, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Error; });
  connect(mActionSetWarningLevel, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Warning; });
  connect(mActionSetInfoLevel, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Info; });
  connect(mActionSetDebugLevel, &QAction::triggered, [this] { mLogLevel = logging::LogLevel::Debugging; });

  // Internal actions =============================================================
  connect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  connect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  connect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  connect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);

  connect(mSystemMenu, &TreeMenu::nodeRemoved, canvas(), &Canvas::onRemoveNode);
  connect(mSystemMenu, &TreeMenu::nodeSelected, canvas(), &Canvas::onSelectNode);
  connect(mSystemMenu, &TreeMenu::nodeRenamed, canvas(), &Canvas::onRenameNode);
  connect(mSystemMenu, &TreeMenu::nodeFocused, canvas(), &Canvas::onFocusNode);

  // auto behaviourMenu = static_cast<BehaviourMenu*>(mUI->behaviourFrame);
  // behaviourMenu->mGetAvailableNodes = [this]() {
  //   return canvas()->availableNodes();
  // };
}

void MainWindow::bindShortcuts()
{
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this, [this] {
    if (canvas())
      canvas()->copySelectedItems();
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this, [this] {
    if (canvas())
      canvas()->pasteCopiedItems();
  });
  new QShortcut(QKeySequence(Qt::Key_Delete), this, [this] {
    if (canvas())
      canvas()->deleteSelectedItems();
  });
}

Canvas* MainWindow::canvas() const
{
  return static_cast<Canvas*>(mCanvasView->scene());
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

    RETURN_ON_FAILURE(loadLibrary(libConfig));
  }

  return VoidResult();
}

VoidResult MainWindow::loadLibrary(const JSON& config)
{
  if (!config.contains("name"))
    return VoidResult::Failed("Packages must have a name");

  if (!config.contains("libraries"))
    return VoidResult::Failed("Packages must have libraries");

  QString name = config["name"].toString();
  auto libraries = config["libraries"].toArray();
  for (const auto& value : libraries)
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid library format");

    QJsonObject library = value.toObject();
    RETURN_ON_FAILURE(loadElementLibrary(name, library));
  }

  return VoidResult();
}

VoidResult MainWindow::loadElementLibrary(const QString& name, const JSON& config)
{
  if (!config.contains("type"))
    return VoidResult::Failed("Libraries must have a type");

  QString type = config["type"].toString();

  LOG_DEBUG("Loading library: %s", qPrintable(name));

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  QToolBox* toolbox = nullptr;
  if (type == "structure")
    toolbox = mStructureToolBox;
  else
    toolbox = mBehaviourToolBox;

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

void MainWindow::onActionNew()
{
  LOG_DEBUG("Triggering action new");

  // Repopulate the canvas
  SaveInfo emptySave;
  canvas()->loadFromSave(emptySave);
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

void MainWindow::onNodeSelected(NodeItem* node, bool selected)
{
  if (node)
  {
    mInfoText->setText(node->help().message);
    mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    mInfoText->setFont(Fonts::Property);
  }

  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeSelected(node, selected));
  LOG_WARN_ON_FAILURE(mFieldsMenu->onNodeSelected(node, selected));
  // LOG_WARN_ON_FAILURE(mUI->behaviourFrame->onNodeSelected(node, selected));
}

void MainWindow::onNodeAdded(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was added but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeAdded(node));
}

void MainWindow::onNodeRemoved(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeRemoved(node));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeRemoved(node));
  LOG_WARN_ON_FAILURE(mFieldsMenu->onNodeRemoved(node));
  // LOG_WARN_ON_FAILURE(mUI->behaviourFrame->onNodeRemoved(node));
}

void MainWindow::onNodeModified(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeModified(node));
}
