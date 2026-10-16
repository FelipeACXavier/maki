#include "main_window.h"

#include <QComboBox>
#include <QDesktopServices>
#include <QDrag>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QStackedWidget>
#include <QString>
#include <QTextBlock>
#include <QTextBrowser>
#include <QUndoGroup>
#include <QUrl>
#include <QWidget>
#include <memory>
#include <oclero/qlementine/widgets/AboutDialog.hpp>
#include <oclero/qlementine/widgets/AbstractItemListWidget.hpp>

#include "app_configs.h"
#include "app_paths.h"
#include "behaviour_canvas.h"
#include "canvas.h"
#include "canvas_info.h"
#include "canvas_view.h"
#include "compiler/pipeline.h"
#include "compiler/pipeline_graph.h"
#include "compiler/plugin_action_registry.h"
#include "compiler/plugin_pipeline.h"
#include "config.h"
#include "config_table.h"
#include "edge_router.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "flow_info.h"
#include "host_services.h"
#include "keys.h"
#include "library_container.h"
#include "library_scene.h"
#include "logger.h"
#include "logging.h"
#include "macros.h"
#include "notifications.h"
#include "pipeline_canvas.h"
#include "plugin_manager.h"
#include "plugin_tab.h"
#include "plugin_view.h"
#include "process_tab.h"
#include "result.h"
#include "save_handler.h"
#include "structure_canvas.h"
#include "style_helpers.h"
#include "system/main_window_layout.h"
#include "tutorials/tour_guide.h"
#include "type_registry.h"
#include "types.h"
#include "widgets/controls/capability_call_selector.h"
#include "widgets/dialogs/prompt.h"
#include "widgets/dialogs/text_prompt.h"
#include "widgets/editor/editor_tab_group.h"
#include "widgets/editor/editor_workspace.h"
#include "widgets/language_manager.h"
#include "widgets/log_table_widget.h"
#include "widgets/notification_manager.h"
#include "widgets/properties/mission_parameter_widget.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"
#include "widgets/settings_dialog.h"
#include "widgets/settings_manager.h"
#include "widgets/structure/file_menu.h"
#include "widgets/structure/system_menu.h"
#include "widgets/type_editor.h"

static QWidget* findAncestor(QWidget* w, const QMetaObject* type)
{
  while (w)
  {
    // LOG_TRACE("Finding: {} vs {}", type->className(), w->metaObject(->className()));
    if (w->metaObject()->inherits(type))
      return w;
    w = w->parentWidget();
  }
  return nullptr;
}

#define IN_TYPE(OBJ, TYPE) qobject_cast<TYPE*>(findAncestor(OBJ, &TYPE::staticMetaObject))

MainWindow::MainWindow(QApplication* app, oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
    : MainWindowLayout(parent)
    , mActiveCanvas(nullptr)
    , mThemeManager(themeManager)
    , mApp(app)
{
}

MainWindow::~MainWindow()
{
  if (mNotificationManager)
  {
    delete mNotificationManager;
    mNotificationManager = nullptr;
  }
}

VoidResult MainWindow::start()
{
  logging::gSilentLog = false;
  logging::gMinLogLevel = logging::LogLevel::Debugging;
  logging::gSourceName = Config::APPLICATION_NAME.toStdString();
  logging::gLogToStream = [this](std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& source, const std::string& filename,
                                 const uint32_t& line, const std::string& message) {
    if (mLogTable)
      mLogTable->append(level, source, filename, line, message);
  };

  notification::gNotificationStream = [this](logging::LogLevel level, const std::string& header, const std::string& message) {
    if (mNotificationManager)
      mNotificationManager->showNotification(QString::fromStdString(header), QString::fromStdString(message), level);
  };
  notification::gLongNotificationStream = [this](const QString& id, logging::LogLevel level, const std::string& header, QWidget* body) {
    if (mNotificationManager)
      return mNotificationManager->showLongNotification(id, QString::fromStdString(header), body, level);
    return QString();
  };

  for (const auto& t : AppPaths::themes())
    mThemeManager->loadDirectory(t);

  // mStorage = std::make_shared<SaveInfo>();

  LOG_DEBUG("Starting the main window");

  LOG_INFO("Using application path: {}", QCoreApplication::applicationDirPath());
  mSaveHandler = std::make_unique<SaveHandler>(this);
  mSettingsManager = std::make_shared<SettingsManager>(mThemeManager, this);
  mLanguageManager = std::make_shared<LanguageManager>(this);
  mRouter = std::make_shared<EdgeRouter>(this);

  auto processPipeline = new Pipeline(this);
  mPipeline = new Pipeline(this);
  mPluginPipeline = new maki::PluginPipeline(processPipeline, this);
  mLogger = new Logger(this);
  mNotificationManager = new NotificationManager(mEditorWorkspace, this);
  mPluginManager = std::make_unique<PluginManager>(mPluginPipeline->registry(), mPipeline, this);

  mPluginTab = new PluginTab(mSpecialTabsMenu, this);

  mMissionParameters->documentUpdated();

  mHostServices = new HostServices(mPipeline, mSettingsManager.get(), QCoreApplication::applicationDirPath(), this);
  mHostServices->setPluginTab(mPluginTab);
  mHostServices->setLogger(mLogger);

  mProcessTab->addPipeline(mPipeline);
  mProcessTab->addPipeline(processPipeline);

  startUI();
  bind();
  bindShortcuts();

  // mPropertiesMenu->start(mStorage);

  if (mPipelineRun)
    for (const auto& pipeline : SaveInfo::instance().pipelines())
    {
      mPipelineRun->addOption(pipeline->getname(), pipeline->getid());
      if (mSystemMenu)
        mSystemMenu->onPipelineAdded(pipeline);
    }

  // Set initial tabs
  mPalette->setCurrentIndex(0);
  mNavigationTab->setCurrentIndex(0);
  mPropertiesTab->setCurrentIndex(0);

  // Load elements after the system started
  QTimer::singleShot(0, this, [this]() {
    if (mPluginManager && mSettingsManager)
      LOG_WARN_ON_FAILURE(mPluginManager->start(mSettingsManager->plugins(), mHostServices));

    onSettingsChanged(true);

    LOG_WARN_ON_FAILURE(loadElements());
    LOG_WARN_ON_FAILURE(loadLastSession());

    if (!DEFAULT_TEMPLATE.isEmpty())
      onActionNew(DEFAULT_TEMPLATE);

    startTourGuide(true);
  });

  LOG_DEBUG("Main window started");

  return VoidResult();
}

std::pair<LibraryContainer*, DraggableItem*> MainWindow::findDraggableInPalette(const QString& nodeType) const
{
  for (const auto* section : mPalette->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      if (auto palette = qobject_cast<LibraryScene*>(library->scene()))
        for (const auto& item : palette->items())
          if (auto draggable = qgraphicsitem_cast<DraggableItem*>(item); draggable->nodeId() == nodeType)
            return std::make_pair(library, draggable);

  return std::make_pair(nullptr, nullptr);
}

void MainWindow::startTourGuide(bool runningOnStartUp)
{
  mTourGuide = std::make_shared<TourGuide>(this, this);
  if (runningOnStartUp && mSettingsManager && !mSettingsManager->general().showTourOnStart)
    return;

  auto [taskLibrary, taskNode] = findDraggableInPalette(ConfigKeys::TASK_NODE);
  if (!taskNode || !taskLibrary)
  {
    LOG_WARNING("Could not find task node in palette");
    return;
  }

  auto [callLibrary, callNode] = findDraggableInPalette(ConfigKeys::CAPABILITY_CALL_NODE);
  if (!callNode || !callLibrary)
  {
    LOG_WARNING("Could not find capability call node in palette");
    return;
  }

  auto [capabilityLibrary, capabilityNode] = findDraggableInPalette("Koda::Siren");
  if (!capabilityNode || !capabilityLibrary)
  {
    LOG_WARNING("Could not find capability call node in palette");
    return;
  }

  struct Steps
  {
    int showWelcome = TourStep::Invalid;
    int showPalette = TourStep::Invalid;
    int startDragTask = TourStep::Invalid;
    int endDragTask = TourStep::Invalid;
    int highlightTask = TourStep::Invalid;
    int showProperties = TourStep::Invalid;
    int updateName = TourStep::Invalid;
    int highlightTaskName = TourStep::Invalid;
    int showCapabilities = TourStep::Invalid;
    int startDragCapability = TourStep::Invalid;
    int endDragCapability = TourStep::Invalid;
    int highlightCapability = TourStep::Invalid;
    int preControlHover = TourStep::Invalid;
    int highlightControl = TourStep::Invalid;
    int showBehaviourCanvas = TourStep::Invalid;
    int showBehaviourPalette = TourStep::Invalid;
    int startDragCall = TourStep::Invalid;
    int endDragCall = TourStep::Invalid;
    int highlightCall = TourStep::Invalid;
    int selectCapability = TourStep::Invalid;
    int selectCapabilityEvent = TourStep::Invalid;
    int confirmCapability = TourStep::Invalid;
    int completeCall = TourStep::Invalid;
    int startVerification = TourStep::Invalid;
    int verifying = TourStep::Invalid;
    int verificationDone = TourStep::Invalid;
    int end = TourStep::Invalid;

    QString taskId = "";
  };

  auto steps = std::make_shared<Steps>();
  auto canvasView = qobject_cast<CanvasView*>(mEditorWorkspace->activeEditor());
  if (!canvasView)
  {
    LOG_DEBUG("Failed to focus on active editor");
    return;
  }

  auto structureCanvas = qobject_cast<Canvas*>(canvasView->scene());

  steps->showWelcome = mTourGuide->addStep(nullptr, tr("Welcome to %1").arg(Config::APPLICATION_NAME),
                                           tr("This quick tour shows you how to create a simple application with %1.").arg(Config::APPLICATION_NAME));
  steps->showPalette = mTourGuide->addStep(
    mPalette, tr("Structure Tab"),
    tr("%1 applications revolved around nodes. Structural nodes define the structure of the application. They can be found in the Structure Tab.")
      .arg(Config::APPLICATION_NAME));
  steps->startDragTask = mTourGuide->addItemStep(taskLibrary, taskNode, tr("Task node"), tr("Click and drag the Task node onto the Canvas"),
                                                 TourStep::AdvanceOn::UserAction, steps->showPalette);
  steps->endDragTask = mTourGuide->addStep(canvasView, tr("Drop it on the Canvas"), tr("Release the Task node anywhere on the Canvas."),
                                           TourStep::AdvanceOn::UserAction, steps->startDragTask);

  steps->showProperties = mTourGuide->addStep(
    mPropertiesTab, tr("Properties"),
    tr("Nodes may have properties that are used to modify how they behave. In the \"Property Tab\" you can find the properties of the dropped node"));

  steps->showCapabilities = mTourGuide->addStep(
    capabilityLibrary, tr("Capabilities"),
    tr("Capabilities define the robot functions available to a Task. Capabilities are Structural nodes and can be found in the Structure Tab."));
  steps->startDragCapability = mTourGuide->addItemStep(
    capabilityLibrary, capabilityNode, tr("%1 Capability").arg(tour::QUICK_TOUR_CAPABILTY),
    tr("To add a Capability to a Task. Click on the %1 Capablity and drag in onto a Task node in the Canvas.").arg(tour::QUICK_TOUR_CAPABILTY),
    TourStep::AdvanceOn::UserAction, steps->showCapabilities);

  steps->end = mTourGuide->addStep(
    nullptr, tr("Congratulations!"),
    tr("You should be ready to start creating your application. For more information, explore the other available nodes and the help menu in the toolbar."));

  connect(mPropertiesMenu, &PropertiesMenu::nodeFocused, this, [this, steps](const QString& nodeId) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->highlightTask)
      return;

    if (steps->taskId != nodeId)
      return;

    auto lineEdits = mPropertiesMenu->findChildren<QLineEdit*>();
    if (auto lineEdit = lineEdits.isEmpty() ? nullptr : lineEdits.front(); lineEdit)
    {
      steps->updateName = mTourGuide->insertStep(steps->highlightTaskName, lineEdit, tr("Update properties"),
                                                 tr("We need to give our Task a name. Write \"%1\" as the Task name.").arg(tour::QUICK_TOUR_NAME),
                                                 TourStep::AdvanceOn::UserAction, steps->highlightTask, steps->updateName);
    }
  });
  connect(mPropertiesMenu, &PropertiesMenu::propertyUpdated, this, [this, steps](const QString& parameterId, const QString& nodeId, const maki::Value& value) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->updateName)
      return;

    if (steps->taskId != nodeId)
      return;

    if (parameterId == ConfigKeys::NAME && value.isString() && value.toString() == tour::QUICK_TOUR_NAME)
      mTourGuide->advance();
  });
  connect(mNotificationManager, &NotificationManager::showingNotification, this, [this, steps](NotificationWidget* toast) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->startVerification)
      return;

    if (!toast || toast->disappearing())
      return;

    steps->verifying = mTourGuide->insertStep(steps->end, toast, tr("Verification progress"),
                                              tr("You can monitor the verification progress here. Depending on the size of your "
                                                 "application, the verification may take some time. Let's wait a bit until it is done."),
                                              TourStep::AdvanceOn::UserAction, TourStep::Invalid, steps->verifying);
    steps->verificationDone = mTourGuide->insertStep(steps->end, toast, tr("Verification done"),
                                                     tr("And there we go! Our program has being verified and no errors were found!"),
                                                     TourStep::AdvanceOn::Button, TourStep::Invalid, steps->verificationDone);

    mTourGuide->advance();
  });
  connect(structureCanvas, &Canvas::nodeAdded, this, [this, canvasView, steps](Canvas* /* canvas */, NodeItem* node) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->endDragTask)
      return;

    steps->taskId = node->id();

    steps->highlightTask = mTourGuide->insertItemStep(steps->showProperties, canvasView, node, tr("Task node"),
                                                      tr("Nice! Now we have a Task node on the Canvas."), TourStep::AdvanceOn::Button, TourStep::Invalid,
                                                      steps->highlightTask);

    steps->highlightTaskName = mTourGuide->insertItemStep(steps->showCapabilities, canvasView, node, tr("Name updated"),
                                                          tr("Good job! You can see that the node label was also updated."), TourStep::AdvanceOn::Button,
                                                          TourStep::Invalid, steps->highlightTaskName);

    steps->endDragCapability = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Drop Capability"),
                                                          tr("Drop the %1 Capability on the Task node.").arg(tour::QUICK_TOUR_CAPABILTY),
                                                          TourStep::AdvanceOn::UserAction, steps->startDragCapability, steps->endDragCapability);

    steps->highlightCapability = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Task with Capability"),
                                                            tr("Great! Now, our Task is able to access the %1 Capability.").arg(tour::QUICK_TOUR_CAPABILTY),
                                                            TourStep::AdvanceOn::Button, TourStep::Invalid, steps->highlightCapability);

    steps->preControlHover = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Modifying behavior"),
                                                        tr("Move the mouse over the Task node to see what else we can do."), TourStep::AdvanceOn::UserAction,
                                                        steps->highlightCapability, steps->preControlHover);
  });
  connect(structureCanvas, &Canvas::controlCreated, this, [this, steps](maki::ControlWidget* control) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->preControlHover)
      return;

    control->setTouring();

    steps->highlightControl = mTourGuide->insertStep(
      steps->end, control, tr("Task node menu"),
      tr("This is the task node menu. Here, you can open the main flow or add new Flows to Tasks. Click on \"Open main flow\" to start updating it."),
      TourStep::AdvanceOn::UserAction, TourStep::Invalid, steps->highlightControl);

    mTourGuide->advance();
  });
  connect(
    structureCanvas, &Canvas::openFlow, this,
    [this, callLibrary, callNode, steps](std::shared_ptr<FlowSaveInfo> flow, const QString& nodeId, const maki::FocusProperties& /* properties */
    ) {
      if (!mTourGuide->isActive())
        return;

      if (mTourGuide->currentStepId() != steps->highlightControl)
        return;

      const QString tabId = QString::fromStdString(std::format("{}_{}", flow->getid(), nodeId));
      auto behaviourView = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(tabId));
      if (!behaviourView)
      {
        LOG_DEBUG("Could not find editor: {}", tabId);
        return;
      }

      auto behaviorCanvas = qobject_cast<Canvas*>(behaviourView->scene());

      steps->showBehaviourCanvas = mTourGuide->insertStep(steps->end, behaviourView, tr("Behavior Canvas"),
                                                          tr("This is the Behavior Canvas. Here, you can compose the behavior of Task Flows."),
                                                          TourStep::AdvanceOn::Button, TourStep::Invalid, steps->showBehaviourCanvas);
      steps->showBehaviourPalette = mTourGuide->insertStep(steps->end, mPalette, tr("Behavior Tab"),
                                                           tr("In the Behavior Tab, you can find the available behavior nodes. Behavior nodes define how "
                                                              "the different Capabilities must be executed in order to complete the Task."),
                                                           TourStep::AdvanceOn::Button, TourStep::Invalid, steps->showBehaviourPalette);

      steps->startDragCall = mTourGuide->insertItemStep(
        steps->end, callLibrary, callNode, tr("%1 node").arg(tour::QUICK_TOUR_CAPABILTY),
        tr("Let's update the Flow. You know the drill by now, click the \"%1\" node and dragging it onto the Canvas.").arg(tour::QUICK_TOUR_CAPABILTY),
        TourStep::AdvanceOn::UserAction, steps->showBehaviourPalette, steps->startDragCall);

      steps->endDragCall =
        mTourGuide->insertStep(steps->end, behaviourView, tr("Node connection feedback"),
                               tr("Nodes can dropped anywhere in the Canvas. If they are dropped on an existing Transition, %1 will automatically "
                                  "connect the new node. Transitions will update their color when a node can be dropped on them")
                                 .arg(Config::APPLICATION_NAME),
                               TourStep::AdvanceOn::UserAction, steps->startDragCall, steps->endDragCall);

      connect(behaviorCanvas, &Canvas::nodeAdded, this, [this, behaviourView, steps](Canvas* /* canvas */, NodeItem* node) {
        if (!mTourGuide->isActive())
          return;

        if (mTourGuide->currentStepId() != steps->endDragCall)
          return;

        steps->highlightCall = mTourGuide->insertItemStep(steps->end, behaviourView, node, tr("Updating the call node"),
                                                          tr("Perfect! Currently, the node doesn't do anything. You need to assign a Capability to be "
                                                             "called by this node. Click on the plus icon in the \"%1\" node.")
                                                            .arg(tour::QUICK_TOUR_CAPABILTY),
                                                          TourStep::AdvanceOn::UserAction, steps->endDragCall, steps->highlightCall);
        steps->completeCall =
          mTourGuide->insertItemStep(steps->end, behaviourView, node, tr("Complete call node"),
                                     tr("That is it! Now, %1 knows which Capability to call during execution.").arg(Config::APPLICATION_NAME),
                                     TourStep::AdvanceOn::Button, TourStep::Invalid, steps->completeCall);

        steps->startVerification = mTourGuide->insertStep(
          steps->end, mPipelineRun, tr("Verification"),
          tr("With the behavior defined, it can be verified. Click the \"Verify\" button. You will then be "
             "prompted to save the application and %1 will handle the rest. A formal model will be automatically generated and verified.")
            .arg(Config::APPLICATION_NAME),
          TourStep::AdvanceOn::UserAction, steps->completeCall, steps->startVerification);
      });
      connect(behaviorCanvas, &Canvas::controlCreated, this, [this, steps](maki::ControlWidget* control) {
        if (!mTourGuide->isActive())
          return;

        if (mTourGuide->currentStepId() != steps->highlightCall)
          return;

        control->setTouring();
        steps->selectCapability =
          mTourGuide->insertStep(steps->completeCall, control, tr("%1 menu").arg(tour::QUICK_TOUR_CAPABILTY),
                                 tr("Here, you can find the Capabilities which can be called. Let's select our %1 capabilty.").arg(tour::QUICK_TOUR_CAPABILTY),
                                 TourStep::AdvanceOn::UserAction, steps->highlightCall, steps->selectCapability);

        if (auto capSelect = qobject_cast<CapabilityCallSelector*>(control))
        {
          connect(capSelect, &CapabilityCallSelector::changedTabs, this, [this, capSelect, steps](const int tab) {
            if (auto eventSelector = capSelect->eventSelector(); eventSelector)
            {
              steps->selectCapabilityEvent = mTourGuide->insertStep(steps->completeCall, eventSelector, tr("%1 event selection").arg(tour::QUICK_TOUR_NODE),
                                                                    tr("After seleting the Capability, you must select the event that should be called by "
                                                                       "the \"%1\" node. Select \"%2\" in the Event drop-down list.")
                                                                      .arg(tour::QUICK_TOUR_NODE, tour::QUICK_TOUR_EVENT),
                                                                    TourStep::AdvanceOn::UserAction, steps->selectCapability, steps->selectCapabilityEvent);
              mTourGuide->advance();
            }
          });

          connect(capSelect, &CapabilityCallSelector::eventSelected, this, [this, capSelect, steps](const QString& event) {
            if (event != tour::QUICK_TOUR_EVENT)
              return;

            if (auto button = capSelect->confirmButton(); button)
            {
              steps->confirmCapability = mTourGuide->insertStep(steps->completeCall, button, tr("%1 menu").arg(tour::QUICK_TOUR_CAPABILTY),
                                                                tr("Fantastic! Now, press the Confirm button to accept your changes."),
                                                                TourStep::AdvanceOn::UserAction, steps->selectCapabilityEvent, steps->confirmCapability);
              mTourGuide->advance();
            }
          });
        }

        mTourGuide->advance();
      });

      mTourGuide->connectTriggerSuccess(behaviorCanvas, &Canvas::nodeAdded,
                                        [steps](const int currentStep) { return (currentStep == steps->endDragCall) ? TourStep::Valid : TourStep::Invalid; });
      mTourGuide->connectTriggerSuccess(behaviorCanvas, &Canvas::controlDismissed, [steps](const int currentStep) {
        return (currentStep == steps->confirmCapability) ? TourStep::Valid : TourStep::Invalid;
      });

      mTourGuide->advance();
    });

  for (const auto* section : mPalette->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
    {
      if (auto palette = qobject_cast<LibraryScene*>(library->scene()))
      {
        // We use this to automagically move the focus to the Canvas
        mTourGuide->connectTriggerSuccess(palette, &LibraryScene::dragStarted, [steps](const int currentStep) {
          return (currentStep == steps->startDragTask || currentStep == steps->startDragCapability || currentStep == steps->startDragCall) ? TourStep::Valid
                                                                                                                                           : TourStep::Invalid;
        });
        mTourGuide->connectTriggerFailure(palette, &LibraryScene::dragCancelled, [steps](const int currentStep) {
          if (currentStep == steps->endDragTask)
            return steps->startDragTask;
          else if (currentStep == steps->endDragCapability)
            return steps->startDragCapability;
          else if (currentStep == steps->endDragCall)
            return steps->startDragCall;
          else
            return TourStep::Invalid;
        });
      }
    }

  // triggers for the action-driven steps:
  mTourGuide->connectTriggerSuccess(structureCanvas, &Canvas::nodeAdded, [steps](const int currentStep) {
    return (currentStep == steps->endDragTask || currentStep == steps->endDragCapability) ? TourStep::Valid : TourStep::Invalid;
  });
  mTourGuide->connectTriggerSuccess(mPluginPipeline, &maki::PluginPipeline::pipelineFinished,
                                    [steps](const int currentStep) { return (currentStep == steps->verifying) ? TourStep::Valid : TourStep::Invalid; });

  connect(mTourGuide.get(), &TourGuide::finished, this, [] { NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Tour completed"); });
  connect(mTourGuide.get(), &TourGuide::cancelled, this, [this, runningOnStartUp]() {
    NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Tour cancelled");

    if (!mSettingsManager || !runningOnStartUp)
      return;

    const bool showTour = maki::confirmationPrompt(tr("Show the Quick Start Tour on startup?"), tr("Show"), tr("Don't show"),
                                                   tr("You can always start it again from the Help menu or enable it in the settings"));

    LOG_INFO("Show tour on start up? {}", showTour);
    if (!showTour)
    {
      auto general = mSettingsManager->general();
      general.showTourOnStart = false;
      mSettingsManager->setGeneral(general);
      mSettingsManager->applySettings();
    }
  });

  constexpr std::size_t stepCount = offsetof(Steps, taskId) / sizeof(int);
  mTourGuide->setStepCount(stepCount);
  mTourGuide->start();
}

void MainWindow::onThemeChanged(const AppearanceSettings& settings, bool initialConfig)
{
  // Apply the theme first so we can then call appStyle
  mThemeManager->setCurrentTheme(settings.theme);

  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  // These are used by the nodes
  Config::HIGHLIGHT = theme.primaryAlternativeColor;
  Config::HOVER = theme.primaryColor;
  Config::FOREGROUND = theme.secondaryColor;

  // Update the libraries
  for (const auto& section : mStructureTab->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      qobject_cast<LibraryScene*>(library->scene())->themeChanged();

  // Update all items in all canvases
  for (auto* editor : mEditorWorkspace->editors())
    if (auto* view = qobject_cast<CanvasView*>(editor))
    {
      view->onSettingsChanged(settings);
      if (auto* scene = qobject_cast<Canvas*>(view->scene()))
      {
        scene->themeChanged();
        scene->settingsChanged(settings);
      }
    }

  if (mPluginTab)
    mPluginTab->onThemeChanged();

  auto menuBarChanged = mMenuBar->isNativeMenuBar() != settings.nativeMenuBar;
  mMenuBar->setNativeMenuBar(settings.nativeMenuBar);
  if (!initialConfig && menuBarChanged)
  {
    if (maki::confirmationPrompt("A full restart is required for the menu bar to be updated", "Restart now", "Restart later"))
      onActionRestart();
  }
}

void MainWindow::onSettingsChanged(bool initialSetup)
{
  if (!mSettingsManager)
    return;

  LOG_DEBUG("Settings changed");

  onThemeChanged(mSettingsManager->appearance(), initialSetup);

  if (mFileMenu)
    mFileMenu->setGenerationRoot(mSettingsManager->generation().generationDir);

  if (mLanguageManager)
    mLanguageManager->setLanguage(mSettingsManager->general().language);

  if (mSaveHandler)
    mSaveHandler->setLastDir(mSettingsManager->general().lastOpenFileDir);

#ifdef TRACE_LOGS
  if (mSettingsManager->general().enableDebugLogs)
    logging::gMinLogLevel = logging::LogLevel::Trace;
  else
    logging::gMinLogLevel = logging::LogLevel::Debugging;
#else
  if (mSettingsManager->general().enableDebugLogs)
    logging::gMinLogLevel = logging::LogLevel::Debugging;
  else
    logging::gMinLogLevel = logging::LogLevel::Info;
#endif

  if (mLogTable)
  {
    mLogTable->onSettingsChanged(mSettingsManager->general());
    if (mSettingsManager->appearance().startLogFilterExpanded)
      mLogTable->showAll();
  }

  if (mInfoText && !mSettingsManager->general().showWelcomeMessage)
    mInfoText->clear();

  if (mPluginManager)
    mPluginManager->settingsChanged(mSettingsManager->plugins(), mHostServices);

  if (mRouter)
    mRouter->setRouteOption((EdgeRouter::Option)mSettingsManager->appearance().edgeShape);

  if (mProcessTab)
  {
    mProcessTab->setMergedLogs(mSettingsManager->plugins().showCombinedLogs);
    mProcessTab->onSettingsChanged(mSettingsManager->general());
  }

  // Clean and repopulate the recent files
  mActionOpenRecent->clear();
  for (const auto& file : mSettingsManager->general().recentFiles)
  {
    const QAction* action = mActionOpenRecent->addAction(elideLeft(file, mActionOpenRecent));
    connect(action, &QAction::triggered, [this, file] { onActionLoad(file); });
  }

  // Update node palette
  for (const auto& section : mPalette->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      library->setColumnCount(mSettingsManager->appearance().numberOfColumns);
}

void MainWindow::startUI()
{
  CanvasView* canvasView = new CanvasView(mEditorWorkspace);
  StructureCanvas* structureCanvas = new StructureCanvas(Config::MAIN_CANVAS, mRouter, canvasView);
  bindCanvas(structureCanvas);

  mActiveCanvas = structureCanvas;
  canvasView->setScene(structureCanvas);

  // Add the editor to the canvas and make sure it is active
  mEditorWorkspace->addEditor(Config::MAIN_CANVAS, canvasView, tr("System"), QIcon(":/icons/structure.svg"), false);

  mUndoGroup->addStack(structureCanvas->undoStack());
  mUndoGroup->setActiveStack(structureCanvas->undoStack());
}

void MainWindow::bind()
{
  LOG_DEBUG("Binding UI callbacks");

  // General actions =============================================================
  auto* findAction = new QAction(tr("Find"), this);
  findAction->setShortcut(QKeySequence::Find);
  findAction->setShortcutContext(Qt::ApplicationShortcut);
  addAction(findAction);

  connect(findAction, &QAction::triggered, this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Focused on: {}", fw->metaObject()->className());
    // 1) If focus is in the node library panel -> search there
    if (auto* lib = IN_TYPE(fw, QScrollArea); lib == mStructureScrollArea || lib == mBehaviourScrollArea)
    {
      // For now, we only search in the structure tab
      LOG_DEBUG("Finding in palette");
      mPaletteSearch->show();
      mPaletteSearch->widget()->setFocus(Qt::ShortcutFocusReason);
      return;
    }
    else if (auto* search = IN_TYPE(fw, maki::SearchWidget); search == mPaletteSearch)
    {
      LOG_DEBUG("Finding in palette");
      mPaletteSearch->show();
      mPaletteSearch->widget()->setFocus(Qt::ShortcutFocusReason);
      return;
    }
    // 2) If focus is in the canvas -> search there
    else if (auto* view = IN_TYPE(fw, CanvasView))
    {
      Q_UNUSED(view);
      LOG_DEBUG("Finding in canvas");
      return;
    }
    else if (auto* bottom = IN_TYPE(fw, QStackedWidget); bottom == mBottomPanel)
    {
      LOG_DEBUG("Finding in bottom container");
      if (mBottomPanel->currentIndex() == LOG_TAB_INDEX)
        mLogTable->search();
      return;
    }
  });

  // File actions =============================================================
  connect(mActionNew, &QAction::triggered, this, [this] { onActionNew(""); });
  mActionNew->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));

  connect(mActionOpen, &QAction::triggered, [this] { onActionLoad(""); });
  mActionOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

  connect(mActionSave, &QAction::triggered, [this] { LOG_WARN_ON_FAILURE(onActionSave()); });
  mActionSave->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));

  connect(mActionExit, &QAction::triggered, this, &MainWindow::onActionExit);

  connect(mActionSaveAs, &QAction::triggered, this, &MainWindow::onActionSaveAs);
  mActionSaveAs->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

  connect(mPipelineRun, &ExecuteButton::executeRequested, this, &MainWindow::onActionGenerate);
  connect(mPipelineRun, &ExecuteButton::editOptionRequested, this, &MainWindow::onActionEditPipeline);
  connect(mPipelineRun, &ExecuteButton::renameOptionRequested, this, &MainWindow::onActionRenamePipeline);
  connect(mPipelineRun, &ExecuteButton::deleteOptionRequested, this, &MainWindow::onActionDeletePipeline);

  // View actions =============================================================
  mOpenComponentsPanel->setShortcut(QKeySequence(Qt::Key_F7));
  mOpenInfoPanel->setShortcut(QKeySequence(Qt::Key_F8));
  mOpenPropertiesPanel->setShortcut(QKeySequence(Qt::Key_F9));

  connect(mPluginTab, &PluginTab::openView, this, &MainWindow::addPluginTab);
  connect(mPluginTab, &PluginTab::closeView, this, &MainWindow::removePluginTab);

  mActionToggleToasts->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_M));
  connect(mActionToggleToasts, &QAction::toggled, this, [this](bool toggled) {
    if (mNotificationManager)
      mNotificationManager->toggleMinimize(toggled);
  });

  // Diagram actions =============================================================
  connect(mActionAutoRoute, &QAction::triggered, this, [this] {
    if (canvas())
      canvas()->autoRoute();
  });
  mActionAutoRoute->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_L));
  connect(mActionDistribute, &QAction::triggered, this, [this] {
    if (canvas())
      canvas()->requestDistributeNodes();
  });
  mActionDistribute->setShortcut(QKeySequence(Qt::ALT | Qt::Key_D));
  connect(mActionAlignH, &QAction::triggered, this, [this] {
    if (canvas())
      canvas()->alignSelectedNodes(Types::AlignmentMode::HORIZONTAL);
  });
  mActionAlignH->setShortcut(QKeySequence(Qt::ALT | Qt::Key_H));
  connect(mActionAlignV, &QAction::triggered, this, [this] {
    if (canvas())
      canvas()->alignSelectedNodes(Types::AlignmentMode::VERTICAL);
  });
  mActionAlignV->setShortcut(QKeySequence(Qt::ALT | Qt::Key_V));

  // Setting actions =============================================================
  connect(mOpenAllSettings, &QAction::triggered, this, [this] {
    LOG_INFO("Opening all settings");
    SettingsDialog* settingsDialog = new SettingsDialog(tr("Settings"), mSettingsManager, mLanguageManager, this);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->show();
  });
  mOpenAllSettings->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));

  connect(mActionDocumentation, &QAction::triggered, this, [] {
    if (maki::warningPrompt(tr("Going outside %1").arg(Config::APPLICATION_NAME), tr("This will open the documentation on your default browser. Continue?")))
      if (!QDesktopServices::openUrl(QUrl(Config::MAKI_WEBSITE)))
        NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "Could not open the documentation");
  });
  mActionDocumentation->setShortcut(QKeySequence(Qt::Key_F1));

  connect(mActionQuickStartGuide, &QAction::triggered, this, [this] {
    if (!SaveInfo::instance().getnodes().isEmpty())
    {
      NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "Make sure you are on an empty project before starting the Quick Start Guide");
      return;
    }

    startTourGuide(false);
  });
  connect(mAboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
  mAboutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F1));

  // Internal actions =============================================================
  connect(mEditorWorkspace, &maki::EditorWorkspace::activeEditorChanged, this, &MainWindow::onCanvasTabChanged);
  connect(mEditorWorkspace, &maki::EditorWorkspace::editorCloseRequested, this, &MainWindow::closeCanvasTab);

  connect(rootCanvas(), &Canvas::openFlow, this, &MainWindow::onOpenFlow);
  connect(rootCanvas(), &Canvas::flowAdded, this, &MainWindow::onFlowAdded);
  connect(rootCanvas(), &Canvas::flowRemoved, this, &MainWindow::onFlowRemoved);

  connect(mPropertiesMenu, &PropertiesMenu::flowSelected, this,
          [this](const QString& flowId, const QString& nodeId) { focusOn(nodeId, flowId, maki::FocusProperties::internal()); });
  connect(mHostServices, &HostServices::onFocusOnNode, this, &MainWindow::focusOn);

  connect(mSystemMenu, &SystemMenu::editPipeline, this, &MainWindow::onActionEditPipeline);
  connect(mSystemMenu, &SystemMenu::removePipeline, this, &MainWindow::onActionDeletePipeline);
  connect(mSystemMenu, &SystemMenu::flowSelected, this,
          [this](const QString& flowId, const QString& nodeId) { focusOn(nodeId, flowId, maki::FocusProperties::internal()); });
  connect(mSystemMenu, &SystemMenu::flowRemoved, rootCanvas(), &Canvas::onFlowRemoved);
  connect(mSystemMenu, &SystemMenu::nodeRemoved, rootCanvas(), &Canvas::onRemoveNode);
  connect(mSystemMenu, &SystemMenu::nodeFocused, this,
          [this](const QString& flowId, const QString& nodeId) { focusOn(nodeId, flowId, maki::FocusProperties::internal()); });

  connect(mPropertiesMenu, &PropertiesMenu::flowRemoved, rootCanvas(), &Canvas::onFlowRemoved);
  connect(mPropertiesMenu, &PropertiesMenu::openParameter, this, [this](const QString& parameterId) {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(MISSION_PARAMETER_INDEX);

    if (mMissionParameters)
      mMissionParameters->editParameter(parameterId);
  });

  connect(mTypeEditor, &maki::TypeEditor::requestFocus, this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(TYPE_EDITOR_INDEX);
  });

  connect(mInfoText, &QTextBrowser::anchorClicked, this, [](const QUrl& url) { maki::TypeRegistry::instance().showDefinition(typeFromLink(url)); });

  connect(mFileMenu, &GeneratedFilesPanel::openExternallyRequested, this, &MainWindow::addEditorTab);

  connect(mSaveHandler.get(), &SaveHandler::fileLoaded, this, &MainWindow::onFileLoaded);
  connect(mSaveHandler.get(), &SaveHandler::fileSaved, mSettingsManager.get(), &SettingsManager::addRecentFile);

  connect(mPluginManager.get(), &PluginManager::pluginAdded, [this](const PluginManager::Plugin& plugin) {
    LOG_INFO("====== Plugin added ======");
    QJsonObject library;
    library[ConfigKeys::NAME] = plugin.plugin->languageName();
    library[ConfigKeys::TYPE] = ConfigKeys::PIPELINE;

    QJsonArray nodes;
    for (const auto& action : mPluginPipeline->registry()->actionsOfPlugin(plugin.plugin->languageName()))
    {
      QJsonObject node;
      node[ConfigKeys::TYPE] = action->id();

      QJsonObject body;
      body["shape"] = "circle";
      body["textColor"] = "#FFFFFF";
      body["backgroundColor"] = "#D1D1D1";
      body["width"] = 100;
      body["height"] = 100;
      body["icon"] = plugin.plugin->manifest().iconPath();

      node["body"] = body;

      QJsonArray ports = {"in", "out"};
      node["ports"] = ports;

      QJsonArray properties;
      QJsonObject name;
      QJsonObject nameDefault;
      name[ConfigKeys::ID] = "name";
      name[ConfigKeys::TYPE] = "string";
      nameDefault["kind"] = "string";
      nameDefault["value"] = action->displayName();
      name[ConfigKeys::DEFAULT] = nameDefault;
      properties.append(name);

      for (const auto& p : action->parameters())
      {
        QJsonObject prop;
        prop[ConfigKeys::ID] = p.getid();
        prop[ConfigKeys::TYPE] = QString::fromStdString(p.gettype().toString());
        if (const auto* val = dynamic_cast<const maki::Value*>(p.getvalue()))
          name[ConfigKeys::DEFAULT] = val->toJson();
        properties.append(prop);
      }

      node[ConfigKeys::PROPERTIES] = properties;
      nodes.append(node);
    }

    library[ConfigKeys::NODES] = nodes;
    LOG_WARN_ON_FAILURE(loadElementLibrary(library[ConfigKeys::NAME].toString(), library, false));
  });
  connect(mSettingsManager.get(), &SettingsManager::settingsChanged, this, [this] { onSettingsChanged(false); });

  // Pipeline stuff =============================================================
  connect(mPluginPipeline, &maki::PluginPipeline::pipelineStarted, [this] { toggleGenerationButton(true); });
  connect(mPluginPipeline, &maki::PluginPipeline::pipelineFinished, [this](const QString& outputFolder) {
    if (!outputFolder.isEmpty())
      mFileMenu->setGenerationRoot(outputFolder);

    toggleGenerationButton(false);
  });

  // Search stuff =============================================================
  connect(mPaletteSearch, &maki::SearchWidget::valueChanged, [this](const QString& query) {
    // This is inefficient and might cause some issues in the future since the search is done on every input change.
    // Still, it is enough for now
    QList<SectionWidget*> sections = mStructureTab->findChildren<SectionWidget*>();
    for (const auto& section : sections)
      if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
        section->setExpanded(library->filterNodes(query) || query.isEmpty());
  });
}

void MainWindow::changeEvent(QEvent* event)
{
  QMainWindow::changeEvent(event);
  if (event->type() == QEvent::LanguageChange)
  {
    LOG_DEBUG("Language changed");
    onLanguageChanged();
  }
}

void MainWindow::bindCanvas(Canvas* canvas)
{
  connect(canvas, &Canvas::openFlow, this, &MainWindow::onOpenFlow);
  connect(canvas, &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  connect(canvas, &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  connect(canvas, &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  connect(canvas, &Canvas::nodeModified, this, &MainWindow::onNodeModified);
  connect(canvas, &Canvas::focusOn, this, &MainWindow::focusOn);
  connect(canvas, &Canvas::createEvent, this, [this](NodeItem* node) {
    if (mPropertiesMenu)
      LOG_WARN_ON_FAILURE(mPropertiesMenu->onCreateEvent(node, false));
  });
  connect(canvas, &Canvas::transitionSelected, mPropertiesMenu, &PropertiesMenu::onTransitionSelected);
}

void MainWindow::unbindCanvas(Canvas* canvas)
{
  disconnect(canvas, &Canvas::openFlow, this, &MainWindow::onOpenFlow);
  disconnect(canvas, &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  disconnect(canvas, &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  disconnect(canvas, &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  disconnect(canvas, &Canvas::nodeModified, this, &MainWindow::onNodeModified);
  disconnect(canvas, &Canvas::focusOn, this, &MainWindow::focusOn);
  disconnect(canvas, &Canvas::createEvent, this, nullptr);
  disconnect(canvas, &Canvas::transitionSelected, mPropertiesMenu, &PropertiesMenu::onTransitionSelected);
}

void MainWindow::bindShortcuts()
{
  // Copy
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this, [] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Copy, focused on: {}", fw->metaObject()->className());
    if (auto* textEdit = IN_TYPE(fw, QTextEdit))
    {
      textEdit->copy();
      return;
    }
    else if (auto* browser = IN_TYPE(fw, QTextBrowser))
    {
      browser->copy();
      return;
    }
    else if (auto* canvasView = IN_TYPE(fw, CanvasView))
    {
      if (auto* scene = qobject_cast<Canvas*>(canvasView->scene()))
        scene->copySelectedItems(nullptr);

      return;
    }
  });
  // Paste
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Copy, focused on: {}", fw->metaObject()->className());

    // 1) If focus is in the node library panel -> search there
    if (auto* textEdit = IN_TYPE(fw, QTextEdit))
    {
      if (!textEdit->isReadOnly())
        textEdit->paste();

      return;
    }
    else if (auto* browser = IN_TYPE(fw, QTextBrowser))
    {
      if (!browser->isReadOnly())
        browser->paste();

      return;
    }
    else if (auto* canvasView = IN_TYPE(fw, CanvasView))
    {
      if (auto* scene = qobject_cast<Canvas*>(canvasView->scene()))
        scene->pasteCopiedItems();

      return;
    }
  });
  // Focus shift backward
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Ctrl, tab, focused on: {}", fw->metaObject()->className());
    if (auto* stackedWidget = IN_TYPE(fw, QStackedWidget); stackedWidget == mBottomPanel && mBottomNavigation)
    {
      int next = mBottomNavigation->currentIndex() - 1;
      mBottomNavigation->setCurrentIndex(next < 0 ? mBottomNavigation->itemCount() - 1 : next);
      return;
    }
    else if (auto* widget = IN_TYPE(fw, oclero::qlementine::AbstractItemListWidget); widget == mBottomNavigation)
    {
      int next = mBottomNavigation->currentIndex() - 1;
      mBottomNavigation->setCurrentIndex(next < 0 ? mBottomNavigation->itemCount() - 1 : next);
      return;
    }
    else if (auto* tabWidget = IN_TYPE(fw, maki::EditorWorkspace); tabWidget == mEditorWorkspace)
    {
      auto* group = mEditorWorkspace->activeGroup();
      if (!group)
        return;

      int next = group->currentIndex() - 1;
      group->setCurrentIndex(next < 0 ? group->count() - 1 : next);
      return;
    }
  });
  // Focus shift forward
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Ctrl, shift, tab, focused on: {}", fw->metaObject()->className());
    if (auto* stackedWidget = IN_TYPE(fw, QStackedWidget); stackedWidget == mBottomPanel && mBottomNavigation)
    {
      int next = mBottomNavigation->currentIndex() + 1;
      mBottomNavigation->setCurrentIndex(next >= mBottomNavigation->itemCount() ? 0 : next);
      return;
    }
    else if (auto* widget = IN_TYPE(fw, oclero::qlementine::AbstractItemListWidget); widget == mBottomNavigation)
    {
      int next = mBottomNavigation->currentIndex() + 1;
      mBottomNavigation->setCurrentIndex(next >= mBottomNavigation->itemCount() ? 0 : next);
      return;
    }
    else if (auto* tabWidget = IN_TYPE(fw, maki::EditorWorkspace); tabWidget == mEditorWorkspace)
    {
      auto* group = mEditorWorkspace->activeGroup();
      if (!group)
        return;

      int next = group->currentIndex() + 1;
      group->setCurrentIndex(next >= group->count() ? 0 : next);
      return;
    }
  });
  // Center nodes
  new QShortcut(QKeySequence(Qt::ALT | Qt::Key_C), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("ALT+C, focused on: {}", fw->metaObject()->className());
    if (auto* tabWidget = IN_TYPE(fw, maki::EditorWorkspace); tabWidget == mEditorWorkspace)
    {
      if (auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->activeEditor()); view)
        qobject_cast<Canvas*>(view->scene())->centerOnNodes();
    }
  });
  // Select all
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("CTRL+A, focused on: {}", fw->metaObject()->className());
    if (auto* tabWidget = IN_TYPE(fw, maki::EditorWorkspace); tabWidget == mEditorWorkspace)
    {
      if (auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->activeEditor()); view)
        qobject_cast<Canvas*>(view->scene())->selectAll();
    }
  });
  // Delete
  new QShortcut(QKeySequence(Qt::Key_Delete), this, [this] {
    if (canvas())
      canvas()->deleteSelectedItems();
  });
  // Reload plugins
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::ALT | Qt::Key_R), this, [this] {
    if (mPluginManager && mPluginManager->currentPlugin())
      LOG_WARN_ON_FAILURE(mPluginManager->reloadPlugin(mPluginManager->currentPlugin()->languageName(), mHostServices));
  });
  // Add new mission parameter
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(MISSION_PARAMETER_INDEX);

    if (mMissionParameters)
      mMissionParameters->addParameter();
  });
  // Add new record type
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(TYPE_EDITOR_INDEX);

    if (mTypeEditor)
    {
      mTypeEditor->createRecord();
      mTypeEditor->focusCurrentEditor();
    }
  });
  // Add new alias type
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(TYPE_EDITOR_INDEX);

    if (mTypeEditor)
    {
      mTypeEditor->createAlias();
      mTypeEditor->focusCurrentEditor();
    }
  });
  // Add new enum type
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(TYPE_EDITOR_INDEX);

    if (mTypeEditor)
    {
      mTypeEditor->createEnum();
      mTypeEditor->focusCurrentEditor();
    }
  });

  mActionUndo->setShortcuts(QKeySequence::Undo);
  mActionRedo->setShortcuts(QKeySequence::Redo);
}

Canvas* MainWindow::canvas() const
{
  return mActiveCanvas;
}

Canvas* MainWindow::rootCanvas() const
{
  auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(Config::MAIN_CANVAS));
  if (!view)
    return nullptr;

  return qobject_cast<Canvas*>(view->scene());
}

VoidResult MainWindow::loadElements()
{
  LOG_DEBUG("Loading the elements");

  auto libPaths = AppPaths::libraries();
  auto loadLib = [this](const QStringList& libPaths, bool dataOnly) {
    for (const auto& path : libPaths)
    {
      QDir libDir(path);
      QStringList files = libDir.entryList(QDir::Files);
      for (const auto& file : files)
      {
        const auto fileName = libDir.absoluteFilePath(file);
        auto libRead = JSON::fromFile(fileName);
        if (!libRead.IsSuccess())
          return VoidResult::Failed("Failed to open library: {}", fileName);

        LOG_DEBUG("Trying to load library: {}", fileName);
        auto libConfig = libRead.Value();
        LOG_ERROR_ON_FAILURE(loadLibrary(libConfig, dataOnly));
      }
    }
    return VoidResult();
  };

  // First load all the available data types
  RETURN_ON_FAILURE(loadLib(libPaths, true));
  // Then the rest of the libraries
  RETURN_ON_FAILURE(loadLib(libPaths, false));

  // Once we are done with the libraries, we can make sure they are positioned on the top
  dynamic_cast<QVBoxLayout*>(mStructureTab->layout())->addStretch();
  dynamic_cast<QVBoxLayout*>(mBehaviourTab->layout())->addStretch();
  dynamic_cast<QVBoxLayout*>(mPipelineTab->layout())->addStretch();

  NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Loaded all libraries");
  return VoidResult();
}

VoidResult MainWindow::loadLibrary(const JSON& config, bool dataOnly)
{
  if (!config.contains(ConfigKeys::NAME))
    return VoidResult::Failed("Packages must have a name");

  if (!config.contains(ConfigKeys::LIBRARIES))
    return VoidResult::Failed("Packages must have libraries");

  QString name = config[ConfigKeys::NAME].toString();
  auto libraries = config[ConfigKeys::LIBRARIES].toArray();
  for (const auto& value : libraries)
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid library format");

    QJsonObject library = value.toObject();
    LOG_ERROR_ON_FAILURE(loadElementLibrary(name, library, dataOnly));
  }

  return VoidResult();
}

VoidResult MainWindow::loadElementLibrary(const QString& name, const JSON& config, bool dataOnly)
{
  if (!config.contains(ConfigKeys::TYPE))
    return VoidResult::Failed("Libraries must have a type");

  QString libraryName = config.contains(ConfigKeys::NAME) ? config[ConfigKeys::NAME].toString() : name;
  QString type = config[ConfigKeys::TYPE].toString();

  if (dataOnly && type == ConfigKeys::DATA)
    return maki::TypeRegistry::instance().loadFromLibrary(config);

  if (dataOnly || type == ConfigKeys::DATA)
    return VoidResult();

  LOG_DEBUG("Loading library: {} of {}", libraryName, name);

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  QWidget* toolbox = nullptr;
  if (type == ConfigKeys::STRUCTURAL)
    toolbox = mStructureTab;
  else if (type == ConfigKeys::BEHAVIOURAL)
    toolbox = mBehaviourTab;
  else if (type == ConfigKeys::PIPELINE)
    toolbox = mPipelineTab;
  else
    return VoidResult::Failed("Unknown library type: {}", type);

  LibraryContainer* sidebarview = LibraryContainer::create(libraryName, toolbox);
  const auto* sidebarScene = dynamic_cast<LibraryScene*>(sidebarview->scene());
  connect(sidebarScene, &LibraryScene::libraryNodeSelected, [this](const QString& nodeType, const QString& /* instanceId */) {
    if (auto info = ConfigurationTable::instance().get(nodeType))
      mInfoText->setHtml(createInformationMessage(*info));
  });
  // Make sure the configured value is there by default
  sidebarview->setColumnCount(mSettingsManager->appearance().numberOfColumns);

  auto nodes = config[ConfigKeys::NODES];
  if (!nodes.isArray())
    return VoidResult::Failed("nodes must be in a list in the format \"nodes\": []");

  // Every library has a bunch of elements, here we add them.
  for (const auto& value : nodes.toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid node format");

    QJsonObject node = value.toObject();
    // LOG_INFO("Loading element: {}", node[ConfigKeys::TYPE].toString());

    // Parse config and make sure it is valid before continuing
    auto nodeConfig = std::make_shared<NodeConfig>(node);
    if (!nodeConfig->isValid())
      return VoidResult::Failed(nodeConfig->errorMessage.toStdString());

    // Initialize the library type
    if (type == ConfigKeys::STRUCTURAL)
      nodeConfig->libraryType = Types::LibraryTypes::STRUCTURAL;
    else if (type == ConfigKeys::BEHAVIOURAL)
      nodeConfig->libraryType = Types::LibraryTypes::BEHAVIOUR;
    else if (type == ConfigKeys::PIPELINE)
      nodeConfig->libraryType = Types::LibraryTypes::PIPELINE;

    auto nodeId = QStringLiteral("%1::%2").arg(name, nodeConfig->type);
    nodeConfig->type = nodeId;
    sidebarview->addNode(nodeId, nodeConfig);

    LOG_TRACE("Adding key: {} to the config table", nodeId);
    LOG_ERROR_ON_FAILURE(ConfigurationTable::instance().add(nodeId, nodeConfig));
    LOG_ERROR_ON_FAILURE(maki::TypeRegistry::instance().registerNode(nodeId, NodeSaveInfo(*nodeConfig)));
  }

  return VoidResult();
}

void MainWindow::onActionNew(const QString& templateName)
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  // Repopulate the canvas
  SaveInfo emptySave;
  SaveInfo::instance().reset();

  // Gotta make sure we don't save over an old file
  mSaveHandler->newFileCreated();

  for (auto* editor : mEditorWorkspace->editors())
  {
    if (auto view = qobject_cast<CanvasView*>(editor))
      if (auto* scene = qobject_cast<Canvas*>(view->scene()))
        scene->loadFromSave(emptySave);

    const auto tabId = mEditorWorkspace->editorId(editor);
    if (tabId != Config::MAIN_CANVAS)
      closeCanvasTab(editor);
  }

  maki::TypeRegistry::instance().removeUserTypes();
  if (mMissionParameters)
    mMissionParameters->documentUpdated();
  if (mPipelineRun)
    mPipelineRun->reset();
  if (mSystemMenu)
    mSystemMenu->clear();

  if (!templateName.isEmpty())
    onActionLoad(templateName, true);

  NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Created new project");
}

void MainWindow::onActionExit()
{
  QCoreApplication::quit();
}

void MainWindow::onActionRestart()
{
  LOG_DEBUG("Restart requested");
  // Prepare to start a new instance of MAKI
  QObject::connect(qApp, &QCoreApplication::aboutToQuit, [] {
    const QString program = QCoreApplication::applicationFilePath();
    const QStringList args = QCoreApplication::arguments().mid(1);

    QProcess::startDetached(program, args);
  });

  // Quit from the running instance
  onActionExit();
}

void MainWindow::onActionGenerate(const QString& pipelineId, bool fromScratch)
{
  if (!mPluginPipeline)
    LOG_WARNING("No pipeline available");

  // If we are running, then we should cancel
  if (mPluginPipeline->isRunning())
  {
    LOG_DEBUG("Plugin pipeline is running, stopping");
    LOG_WARN_ON_FAILURE(mPluginPipeline->abort());
    return;
  }

  QByteArray byteArray;
  QDataStream out(&byteArray, QIODevice::WriteOnly);
  out << mHostServices->document()->getnodes();

  if (SaveInfo::instance().name.isEmpty() && !onActionSave().IsSuccess())
    return;

  maki::PipelineContext context;
  context.buildDir = QDir(mSettingsManager->generation().generationDir + "/" + SaveInfo::instance().name);
  context.projectDir = QDir(mSettingsManager->generation().generationDir + "/" + SaveInfo::instance().name);
  context.addArtifact({
    .id = "maki.nodes",
    .type = "maki",
    .producer = Config::APPLICATION_NAME,
    .paths =
      {
        {"nodes", byteArray.toBase64()},
      },
  });

  // Make sure the project exists
  if (!context.buildDir.exists())
    context.buildDir.mkpath(".");

  for (const auto& pipeline : SaveInfo::instance().pipelines())
  {
    if (pipeline->getid() != pipelineId)
      continue;

    auto graph = maki::PipelineGraph::fromFlow(*pipeline);
    if (!graph.IsSuccess())
    {
      LOG_ERROR(graph.ErrorMessage());
      return;
    }

    graph.Value().print();

    LOG_ERROR_ON_FAILURE(mPluginPipeline->run(graph.Value(), context, fromScratch));
    return;
  }

  // We tried running an unknown pipeline, so we can create it
  onActionEditPipeline(pipelineId);
}

void MainWindow::onActionDeletePipeline(const QString& pipelineId)
{
  if (!mPluginPipeline)
  {
    LOG_WARNING("No pipeline available");
    return;
  }

  std::shared_ptr<FlowSaveInfo> pipeline;
  for (const auto& p : SaveInfo::instance().pipelines())
  {
    if (p->getid() != pipelineId)
      continue;

    pipeline = p;
    break;
  }

  if (!pipeline)
  {
    LOG_WARNING("Could not find pipeline: {}", pipelineId);
    return;
  }

  // Check if the pipeline is currently open
  for (auto* editor : mEditorWorkspace->editors())
  {
    auto editorId = mEditorWorkspace->editorId(editor);
    if (editorId != pipeline->getid())
      continue;

    // Change to the previous tab and remove the pipeline tab
    closeCanvasTab(editor);
    break;
  }

  SaveInfo::instance().removePipeline(pipeline);
  mPipelineRun->removeOption(pipeline->getid());  // Update the UI as well
  mSystemMenu->onPipelineRemoved(pipelineId);
}

void MainWindow::onActionRenamePipeline(const QString& pipelineId)
{
  std::shared_ptr<FlowSaveInfo> pipeline;
  for (const auto& p : SaveInfo::instance().pipelines())
  {
    if (p->getid() != pipelineId)
      continue;

    pipeline = p;
    break;
  }

  if (!pipeline)
  {
    LOG_WARNING("No pipeline with id {}", pipelineId);
    return;
  }

  auto name = maki::textPrompt(tr("Pipeline rename"), tr("Choose a new pipeline name"), this);
  if (name.isEmpty())
    return;

  auto exists = maki::anyOf(SaveInfo::instance().pipelines(), [&name](const auto& opt) { return opt->getname() == name; });
  if (exists)
  {
    NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "{}: {}", tr("There is already a pipeline named"), name);
    return;
  }

  pipeline->setName(name);
  mPipelineRun->addOption(pipeline->getname(), pipeline->getid());
  mSystemMenu->onPipelineAdded(pipeline);

  for (const auto* editor : mEditorWorkspace->editors())
  {
    auto editorId = mEditorWorkspace->editorId(editor);
    if (editorId != pipeline->getid())
      continue;

    mEditorWorkspace->setEditorTitle(editorId, pipeline->getname());
    break;
  }

  NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "{}", tr("Pipeline renamed successfully"));
}

void MainWindow::onActionEditPipeline(const QString& pipelineId)
{
  std::shared_ptr<FlowSaveInfo> pipeline;
  for (const auto& p : SaveInfo::instance().pipelines())
  {
    if (p->getid() != pipelineId)
      continue;

    pipeline = p;
    break;
  }

  // If we are dealing with an unknown or new pipeline, we must create it
  if (!pipeline)
  {
    FlowSaveInfo info;
    auto name = maki::textPrompt(tr("Pipeline name"), tr("Choose a pipeline name"), this);
    if (name.isEmpty())
      return;

    auto exists = maki::anyOf(SaveInfo::instance().pipelines(), [&name](const auto& opt) { return opt->getname() == name; });
    if (exists)
    {
      NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "{}: {}", tr("There is already a pipeline named"), name);
      return;
    }

    pipeline = std::make_shared<FlowSaveInfo>(info);
    pipeline->setId(QUuid::createUuid().toString());
    pipeline->setName(name);

    SaveInfo::instance().addPipeline(pipeline);
    mPipelineRun->addOption(pipeline->getname(), pipeline->getid());  // Update the UI as well
    mSystemMenu->onPipelineAdded(pipeline);
  }

  // Check if the pipeline is already open
  for (const auto* editor : mEditorWorkspace->editors())
  {
    auto editorId = mEditorWorkspace->editorId(editor);
    if (editorId != pipeline->getid())
      continue;

    mEditorWorkspace->activateEditor(editorId);
    return;
  }

  CanvasView* newView = new CanvasView(mEditorWorkspace);
  PipelineCanvas* pipelineCanvas = new PipelineCanvas(pipeline, mRouter, newView);
  bindCanvas(pipelineCanvas);
  newView->setScene(pipelineCanvas);
  pipelineCanvas->populate(*pipeline);

  // Hide all tabs in the palette but the pipeline one
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::STRUCTURAL), false);
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::BEHAVIOUR), false);
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::PIPELINE), true);
  mPalette->setCurrentIndex(libraryTypeToIndex(Types::LibraryTypes::PIPELINE));

  mEditorWorkspace->addEditor(pipelineId, newView, pipeline->getname(), QIcon(":/icons/pipeline.svg"), true);
  mEditorWorkspace->activateEditor(pipelineId);
}

VoidResult MainWindow::onActionSave()
{
  if (!mSaveHandler)
    return VoidResult::Failed("System not initialized");

  if (CanvasView* view = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(Config::MAIN_CANVAS)))
  {
    if (auto* scene = qobject_cast<Canvas*>(view->scene()))
    {
      CanvasSaveInfo info;
      info.setCenter(scene->getCenter());
      info.setScale(scene->getScale());
      SaveInfo::instance().setCanvasInfo(info);
    }
  }

  auto saved = mSaveHandler->saveProject(SaveInfo::instance());
  if (saved)
  {
    NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Saved project: {}", SaveInfo::instance().name.toStdString());
  }
  else
  {
    LOG_ERROR(saved.ErrorMessage());
    NOTIFY_ERROR(Config::APPLICATION_NAME.toStdString(), "Could not save project: {}\n{}", SaveInfo::instance().name.toStdString(), saved.ErrorMessage());
  }

  return saved;
}

void MainWindow::onActionSaveAs()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  auto saved = mSaveHandler->saveProjectAs(SaveInfo::instance());
  if (saved)
  {
    NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Saved project: {}", SaveInfo::instance().name.toStdString());
  }
  else
  {
    LOG_ERROR(saved.ErrorMessage());
    NOTIFY_ERROR(Config::APPLICATION_NAME.toStdString(), "Could not save project: {}\n{}", SaveInfo::instance().name.toStdString(), saved.ErrorMessage());
  }
}

void MainWindow::onActionLoad(const QString& filename, bool fromTemplate)
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  if (filename.isEmpty())
  {
    mSaveHandler->loadProject();
    return;
  }

  // Clear the previous types and parameters
  maki::TypeRegistry::instance().removeUserTypes();
  auto loaded = mSaveHandler->loadProject(filename);
  if (!loaded.IsSuccess())
  {
    onFileLoaded(filename, SaveInfo(), QString::fromStdString(loaded.ErrorMessage()), fromTemplate);
    return;
  }

  auto info = loaded.Value();
  onFileLoaded(filename, info, "", fromTemplate);
}

void MainWindow::onFileLoaded(const QString& file, const SaveInfo& info, const QString& error, bool fromTemplate)
{
  if (!error.isEmpty())
  {
    LOG_ERROR("Failed to load project: {}", error);
    NOTIFY_ERROR(Config::APPLICATION_NAME.toStdString(), "Failed to load project.\n{}", error);
    return;
  }

  if (mSettingsManager)
    mSettingsManager->addRecentFile(file);

  // Close all tabs except the first
  mEditorWorkspace->closeAllExcept(Config::MAIN_CANVAS);

  // Clear the storage so it can be populated by the canvas
  SaveInfo::instance() = info;
  SaveInfo::instance().clearNodes();

  // Repopulate the canvas (and the storage)
  rootCanvas()->loadFromSave(info);

  if (mMissionParameters)
    mMissionParameters->documentUpdated();

  GeneralSettings general = mSettingsManager->general();
  general.lastOpenFileDir = mSaveHandler->lastDir();
  mSettingsManager->setGeneral(general);
  mSettingsManager->applySettings();

  if (mPipelineRun)
  {
    mPipelineRun->reset();
    for (const auto& pipeline : SaveInfo::instance().pipelines())
    {
      mPipelineRun->addOption(pipeline->getname(), pipeline->getid());
      if (mSystemMenu)
        mSystemMenu->onPipelineAdded(pipeline);
    }
  }

  if (mSystemMenu)
    mSystemMenu->expandToDepth(0);

  if (fromTemplate)
  {
    SaveInfo::instance().rootPath.clear();
    SaveInfo::instance().name.clear();
    SaveInfo::instance().version.clear();
    SaveInfo::instance().saveFile.clear();
  }

  LOG_INFO("Project with {} nodes after", SaveInfo::instance().getnodes().size());
  NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Loaded project: {}", SaveInfo::instance().name.toStdString());
}

void MainWindow::onNodeSelected(NodeItem* node, bool selected)
{
  if (node)
  {
    if (auto info = ConfigurationTable::instance().get(node->nodeType()))
      mInfoText->setHtml(createInformationMessage(*info));
  }
  else
  {
    if (mSettingsManager->general().showWelcomeMessage)
      mInfoText->setHtml(createDefaultMessage());
    else
      mInfoText->clear();
  }

  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeSelected(node, selected));
}

void MainWindow::onNodeAdded(Canvas* canvas, NodeItem* node)
{
  if (!canvas)
  {
    LOG_WARNING("A node was added but no canvas was provided");
    return;
  }

  if (!node)
  {
    LOG_WARNING("A node was added but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeAdded(canvas->id(), node, canvas->type()));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeAdded(node));
}

void MainWindow::onNodeRemoved(Canvas* canvas, const QString& nodeId, const QString& parentId)
{
  if (!canvas)
  {
    LOG_WARNING("A node was added but no canvas was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeRemoved(canvas->id(), nodeId, parentId));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeRemoved(nodeId));
}

void MainWindow::onNodeModified(Canvas* canvas, NodeItem* node)
{
  if (!canvas)
  {
    LOG_WARNING("A node was added but no canvas was provided");
    return;
  }

  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeModified(canvas->id(), node));
}

void MainWindow::onCanvasTabChanged(QWidget* widget)
{
  CanvasView* newCanvas = qobject_cast<CanvasView*>(widget);
  if (!newCanvas)
    return;

  mActiveCanvas = qobject_cast<Canvas*>(newCanvas->scene());
  if (!canvas())
    return;

  auto libIndex = libraryTypeToIndex(canvas()->type());
  mPalette->setCurrentIndex(libIndex);

  auto activeStack = canvas()->undoStack();
  if (!mUndoGroup->stacks().contains(activeStack))
    mUndoGroup->addStack(activeStack);

  mUndoGroup->setActiveStack(activeStack);

  if (canvas()->type() == Types::LibraryTypes::PIPELINE)
  {
    // If we are closing a pipeline canvas, then we must hide it
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::STRUCTURAL), false);
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::BEHAVIOUR), false);
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::PIPELINE), true);
  }
  else
  {
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::STRUCTURAL), true);
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::BEHAVIOUR), true);
    mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::PIPELINE), false);
  }

  // Select the first available node
  auto existing = maki::findIfPtr(canvas()->selectedItems(), [](const auto& opt) { return opt->type() == NodeItem::Type; });
  if (auto* nodeItem = qgraphicsitem_cast<NodeItem*>(existing))
    onNodeSelected(nodeItem, true);
}

void MainWindow::closeCanvasTab(QWidget* page)
{
  const auto editorId = mEditorWorkspace->editorId(page);
  if (CanvasView* closedCanvas = qobject_cast<CanvasView*>(page))
  {
    auto toBeRemoved = qobject_cast<Canvas*>(closedCanvas->scene());
    mUndoGroup->removeStack(toBeRemoved->undoStack());

    if (mPropertiesMenu)
      mPropertiesMenu->onNodeSelected(nullptr, false);

    // If we are closing the currently focused canvas, we need to unbind its callbacks
    if (toBeRemoved->id() == canvas()->id())
      unbindCanvas(toBeRemoved);
  }
  else if (PluginView* pluginView = qobject_cast<PluginView*>(page))
  {
    Q_UNUSED(pluginView);
  }
  else if (QPlainTextEdit* textView = qobject_cast<QPlainTextEdit*>(page))
  {
    LOG_DEBUG("Closing editor tab");
    Q_UNUSED(textView);
  }
  else if (ProcessTab* processView = qobject_cast<ProcessTab*>(page))
  {
    processView->hide();
  }

  mEditorWorkspace->closeEditor(editorId);
}

void MainWindow::onOpenFlow(std::shared_ptr<FlowSaveInfo> flow, const QString& nodeId, const maki::FocusProperties& properties)
{
  if (!flow)
  {
    if (!mPropertiesMenu)
    {
      LOG_WARNING("System not initialized");
      return;
    }

    auto node = rootCanvas()->findNodeWithId(nodeId);
    if (!node)
    {
      LOG_WARNING("Could not find task node with id: {}", nodeId);
      return;
    }

    const auto ret = mPropertiesMenu->onCreateEvent(node, true);
    if (!ret.IsSuccess())
    {
      LOG_WARNING("Could not create flow for node {}: {}", nodeId, ret.ErrorMessage());
      return;
    }

    flow = ret.Value();
  }

  if (!flow)
  {
    LOG_INFO("No name provided, skipping flow creation");
    return;
  }

  // Check if the flow is already open in some tab
  const QString tabId = QString::fromStdString(std::format("{}_{}", flow->getid(), nodeId));
  for (const auto* editor : mEditorWorkspace->editors())
  {
    auto editorId = mEditorWorkspace->editorId(editor);
    LOG_DEBUG("Trying to open: {} vs {}", editorId, tabId);
    if (editorId != tabId)
      continue;

    mEditorWorkspace->activateEditor(editorId);
    return;
  }

  CanvasView* newView = new CanvasView(mEditorWorkspace);
  BehaviourCanvas* behaviourCanvas = new BehaviourCanvas(flow, nodeId, mRouter, newView);
  bindCanvas(behaviourCanvas);
  behaviourCanvas->setupInitialNodes();
  newView->setScene(behaviourCanvas);

  // Change to respective tabs
  auto index = libraryTypeToIndex(behaviourCanvas->type());
  mPalette->setCurrentIndex(index);

  mEditorWorkspace->addEditor(tabId, newView, flow->getname(), QIcon(":/icons/behaviour.svg"), true);
  mEditorWorkspace->activateEditor(tabId);

  // Populate after creation
  behaviourCanvas->populate(*flow);
}

void MainWindow::focusOn(const QString& nodeId, const QString& flowId, const maki::FocusProperties& properties)
{
  LOG_DEBUG("focusOn with node: {} and flow: {}, reason: {}", nodeId, flowId, (int)properties.reason);
  // If we are focusing on a behaviour node
  if (!nodeId.isEmpty() && !flowId.isEmpty())
  {
    auto [node, flow] = rootCanvas()->getFlow(flowId);
    if (!flow)
    {
      LOG_WARNING("Could not find flow with id: {}", flowId);
      return;
    }

    onOpenFlow(flow->config(), node->id(), properties);

    const QString tabId = QString::fromStdString(std::format("{}_{}", flowId, nodeId));
    if (const auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(tabId)))
      qobject_cast<Canvas*>(view->scene())->focusOnNode(nodeId, properties);
  }
  // If we are focusing on a structural node
  else if (flowId.isEmpty() && !nodeId.isEmpty())
  {
    if (properties.reason == maki::FocusReason::SIMULATION)
    {
      const auto flow = rootCanvas()->getFlowWithNode(nodeId);
      if (!flow)
        return;

      onOpenFlow(flow->config(), nodeId, properties);

      const QString tabId = QString::fromStdString(std::format("{}_{}", flow->id(), nodeId));
      if (const auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(tabId)))
        qobject_cast<Canvas*>(view->scene())->focusOnNode(nodeId, properties);
    }
    else
    {
      // mCanvasPanel->setCurrentIndex(0);
      mEditorWorkspace->activateEditor(Config::MAIN_CANVAS);
      rootCanvas()->focusOnNode(nodeId, properties);
    }
  }
  // If we are focusing on a flow
  else if (!flowId.isEmpty() && nodeId.isEmpty())
  {
    auto [node, flow] = rootCanvas()->getFlow(flowId);
    if (!flow)
      return;

    onOpenFlow(flow->config(), node->id(), properties);

    const QString tabId = QString::fromStdString(std::format("{}_{}", flow->id(), node->id()));
    if (const auto* view = qobject_cast<CanvasView*>(mEditorWorkspace->findEditor(tabId)))
      qobject_cast<Canvas*>(view->scene())->focusOnNode(nodeId, properties);
  }
}

void MainWindow::addPluginTab(const QString& name, PluginView* view)
{
  mEditorWorkspace->addEditor(name, view, name, QIcon(":/icons/plugin.svg"), true);
  mEditorWorkspace->activateEditor(name);
}

void MainWindow::removePluginTab(PluginView* view)
{
  closeCanvasTab(view);
}

void MainWindow::addEditorTab(QPlainTextEdit* editorTab)
{
  for (auto* editor : mEditorWorkspace->editors())
  {
    if (auto* textEditor = qobject_cast<QPlainTextEdit*>(editor))
    {
      const auto editorId = mEditorWorkspace->editorId(editor);
      textEditor->setPlainText(editorTab->toPlainText());
      mEditorWorkspace->setEditorTitle(editorId, editorTab->accessibleName());
      mEditorWorkspace->activateEditor(editorId);
      return;
    }
  }

  mEditorWorkspace->addEditor("text_editor", editorTab, editorTab->accessibleName(), QIcon(":/icons/file.svg"));
  mEditorWorkspace->activateEditor("text_editor");
}

void MainWindow::onFlowAdded(PFlow flow, NodeItem* node)
{
  LOG_WARN_ON_FAILURE(mSystemMenu->onFlowAdded(flow, node));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onFlowAdded(flow, node));
}

void MainWindow::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  const QString tabId = QString::fromStdString(std::format("{}_{}", flowId, node ? node->id() : ""));
  mEditorWorkspace->closeEditor(tabId);

  LOG_WARN_ON_FAILURE(mSystemMenu->onFlowRemoved(flowId, node->id()));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onFlowRemoved(flowId, node));
}

int MainWindow::libraryTypeToIndex(Types::LibraryTypes type) const
{
  switch (type)
  {
    case Types::LibraryTypes::STRUCTURAL:
      return 0;
    case Types::LibraryTypes::BEHAVIOUR:
      return 1;
    case Types::LibraryTypes::PIPELINE:
      return 2;
    default:
      LOG_ERROR("Unknown library type");
      return 0;
  }
}

void MainWindow::showAboutDialog()
{
  oclero::qlementine::AboutDialog dialog(this);

  dialog.setIcon(QApplication::windowIcon());
  dialog.setApplicationName(QApplication::applicationName());
  dialog.setApplicationVersion(QApplication::applicationVersion());

  dialog.setDescription("A low-code development platform for modelling, orchestrating, "
                        "simulating, and verifying component-based systems.");

  dialog.setWebsiteUrl(Config::MAKI_WEBSITE);
  dialog.setLicense("MIT License");
  dialog.setCopyright("© 2026 Felipe Xavier");

  dialog.addSocialMediaLink("GitHub", "https://github.com/FelipeACXavier", QIcon(":/icons/github.svg"));
  dialog.addSocialMediaLink("Research", "https://research.tue.nl/nl/persons/felipe-de-azeredo-coutinho-xavier/", QIcon(":/icons/research.svg"));
  dialog.addSocialMediaLink("Website", "https://felipeacxavier.github.io", QIcon(":/icons/me.svg"));

  dialog.exec();
}

VoidResult MainWindow::loadLastSession()
{
  if (!mSettingsManager || !mSettingsManager->general().restoreLastSession)
    return VoidResult();

  if (mSettingsManager->general().recentFiles.empty())
    return VoidResult::Failed("No file to load");

  const auto toLoad = mSettingsManager->general().recentFiles.front();
  QFileInfo info(toLoad);
  if (!info.exists())
    return VoidResult::Failed("Project file {} does not exist", toLoad);

  onActionLoad(toLoad);

  return VoidResult();
}
