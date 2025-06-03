#include "main_window.h"

#include <qkeysequence.h>
#include <qnamespace.h>

#include <QComboBox>
#include <QDrag>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QShortcut>
#include <QString>
#include <QTextBlock>
#include <QTextBrowser>
#include <QWidget>

#include "app_configs.h"
#include "behaviour_canvas.h"
#include "canvas.h"
#include "canvas_view.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "library_container.h"
#include "logging.h"
#include "plugin_manager.h"
#include "save_handler.h"
#include "structure_canvas.h"
#include "style_helpers.h"
#include "widgets/properties/fields_menu.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/system_menu.h"

MainWindow::MainWindow(QWidget* parent)
    : MainWindowlayout(parent)
    , mActiveCanvas(nullptr)
{
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  mLogLevel = logging::LogLevel::Debugging;

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
  mConfigTable = std::make_shared<ConfigurationTable>();
  mStorage = std::make_shared<SaveInfo>();

  LOG_DEBUG("Starting the main window");

  mGenerator = std::make_shared<Generator>(mStorage);
  mSaveHandler = std::make_unique<SaveHandler>(this);
  mPluginManager = std::make_unique<PluginManager>();

  startUI();
  bind();
  bindShortcuts();

  mPropertiesMenu->start(mStorage);

  RETURN_ON_FAILURE(loadElements());

  // Set initial tabs
  mLeftPanel->setCurrentIndex(0);
  mNavigationTab->setCurrentIndex(0);
  mPropertiesTab->setCurrentIndex(0);

  LOG_DEBUG("Main window started");

  return VoidResult();
}

void MainWindow::startUI()
{
  CanvasView* currentCanvas = static_cast<CanvasView*>(mCanvasPanel->currentWidget());
  // TODO(felaze): Shouldn't be hard-coded
  StructureCanvas* canvas = new StructureCanvas("MainSystemCanvas", mStorage, mConfigTable, currentCanvas);
  mActiveCanvas = canvas;
  currentCanvas->setScene(canvas);

  // Make sure the UI matches the internal state
  mLogLevelComboBox->setCurrentIndex(static_cast<int>(mLogLevel));

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
  mActionGenerate->setShortcut(QKeySequence(Qt::Key_F5));

  // Setting actions =============================================================
  connect(mLogLevelComboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
    mLogLevel = static_cast<logging::LogLevel>(index);
  });

  // Internal actions =============================================================
  connect(mCanvasPanel, &QTabWidget::currentChanged, this, &MainWindow::onCanvasTabChanged);
  connect(mCanvasPanel, &QTabWidget::tabCloseRequested, this, &MainWindow::closeCanvasTab);

  connect(rootCanvas(), &Canvas::openFlow, this, &MainWindow::onOpenFlow);
  connect(rootCanvas(), &Canvas::flowAdded, this, &MainWindow::onFlowAdded);
  connect(rootCanvas(), &Canvas::flowRemoved, this, &MainWindow::onFlowRemoved);
  // connect(rootCanvas(), &Canvas::createEvent, mFieldsMenu, &FieldsMenu::onCreateEvent);

  // connect(mFieldsMenu, &FieldsMenu::flowSelected, rootCanvas(), &Canvas::onFlowSelected);
  connect(mPropertiesMenu, &PropertiesMenu::flowSelected, rootCanvas(), &Canvas::onFlowSelected);

  connect(mFlowMenu, &FlowMenu::flowSelected, rootCanvas(), &Canvas::onFlowSelected);
  connect(mFlowMenu, &FlowMenu::flowRemoved, rootCanvas(), &Canvas::onFlowRemoved);

  bindCanvas();
}

void MainWindow::bindCanvas()
{
  connect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  connect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  connect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  connect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);

  connect(canvas(), &Canvas::createEvent, mPropertiesMenu, &PropertiesMenu::onCreateEvent);
  connect(canvas(), &Canvas::transitionSelected, mPropertiesMenu, &PropertiesMenu::onTransitionSelected);

  connect(mSystemMenu, &SystemMenu::nodeRemoved, canvas(), &Canvas::onRemoveNode);
  connect(mSystemMenu, &SystemMenu::nodeSelected, canvas(), &Canvas::onSelectNode);
  connect(mSystemMenu, &SystemMenu::nodeRenamed, canvas(), &Canvas::onRenameNode);
  connect(mSystemMenu, &SystemMenu::nodeFocused, canvas(), &Canvas::onFocusNode);

}

void MainWindow::unbindCanvas()
{
  disconnect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  disconnect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  disconnect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  disconnect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);
  disconnect(canvas(), &Canvas::createEvent, mPropertiesMenu, &PropertiesMenu::onCreateEvent);

  disconnect(mSystemMenu, &SystemMenu::nodeRemoved, canvas(), &Canvas::onRemoveNode);
  disconnect(mSystemMenu, &SystemMenu::nodeSelected, canvas(), &Canvas::onSelectNode);
  disconnect(mSystemMenu, &SystemMenu::nodeRenamed, canvas(), &Canvas::onRenameNode);
  disconnect(mSystemMenu, &SystemMenu::nodeFocused, canvas(), &Canvas::onFocusNode);
  
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
  return mActiveCanvas;
}

Canvas* MainWindow::rootCanvas() const
{
  auto view = static_cast<CanvasView*>(mCanvasPanel->widget(0));
  return static_cast<Canvas*>(view->scene());
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
    else
      config->libraryType = Types::LibraryTypes::BEHAVIOUR;

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

  mGenerator->generate(mPluginManager->currentPlugin());
}

void MainWindow::onActionSave()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  // QJsonDocument doc(mStorage->toJson());
  // QByteArray jsonBytes = doc.toJson(QJsonDocument::Indented);
  // qDebug().noquote() << jsonBytes;

  mSaveHandler->save(rootCanvas());
}

void MainWindow::onActionSaveAs()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  mSaveHandler->saveFileAs(rootCanvas());
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

  // Close all tabs except the first
  for (int i = 1; i < mCanvasPanel->count(); ++i)
    closeCanvasTab(i);

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
  // LOG_WARN_ON_FAILURE(mFieldsMenu->onNodeSelected(node, selected));
}

void MainWindow::onNodeAdded(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was added but no node was provided");
    return;
  }

  if (canvas()->type() == Types::LibraryTypes::STRUCTURAL)
  {
    LOG_WARN_ON_FAILURE(mSystemMenu->onNodeAdded(node));
    LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeAdded(node));
    // LOG_WARN_ON_FAILURE(mFieldsMenu->onNodeAdded(node));
  }
  else
  {
    LOG_WARN_ON_FAILURE(mFlowMenu->onNodeAdded(canvas()->id(), node));
  }
}

void MainWindow::onNodeRemoved(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeRemoved(node));
  // LOG_WARN_ON_FAILURE(mFieldsMenu->onNodeRemoved(node));

  if (canvas()->type() == Types::LibraryTypes::STRUCTURAL)
    LOG_WARN_ON_FAILURE(mSystemMenu->onNodeRemoved(node));
  else
    LOG_WARN_ON_FAILURE(mFlowMenu->onNodeRemoved(canvas()->id(), node));
}

void MainWindow::onNodeModified(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  if (canvas()->type() == Types::LibraryTypes::STRUCTURAL)
    LOG_WARN_ON_FAILURE(mSystemMenu->onNodeModified(node));
  else
    LOG_WARN_ON_FAILURE(mFlowMenu->onNodeModified(canvas()->id(), node));
}

void MainWindow::onCanvasTabChanged(int index)
{
  CanvasView* newCanvas = static_cast<CanvasView*>(mCanvasPanel->widget(index));
  if (!newCanvas)
    return;

  // Disconnect signals from the previous canvas
  if (mActiveCanvas)
    unbindCanvas();

  mActiveCanvas = static_cast<Canvas*>(newCanvas->scene());
  bindCanvas();

  auto libIndex = libraryTypeToIndex(mActiveCanvas->type());
  mNavigationTab->setCurrentIndex(libIndex);
  mLeftPanel->setCurrentIndex(libIndex);
}

void MainWindow::closeCanvasTab(int index)
{
  CanvasView* newCanvas = static_cast<CanvasView*>(mCanvasPanel->widget(index));
  if (!newCanvas)
    return;

  if (newCanvas->scene() == mActiveCanvas)
  {
    unbindCanvas();

    if (index > 0)
    {
      // The previous tab becomes the new active tab
      newCanvas = static_cast<CanvasView*>(mCanvasPanel->widget(index - 1));
      mActiveCanvas = static_cast<Canvas*>(newCanvas->scene());
      bindCanvas();

      auto libIndex = libraryTypeToIndex(mActiveCanvas->type());
      mNavigationTab->setCurrentIndex(libIndex);
      mLeftPanel->setCurrentIndex(libIndex);
    }
  }

  mCanvasPanel->removeTab(index);
}

void MainWindow::onOpenFlow(Flow* flow, NodeItem* node)
{
  QString flowName;
  if (flow == nullptr)
  {
    QInputDialog* dialog = new QInputDialog(this);
    dialog->setWindowTitle(tr("Flow name"));
    dialog->setLabelText(tr("Enter a name for the new flow:"));
    dialog->setTextValue(tr(""));

    // Set a validator: allow only alphanumerics and spaces
    QRegularExpression rx("[A-Za-z ]+");
    QValidator* validator = new QRegularExpressionValidator(rx, dialog);

    // Access the line edit and assign the validator
    QLineEdit* lineEdit = dialog->findChild<QLineEdit*>();
    if (lineEdit)
      lineEdit->setValidator(validator);

    // Execute the dialog
    if (dialog->exec() != QDialog::Accepted)
      return;

    flowName = dialog->textValue().trimmed();
  }
  else
  {
    flowName = flow->name();
  }

  if (flowName.isEmpty())
  {
    LOG_INFO("No name provided, skipping flow creation");
    return;
  }

  for (int i = 1; i < mCanvasPanel->count(); ++i)
  {
    if (mCanvasPanel->tabText(i) == flowName)
    {
      mCanvasPanel->setCurrentIndex(i);
      return;
    }
  }

  if (flow == nullptr)
  {
    LOG_WARNING("This shouldn't happen");
    return;
  }

  CanvasView* newView = new CanvasView();

  BehaviourCanvas* canvas = new BehaviourCanvas(flow, mStorage, mConfigTable, newView);
  newView->setScene(canvas);

  // Change to respective tabs
  auto index = libraryTypeToIndex(canvas->type());
  mNavigationTab->setCurrentIndex(index);
  mLeftPanel->setCurrentIndex(index);

  // Add default start and end nodes to flow
  canvas->populate(flow);

  mCanvasPanel->addTab(newView, flowName);
  mCanvasPanel->setCurrentWidget(newView);
}

void MainWindow::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  if (mActiveCanvas->id() == flowId)
  {
    int oldTab = mCanvasPanel->currentIndex();
    mCanvasPanel->setCurrentIndex(oldTab - 1);
    mCanvasPanel->removeTab(oldTab);
  }

  LOG_WARN_ON_FAILURE(mFlowMenu->onFlowRemoved(flowId, node));
}

void MainWindow::onFlowAdded(Flow* flow, NodeItem* node)
{
  LOG_WARN_ON_FAILURE(mFlowMenu->onFlowAdded(flow, node));
}

int MainWindow::libraryTypeToIndex(Types::LibraryTypes type) const
{
  switch (type)
  {
    case Types::LibraryTypes::STRUCTURAL:
      return 0;
    case Types::LibraryTypes::BEHAVIOUR:
      return 1;
    default:
      LOG_ERROR("Unknown library type");
      return 0;
  }
}
