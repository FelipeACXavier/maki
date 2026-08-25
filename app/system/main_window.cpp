#include "main_window.h"

#include <QComboBox>
#include <QDrag>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QScrollArea>
#include <QShortcut>
#include <QStackedWidget>
#include <QString>
#include <QTextBlock>
#include <QTextBrowser>
#include <QUndoGroup>
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
#include "compiler/generator.h"
#include "compiler/pipeline.h"
#include "compiler/pipeline_graph.h"
#include "compiler/plugin_action_registry.h"
#include "compiler/plugin_pipeline.h"
#include "config.h"
#include "data_flow_model.h"
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
#include "type_helpers.h"
#include "type_registry.h"
#include "types.h"
#include "widgets/dialogs/prompt.h"
#include "widgets/dialogs/text_prompt.h"
#include "widgets/dropdown_button.h"
#include "widgets/language_manager.h"
#include "widgets/log_table_widget.h"
#include "widgets/notification_manager.h"
#include "widgets/properties/mission_parameter_widget.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"
#include "widgets/settings_dialog.h"
#include "widgets/settings_manager.h"
#include "widgets/structure/file_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/system_menu.h"
#include "widgets/type_editor.h"

MainWindow::MainWindow(QApplication* app, oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
    : MainWindowLayout(parent)
    , mActiveCanvas(nullptr)
    , mThemeManager(themeManager)
    , mApp(app)
{
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  logging::gSilentLog = false;
  logging::gMinLogLevel = logging::LogLevel::Debugging;
  logging::gSourceName = Config::APPLICATION_NAME.toStdString();
  logging::gLogToStream = [this](std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& source,
                                 const std::string& filename, const uint32_t& line, const std::string& message) {
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

  mConfigTable = std::make_shared<ConfigurationTable>();
  mStorage = std::make_shared<SaveInfo>();

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
  mNotificationManager = new NotificationManager(mCanvasPanel);  // TODO(felaze): Check leaks here
  mPluginManager = std::make_unique<PluginManager>(mPluginPipeline->registry(), mPipeline, this);

  mGenerator = new Generator(mPipeline, this);
  mPluginTab = new PluginTab(mSpecialTabsMenu, this);

  mMissionParameters->setStorage(mStorage);

  mHostServices = new HostServices(mStorage, mPipeline, mSettingsManager.get(), QCoreApplication::applicationDirPath(), this);
  mHostServices->setPluginTab(mPluginTab);
  mHostServices->setLogger(mLogger);

  mProcessTab->addPipeline(mPipeline);
  mProcessTab->addPipeline(processPipeline);

  startUI();
  bind();
  bindShortcuts();

  mPropertiesMenu->start(mStorage);

  if (mPipelineRun)
    for (const auto& pipeline : mStorage->pipelines())
      mPipelineRun->addOption(pipeline->getname());

  // Set initial tabs
  mPalette->setCurrentIndex(0);
  mNavigationTab->setCurrentIndex(0);
  mPropertiesTab->setCurrentIndex(0);

  if (mSettingsManager)
  {
    mFileMenu->setGenerationRoot(mSettingsManager->generation().generationDir);
    mLanguageManager->setLanguage(mSettingsManager->general().language);
    mSaveHandler->setLastDir(mSettingsManager->general().lastOpenFileDir);
    mRouter->setRouteOption((EdgeRouter::Option)mSettingsManager->appearance().edgeShape);

    for (const auto& file : mSettingsManager->general().recentFiles)
    {
      QAction* action = mActionOpenRecent->addAction(elideLeft(file, mActionOpenRecent));
      connect(action, &QAction::triggered, [this, file] { onActionLoad(file); });
    }
    connect(mSettingsManager.get(), &SettingsManager::settingsChanged, this, &MainWindow::onSettingsChanged);

    if (!mSettingsManager->general().showWelcomeMessage)
      mInfoText->clear();

    if (mSettingsManager->appearance().startLogFilterExpanded)
      mLogTable->showAll();

    onThemeChanged(mSettingsManager->appearance(), true);
  }

  // Load elements after the system started
  QTimer::singleShot(0, this, [this]() {
    if (mPluginManager && mSettingsManager)
      LOG_WARN_ON_FAILURE(mPluginManager->start(mSettingsManager->plugins(), mHostServices));

    LOG_WARN_ON_FAILURE(loadElements());
  });

  LOG_DEBUG("Main window started");

  return VoidResult();
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
  QList<SectionWidget*> sections = mStructureTab->findChildren<SectionWidget*>();
  for (const auto& section : sections)
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      qobject_cast<LibraryScene*>(library->scene())->themeChanged();

  // Update all items in all canvases
  for (int i = 0; i < mCanvasPanel->count(); ++i)
  {
    QWidget* w = mCanvasPanel->widget(i);
    if (auto canvas = qobject_cast<CanvasView*>(w))
    {
      canvas->onSettingsChanged(settings);
      static_cast<Canvas*>(canvas->scene())->themeChanged();
    }
  }

  if (mPluginTab)
    mPluginTab->onThemeChanged();

  auto menuBarChanged = mMenuBar->isNativeMenuBar() != settings.nativeMenuBar;
  mMenuBar->setNativeMenuBar(settings.nativeMenuBar);
  if (!initialConfig && menuBarChanged)
  {
    if (maki::confirmationPrompt("A full restart is required to for the menu bar to be updated", "Restart now", "Restart later"))
      onActionRestart();
  }
}

void MainWindow::onSettingsChanged()
{
  LOG_DEBUG("Settings changed");

  onThemeChanged(mSettingsManager->appearance(), false);

  if (mFileMenu)
    mFileMenu->setGenerationRoot(mSettingsManager->generation().generationDir);

  if (mLanguageManager)
    mLanguageManager->setLanguage(mSettingsManager->general().language);

  if (mSettingsManager->general().enableDebugLogs)
    logging::gMinLogLevel = logging::LogLevel::Trace;

  if (mPluginManager)
    mPluginManager->settingsChanged(mSettingsManager->plugins(), mHostServices);

  if (mRouter)
    mRouter->setRouteOption((EdgeRouter::Option)mSettingsManager->appearance().edgeShape);

  if (mProcessTab)
    mProcessTab->setMergedLogs(mSettingsManager->plugins().showCombinedLogs);

  // Clean and repopulate the recent files
  mActionOpenRecent->clear();
  for (const auto& file : mSettingsManager->general().recentFiles)
  {
    QAction* action = mActionOpenRecent->addAction(elideLeft(file, mActionOpenRecent));
    connect(action, &QAction::triggered, [this, file] { onActionLoad(file); });
  }

  // Update node palette
  for (const auto& section : mStructureTab->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      library->setColumnCount(mSettingsManager->appearance().numberOfColumns);

  for (const auto& section : mBehaviourTab->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      library->setColumnCount(mSettingsManager->appearance().numberOfColumns);

  for (const auto& section : mPipelineTab->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      library->setColumnCount(mSettingsManager->appearance().numberOfColumns);
}

void MainWindow::startUI()
{
  CanvasView* currentCanvas = static_cast<CanvasView*>(mCanvasPanel->currentWidget());
  StructureCanvas* canvas = new StructureCanvas(mStorage, Config::MAIN_CANVAS, mConfigTable, mRouter, currentCanvas);

  mActiveCanvas = canvas;
  currentCanvas->setScene(canvas);

  mUndoGroup->addStack(canvas->undoStack());
  mUndoGroup->setActiveStack(canvas->undoStack());
}

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

    LOG_INFO("Focused on: {}", fw->metaObject()->className());

    // 1) If focus is in the node library panel -> search there
    if (QScrollArea* lib = qobject_cast<QScrollArea*>(findAncestor(fw, &QScrollArea::staticMetaObject)))
    {
      // For now, we only search in the structure tab
      if (lib == mStructureScrollArea)
      {
        LOG_DEBUG("Finding in palette");
        mPaletteSearch->show();
        mPaletteSearch->widget()->setFocus(Qt::ShortcutFocusReason);
        return;
      }
    }
    if (auto* search = qobject_cast<maki::SearchWidget*>(findAncestor(fw, &maki::SearchWidget::staticMetaObject)))
    {
      if (search == mPaletteSearch)
      {
        LOG_DEBUG("Finding in palette");
        mPaletteSearch->show();
        mPaletteSearch->widget()->setFocus(Qt::ShortcutFocusReason);
      }
      return;
    }

    // 2) If focus is in the canvas -> search there
    if (auto* canvas = qobject_cast<CanvasView*>(findAncestor(fw, &CanvasView::staticMetaObject)))
    {
      Q_UNUSED(canvas);
      LOG_DEBUG("Finding in canvas");
      return;
    }

    if (auto* bottom = qobject_cast<QSplitter*>(findAncestor(fw, &QSplitter::staticMetaObject)))
    {
      LOG_DEBUG("Finding in bottom container");
      if (bottom == mCentralSplitter && mBottomPanel->currentIndex() == 1)
        mLogTable->search();
      return;
    }
  });

  // File actions =============================================================
  connect(mActionNew, &QAction::triggered, this, &MainWindow::onActionNew);
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

  // Setting actions =============================================================
  connect(mOpenAllSettings, &QAction::triggered, this, [this] {
    LOG_INFO("Opening all settings");
    SettingsDialog* settingsDialog = new SettingsDialog(tr("Settings"), mSettingsManager, mLanguageManager, this);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->show();
  });
  mOpenAllSettings->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));

  connect(mAboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

  // Internal actions =============================================================
  connect(mCanvasPanel, &QTabWidget::currentChanged, this, &MainWindow::onCanvasTabChanged);
  connect(mCanvasPanel, &QTabWidget::tabCloseRequested, this, &MainWindow::closeCanvasTab);

  connect(rootCanvas(), &Canvas::openFlow, this, &MainWindow::onOpenFlow);
  connect(rootCanvas(), &Canvas::flowAdded, this, &MainWindow::onFlowAdded);
  connect(rootCanvas(), &Canvas::flowRemoved, this, &MainWindow::onFlowRemoved);

  connect(mPropertiesMenu, &PropertiesMenu::flowSelected, rootCanvas(), &Canvas::onFlowSelected);
  connect(mHostServices, &HostServices::onFocusOnNode, this,
          [this](const QString& nodeId, const QString& flowId, const maki::FocusProperties& properties) {
            rootCanvas()->onFocusNode(flowId, nodeId, properties);
          });

  connect(mSystemMenu, &SystemMenu::flowSelected, rootCanvas(), &Canvas::onFlowSelected);
  connect(mSystemMenu, &SystemMenu::flowRemoved, rootCanvas(), &Canvas::onFlowRemoved);
  connect(mSystemMenu, &SystemMenu::nodeRemoved, rootCanvas(), &Canvas::onRemoveNode);
  connect(mSystemMenu, &SystemMenu::nodeFocused, this,
          [this](const QString& nodeId, const QString& flowId) { rootCanvas()->onFocusNode(flowId, nodeId, maki::FocusProperties::internal()); });

  connect(mPropertiesMenu, &PropertiesMenu::flowRemoved, rootCanvas(), &Canvas::onFlowRemoved);
  connect(mPropertiesMenu, &PropertiesMenu::openParameter, this, [this](const QString& parameterId) {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(2);

    if (mMissionParameters)
      mMissionParameters->editParameter(parameterId);
  });

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
      body["backgroundColor"] = "#202020";
      body["borderColor"] = "#FFFFFF";
      body["borderRadius"] = 10;
      body["width"] = 100;
      body["height"] = 100;
      body["iconColor"] = "#FFFFFF";
      body["iconScale"] = 0.8;
      body["nodeSvg"] = plugin.plugin->manifest().iconPath();

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
    {
      if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      {
        const bool hasMatches = library->filterNodes(query);
        section->setExpanded(hasMatches || query.isEmpty());
      }
    }
  });

  // Type Editor stuff =============================================================
  // if (mTypeEditor)
  // {
  //   connect(mTypeEditor->mRecordAction, &QAction::triggered, this, [this] {
  //     if (mBottomNavigation)
  //       mBottomNavigation->setCurrentIndex(3);
  //   });
  //   connect(mTypeEditor->mAliasAction, &QAction::triggered, this, [this] {
  //     if (mBottomNavigation)
  //       mBottomNavigation->setCurrentIndex(3);
  //   });
  //   connect(mTypeEditor->mEnumAction, &QAction::triggered, this, [this] {
  //     if (mBottomNavigation)
  //       mBottomNavigation->setCurrentIndex(3);
  //   });
  // }

  // Canvas stuff =============================================================
  bindCanvas();
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

void MainWindow::bindCanvas()
{
  connect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  connect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  connect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  connect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);

  connect(canvas(), &Canvas::createEvent, mPropertiesMenu, &PropertiesMenu::onCreateEvent);
  connect(canvas(), &Canvas::transitionSelected, mPropertiesMenu, &PropertiesMenu::onTransitionSelected);
}

void MainWindow::unbindCanvas()
{
  disconnect(canvas(), &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  disconnect(canvas(), &Canvas::nodeAdded, this, &MainWindow::onNodeAdded);
  disconnect(canvas(), &Canvas::nodeRemoved, this, &MainWindow::onNodeRemoved);
  disconnect(canvas(), &Canvas::nodeModified, this, &MainWindow::onNodeModified);

  disconnect(canvas(), &Canvas::createEvent, mPropertiesMenu, &PropertiesMenu::onCreateEvent);
  disconnect(canvas(), &Canvas::transitionSelected, mPropertiesMenu, &PropertiesMenu::onTransitionSelected);
}

void MainWindow::bindShortcuts()
{
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this, [] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Copy, focused on: {}", fw->metaObject()->className());
    if (auto* textEdit = qobject_cast<QTextEdit*>(findAncestor(fw, &QTextEdit::staticMetaObject)))
    {
      textEdit->copy();
      return;
    }
    else if (auto* browser = qobject_cast<QTextBrowser*>(findAncestor(fw, &QTextBrowser::staticMetaObject)))
    {
      browser->copy();
      return;
    }
    else if (auto* canvasView = qobject_cast<CanvasView*>(findAncestor(fw, &CanvasView::staticMetaObject)))
    {
      if (auto* canvas = qobject_cast<Canvas*>(canvasView->scene()))
        canvas->copySelectedItems(nullptr);

      return;
    }
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this, [] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Copy, focused on: {}", fw->metaObject()->className());

    // 1) If focus is in the node library panel -> search there
    if (auto* textEdit = qobject_cast<QTextEdit*>(findAncestor(fw, &QTextEdit::staticMetaObject)))
    {
      if (!textEdit->isReadOnly())
        textEdit->paste();

      return;
    }
    else if (auto* browser = qobject_cast<QTextBrowser*>(findAncestor(fw, &QTextBrowser::staticMetaObject)))
    {
      if (!browser->isReadOnly())
        browser->paste();

      return;
    }
    else if (auto* canvasView = qobject_cast<CanvasView*>(findAncestor(fw, &CanvasView::staticMetaObject)))
    {
      if (auto* canvas = qobject_cast<Canvas*>(canvasView->scene()))
        canvas->pasteCopiedItems();

      return;
    }
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Ctrl, tab, focused on: {}", fw->metaObject()->className());
    if (auto* stackedWidget = qobject_cast<QStackedWidget*>(findAncestor(fw, &QStackedWidget::staticMetaObject)))
    {
      if (stackedWidget == mBottomPanel && mBottomNavigation)
      {
        int next = mBottomNavigation->currentIndex() - 1;
        mBottomNavigation->setCurrentIndex(next < 0 ? mBottomNavigation->itemCount() - 1 : next);
        return;
      }
    }
    else if (auto* widget = qobject_cast<oclero::qlementine::AbstractItemListWidget*>(
                 findAncestor(fw, &oclero::qlementine::AbstractItemListWidget::staticMetaObject)))
    {
      if (widget == mBottomNavigation)
      {
        int next = mBottomNavigation->currentIndex() - 1;
        mBottomNavigation->setCurrentIndex(next < 0 ? mBottomNavigation->itemCount() - 1 : next);
        return;
      }
    }
    else if (auto* tabWidget = qobject_cast<QTabWidget*>(findAncestor(fw, &QTabWidget::staticMetaObject)))
    {
      if (tabWidget == mCanvasPanel)
      {
        int next = mCanvasPanel->currentIndex() - 1;
        mCanvasPanel->setCurrentIndex(next < 0 ? mCanvasPanel->count() - 1 : next);
        return;
      }
    }
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab), this, [this] {
    QWidget* fw = QApplication::focusWidget();
    if (!fw)
      return;

    LOG_TRACE("Ctrl, shift, tab, focused on: {}", fw->metaObject()->className());
    if (auto* stackedWidget = qobject_cast<QStackedWidget*>(findAncestor(fw, &QStackedWidget::staticMetaObject)))
    {
      if (stackedWidget == mBottomPanel && mBottomNavigation)
      {
        int next = mBottomNavigation->currentIndex() + 1;
        mBottomNavigation->setCurrentIndex(next >= mBottomNavigation->itemCount() ? 0 : next);
        return;
      }
    }
    else if (auto* widget = qobject_cast<oclero::qlementine::AbstractItemListWidget*>(
                 findAncestor(fw, &oclero::qlementine::AbstractItemListWidget::staticMetaObject)))
    {
      if (widget == mBottomNavigation)
      {
        int next = mBottomNavigation->currentIndex() + 1;
        mBottomNavigation->setCurrentIndex(next >= mBottomNavigation->itemCount() ? 0 : next);
        return;
      }
    }
    else if (auto* tabWidget = qobject_cast<QTabWidget*>(findAncestor(fw, &QTabWidget::staticMetaObject)))
    {
      if (tabWidget == mCanvasPanel)
      {
        int next = mCanvasPanel->currentIndex() + 1;
        mCanvasPanel->setCurrentIndex(next >= mCanvasPanel->count() ? 0 : next);
        return;
      }
    }
  });
  new QShortcut(QKeySequence(Qt::Key_Delete), this, [this] {
    if (canvas())
      canvas()->deleteSelectedItems();
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::ALT | Qt::Key_R), this, [this] {
    if (mPluginManager && mPluginManager->currentPlugin())
      LOG_WARN_ON_FAILURE(mPluginManager->reloadPlugin(mPluginManager->currentPlugin()->languageName(), mHostServices));
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(2);

    if (mMissionParameters)
      mMissionParameters->addParameter();
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(1);

    if (mTypeEditor)
    {
      mTypeEditor->createRecord();
      mTypeEditor->focusCurrentEditor();
    }
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(1);

    if (mTypeEditor)
    {
      mTypeEditor->createAlias();
      mTypeEditor->focusCurrentEditor();
    }
  });
  new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E), this, [this] {
    if (mBottomNavigation)
      mBottomNavigation->setCurrentIndex(1);

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
  auto view = static_cast<CanvasView*>(mCanvasPanel->widget(0));
  return static_cast<Canvas*>(view->scene());
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
  LibraryScene* sidebarScene = dynamic_cast<LibraryScene*>(sidebarview->scene());
  connect(sidebarScene, &LibraryScene::libraryNodeSelected, [this](const QString& nodeType) {
    if (auto info = mConfigTable->get(nodeType))
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
    sidebarview->addNode(nodeId, nodeConfig);
    nodeConfig->type = nodeId;

    LOG_TRACE("Adding key: {} to the config table", nodeId);
    LOG_ERROR_ON_FAILURE(mConfigTable->add(nodeId, nodeConfig));
    LOG_ERROR_ON_FAILURE(maki::TypeRegistry::instance().registerNode(nodeId, NodeSaveInfo(*nodeConfig)));
  }

  return VoidResult();
}

void MainWindow::onActionNew()
{
  if (!mSaveHandler)
  {
    LOG_WARNING("System not initialized");
    return;
  }

  // Repopulate the canvas
  SaveInfo emptySave;
  *mStorage = emptySave;

  // Gotta make sure we don't save over an old file
  mSaveHandler->newFileCreated();

  maki::TypeRegistry::instance().removeUserTypes();
  canvas()->loadFromSave(emptySave);
  if (mMissionParameters)
    mMissionParameters->setStorage(mStorage);
  if (mPipelineRun)
    mPipelineRun->reset();

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

void MainWindow::onActionGenerate(const QString& pipelineId)
{
  if (!mPluginPipeline)
    LOG_WARNING("No pipeline available");

  // If we are running, then we should cancel
  if (mPluginPipeline->isRunning())
  {
    QTimer::singleShot(0, [this] { LOG_WARN_ON_FAILURE(mPluginPipeline->abort()); });
    return;
  }

  // We should have three buttons:
  // - Verify
  // - Simulate
  // - Deploy
  // Each has a different pipeline graph
  // We no longer select plugins, we just modify the pipeline directly
  // This is similar to the eclipse run configuration
  QByteArray byteArray;
  QDataStream out(&byteArray, QIODevice::WriteOnly);
  out << mHostServices->document()->getnodes();

  if (mStorage->name.isEmpty() && !onActionSave().IsSuccess())
    return;

  maki::PipelineContext context;
  context.buildDir = QDir(mSettingsManager->generation().generationDir + "/" + mStorage->name);
  context.projectDir = QDir(mSettingsManager->generation().generationDir + "/" + mStorage->name);
  context.addArtifact({
      .id = "maki.nodes",
      .type = "maki",
      .producer = "MAKI",
      .paths =
          {
              {"nodes", byteArray.toBase64()},
          },
  });

  // Make sure the project exists
  if (!context.buildDir.exists())
    context.buildDir.mkpath(".");

  for (const auto& pipeline : mStorage->pipelines())
  {
    if (pipeline->getname() != pipelineId)
      continue;

    auto graph = maki::PipelineGraph::fromFlow(*pipeline);
    if (!graph.IsSuccess())
    {
      LOG_ERROR(graph.ErrorMessage());
      return;
    }

    graph.Value().print();

    LOG_ERROR_ON_FAILURE(mPluginPipeline->run(graph.Value(), context));
    return;
  }

  // We tried running an unknown pipeline, so we can create it
  onActionEditPipeline(pipelineId);
}

void MainWindow::onActionDeletePipeline(const QString& pipelineId)
{
  if (!mPluginPipeline)
    LOG_WARNING("No pipeline available");

  QString pipelineName;
  std::shared_ptr<FlowSaveInfo> pipeline;
  for (const auto& p : mStorage->pipelines())
  {
    if (p->getname() != pipelineId)
      continue;

    pipelineName = pipelineId;
    pipeline = p;
    break;
  }

  // Check if the pipeline is currently open
  for (int i = 1; i < mCanvasPanel->count(); ++i)
  {
    auto prop = mCanvasPanel->widget(i)->property("id");
    if (!prop.isValid() || prop.toString() != pipelineName)
      continue;

    // Change to the previous tab and remove the pipeline tab
    mCanvasPanel->setCurrentIndex(qMax<int>(0, i - 1));
    mCanvasPanel->removeTab(i);
    break;
  }

  mStorage->removePipeline(pipeline);
  mPipelineRun->removeOption(pipelineName);  // Update the UI as well
}

void MainWindow::onActionEditPipeline(const QString& pipelineId)
{
  QString pipelineName;
  std::shared_ptr<FlowSaveInfo> pipeline;
  for (const auto& p : mStorage->pipelines())
  {
    if (p->getname() != pipelineId)
      continue;

    pipelineName = pipelineId;
    pipeline = p;
    break;
  }

  // If we are dealing with an unknown or new pipeline, we must create it
  if (pipelineName.isEmpty() || !pipeline)
  {
    FlowSaveInfo info;
    auto name = maki::textPrompt(tr("Pipeline name"), tr("Choose a pipeline name"), this);
    if (name.isEmpty())
      return;

    pipelineName = name;
    info.setName(pipelineName);
    pipeline = std::make_shared<FlowSaveInfo>(info);

    mStorage->addPipeline(pipeline);
    mPipelineRun->addOption(pipelineName);  // Update the UI as well
  }

  // Check if the pipeline is already open
  for (int i = 1; i < mCanvasPanel->count(); ++i)
  {
    auto prop = mCanvasPanel->widget(i)->property("id");
    if (prop.isValid() && prop.toString() == pipelineName)
    {
      mCanvasPanel->setCurrentIndex(i);
      return;
    }
  }

  CanvasView* newView = new CanvasView(mCanvasPanel);
  newView->setProperty("id", pipelineName);

  PipelineCanvas* canvas = new PipelineCanvas(pipeline, mConfigTable, mRouter, newView);
  newView->setScene(canvas);
  canvas->populate(*pipeline);

  // Hide all tabs in the palette but the pipeline one
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::STRUCTURAL), false);
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::BEHAVIOUR), false);
  mPalette->setTabVisible(libraryTypeToIndex(Types::LibraryTypes::PIPELINE), true);
  mPalette->setCurrentIndex(libraryTypeToIndex(Types::LibraryTypes::PIPELINE));

  mCanvasPanel->addTab(newView, QIcon(":/icons/deploy.svg"), pipeline->getname());
  mCanvasPanel->setCurrentWidget(newView);
}

VoidResult MainWindow::onActionSave()
{
  if (!mSaveHandler)
    return VoidResult::Failed("System not initialized");

  if (CanvasView* view = qobject_cast<CanvasView*>(mCanvasPanel->widget(0)))
  {
    if (auto* canvas = qobject_cast<Canvas*>(view->scene()))
    {
      CanvasSaveInfo info;
      info.setCenter(canvas->getCenter());
      info.setScale(canvas->getScale());
      mStorage->setCanvasInfo(info);
    }
  }

  auto saved = mSaveHandler->saveProject(*mStorage);
  if (saved)
  {
    NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Saved project: {}", mStorage->name.toStdString());
  }
  else
  {
    LOG_ERROR(saved.ErrorMessage());
    NOTIFY_ERROR(Config::APPLICATION_NAME.toStdString(), "Could not save project: {}\n{}", mStorage->name.toStdString(), saved.ErrorMessage());
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

  auto saved = mSaveHandler->saveProjectAs(*mStorage);
  if (saved)
  {
    NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Saved project: {}", mStorage->name.toStdString());
  }
  else
  {
    LOG_ERROR(saved.ErrorMessage());
    NOTIFY_ERROR(Config::APPLICATION_NAME.toStdString(), "Could not save project: {}\n{}", mStorage->name.toStdString(), saved.ErrorMessage());
  }
}

void MainWindow::onActionLoad(const QString& filename)
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
    onFileLoaded(filename, SaveInfo(), QString::fromStdString(loaded.ErrorMessage()));
    return;
  }

  auto info = loaded.Value();
  onFileLoaded(filename, info, "");
}

void MainWindow::onFileLoaded(const QString& file, const SaveInfo& info, const QString& error)
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
  for (int i = 1; i < mCanvasPanel->count(); ++i)
    closeCanvasTab(i);

  // Force the structural canvas
  mCanvasPanel->setCurrentIndex(0);

  // Clear the storage so it can be populated by the canvas
  *mStorage = info;
  mStorage->clearNodes();

  // Repopulate the canvas (and the storage)
  canvas()->loadFromSave(info);

  if (mMissionParameters)
    mMissionParameters->setStorage(mStorage);

  GeneralSettings general = mSettingsManager->general();
  general.lastOpenFileDir = mSaveHandler->lastDir();
  mSettingsManager->setGeneral(general);
  mSettingsManager->applySettings();

  if (mPipelineRun)
  {
    mPipelineRun->reset();
    for (const auto& pipeline : mStorage->pipelines())
      mPipelineRun->addOption(pipeline->getname());
  }

  if (mSystemMenu)
    mSystemMenu->expandToDepth(1);

  LOG_INFO("Project with {} nodes after", mStorage->getnodes().size());
  NOTIFY_INFO(Config::APPLICATION_NAME.toStdString(), "Loaded project: {}", mStorage->name.toStdString());
}

void MainWindow::onNodeSelected(NodeItem* node, bool selected)
{
  if (node)
  {
    if (auto info = mConfigTable->get(node->nodeType()))
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

void MainWindow::suggestCapability(NodeItem* node)
{
  if (node->config()->libraryType != Types::LibraryTypes::STRUCTURAL)
    return;

  // We don't need to check tasks
  if (!node->parentNode())
    return;
  if (!mCanvasPanel)
    return;
  if (mCanvasPanel->currentIndex() != libraryTypeToIndex(Types::LibraryTypes::STRUCTURAL))
    return;

  LOG_DEBUG("Looking for suggestions for {}", node->nodeName());
  const auto config = mConfigTable->get(node->nodeType());
  if (!config)
    return;

  QStringList producers;
  QStringList consumers;

  for (const auto& event : config->events)
  {
    for (const auto& argument : event.arguments)
    {
      const maki::DataPort port{.nodeId = node->nodeId(), .eventName = event.name, .argumentName = argument.id};

      if (event.type == Types::CallType::TRIGGER)
      {
        // This concrete input is already connected to some DataEntry.
        if (maki::DataFlowModel::instance().entryConsumedBy(port).has_value())
          continue;

        // The type comes from the capability definition.
        for (const auto& producer : maki::TypeRegistry::instance().findProducers(argument.type))
          if (!mStorage->taskHasCapability(node->parentNode()->id(), producer))
            producers << producer;
      }
      else if (event.type == Types::CallType::RETURN)
      {
        const auto entryId = maki::DataFlowModel::instance().entryProducedBy(port);

        // If this output already has a DataEntry and that entry
        // already has consumers, don't suggest more consumers.
        if (entryId)
        {
          const auto* entry = maki::DataFlowModel::instance().get(*entryId);
          if (entry && !entry->consumers.empty())
            continue;
        }

        for (const auto& consumer : maki::TypeRegistry::instance().findConsumers(argument.type))
          if (!mStorage->taskHasCapability(node->parentNode()->id(), consumer))
            consumers << consumer;
      }
    }
  }

  producers.removeDuplicates();
  consumers.removeDuplicates();

  for (const auto& producer : producers)
    LOG_DEBUG("Producer suggestion: {}", producer);
  for (const auto& consumer : consumers)
    LOG_DEBUG("Consumer suggestion: {}", consumer);

  if (producers.isEmpty() && consumers.isEmpty())
    return;

  if (auto* view = qobject_cast<CanvasView*>(mCanvasPanel->currentWidget()))
    if (auto* canvas = qobject_cast<Canvas*>(view->scene()))
      canvas->suggestedNodes(node, consumers, producers);
}

void MainWindow::onNodeAdded(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was added but no node was provided");
    return;
  }

  if (canvas()->type() != Types::LibraryTypes::PIPELINE)
    LOG_WARN_ON_FAILURE(mSystemMenu->onNodeAdded(canvas()->id(), node));

  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeAdded(node));

  suggestCapability(node);
}

void MainWindow::onNodeRemoved(const QString& nodeId, const QString& parentId)
{
  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeRemoved(canvas()->id(), nodeId, parentId));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onNodeRemoved(nodeId));
}

void MainWindow::onNodeModified(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was removed but no node was provided");
    return;
  }

  LOG_WARN_ON_FAILURE(mSystemMenu->onNodeModified(canvas()->id(), node));
}

void MainWindow::onCanvasTabChanged(int index)
{
  CanvasView* newCanvas = qobject_cast<CanvasView*>(mCanvasPanel->widget(index));
  if (!newCanvas)
    return;

  // Disconnect signals from the previous canvas
  if (canvas())
    unbindCanvas();

  mActiveCanvas = qobject_cast<Canvas*>(newCanvas->scene());
  if (!canvas())
    return;

  bindCanvas();

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

  NodeItem* toFocus = nullptr;
  for (const auto& item : mActiveCanvas->selectedItems())
    if (item->type() == NodeItem::Type)
    {
      toFocus = dynamic_cast<NodeItem*>(item);
      break;
    }

  mPropertiesMenu->onNodeSelected(toFocus, toFocus != nullptr);
}

void MainWindow::closeCanvasTab(int index)
{
  if (CanvasView* closedCanvas = qobject_cast<CanvasView*>(mCanvasPanel->widget(index)))
  {
    auto toBeRemoved = qobject_cast<Canvas*>(closedCanvas->scene());
    mUndoGroup->removeStack(toBeRemoved->undoStack());

    if (closedCanvas->scene() == canvas())
    {
      unbindCanvas();

      if (index > 0)
      {
        // The previous tab becomes the new active tab
        if (auto* newCanvas = qobject_cast<CanvasView*>(mCanvasPanel->widget(index - 1)))
        {
          mActiveCanvas = qobject_cast<Canvas*>(newCanvas->scene());
          if (canvas())
          {
            bindCanvas();

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

            auto libIndex = libraryTypeToIndex(canvas()->type());
            mPalette->setCurrentIndex(libIndex);
          }
        }
      }
    }
  }
  // else if (PluginView* tab = qobject_cast<PluginView*>(mCanvasPanel->widget(index)))
  // {
  //   Q_UNUSED(tab);
  // }
  else if (QPlainTextEdit* tab = qobject_cast<QPlainTextEdit*>(mCanvasPanel->widget(index)))
  {
    LOG_DEBUG("Closing editor tab");
    Q_UNUSED(tab);
  }
  else if (ProcessTab* tab = qobject_cast<ProcessTab*>(mCanvasPanel->widget(index)))
  {
    tab->hide();
  }

  mCanvasPanel->removeTab(index);
}

void MainWindow::onOpenFlow(Flow* flow, const QString& nodeId, const maki::FocusProperties& properties)
{
  QString flowName;
  if (flow == nullptr)
  {
    QInputDialog* dialog = new QInputDialog(this);
    dialog->setWindowTitle(tr("Flow name"));
    dialog->setLabelText(tr("Enter a name for the new flow") + ":");
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

  for (int i = 1; i < mCanvasPanel->count() && flow != nullptr; ++i)
  {
    // Check if the flow is already open in some tab
    auto prop = mCanvasPanel->widget(i)->property("id");
    if (prop.isValid() && prop.toString() == flow->id())
    {
      mCanvasPanel->setCurrentIndex(i);
      // TODO: clean this up
      if (!nodeId.isEmpty())
        if (auto* view = qobject_cast<CanvasView*>(mCanvasPanel->currentWidget()))
          if (auto* canvas = qobject_cast<Canvas*>(view->scene()))
            canvas->onFocusNode("", nodeId, properties);
      return;
    }
  }

  if (flow == nullptr)
  {
    LOG_WARNING("This shouldn't happen, no flow was found");
    return;
  }

  CanvasView* newView = new CanvasView(mCanvasPanel);

  BehaviourCanvas* canvas = new BehaviourCanvas(flow, mConfigTable, mRouter, newView);
  canvas->setupInitialNodes();
  newView->setScene(canvas);

  // Change to respective tabs
  auto index = libraryTypeToIndex(canvas->type());
  mPalette->setCurrentIndex(index);

  LOG_DEBUG("Set tab property to {}", flow->id());
  newView->setProperty("id", flow->id());
  mCanvasPanel->addTab(newView, QIcon(":/icons/behaviour.svg"), flowName);
  mCanvasPanel->setCurrentWidget(newView);

  // Populate after creation
  canvas->populate(*flow->config());

  if (!nodeId.isEmpty())
    canvas->onFocusNode("", nodeId, properties);
}

void MainWindow::addPluginTab(const QString& name, PluginView* view)
{
  mCanvasPanel->addTab(view, QIcon(":/icons/plugin.svg"), name);
  mCanvasPanel->setCurrentWidget(view);
}

void MainWindow::removePluginTab(PluginView* view)
{
  int index = mCanvasPanel->indexOf(view);
  if (index >= 0)
    mCanvasPanel->removeTab(index);
}

void MainWindow::addEditorTab(QPlainTextEdit* editorTab)
{
  for (int i = 0; i < mCanvasPanel->count(); ++i)
  {
    if (auto* editor = qobject_cast<QPlainTextEdit*>(mCanvasPanel->widget(i)))
    {
      editor->setPlainText(editorTab->toPlainText());
      mCanvasPanel->setTabText(i, editorTab->accessibleName());
      mCanvasPanel->setCurrentWidget(editor);
      return;
    }
  }

  mCanvasPanel->addTab(editorTab, QIcon(":/icons/file.svg"), editorTab->accessibleName());
  mCanvasPanel->setCurrentWidget(editorTab);
}

void MainWindow::onFlowAdded(Flow* flow, NodeItem* node)
{
  LOG_WARN_ON_FAILURE(mSystemMenu->onFlowAdded(flow, node));
  LOG_WARN_ON_FAILURE(mPropertiesMenu->onFlowAdded(flow, node));
}

void MainWindow::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  if (canvas()->id() == flowId)
  {
    int oldTab = mCanvasPanel->currentIndex();
    mCanvasPanel->setCurrentIndex(oldTab - 1);
    mCanvasPanel->removeTab(oldTab);
  }
  else
  {
    for (int i = 0; i < mCanvasPanel->count(); ++i)
    {
      auto prop = mCanvasPanel->widget(i)->property("id");
      if (!prop.isValid() || prop.toString() != flowId)
        continue;

      mCanvasPanel->removeTab(i);
    }
  }

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

  dialog.setWebsiteUrl("https://felipeacxavier.github.io/maki/");
  dialog.setLicense("MIT License");
  dialog.setCopyright("© 2026 Felipe Xavier");

  dialog.addSocialMediaLink("GitHub", "https://github.com/FelipeACXavier", QIcon(":/icons/github.svg"));

  dialog.addSocialMediaLink("Research", "https://research.tue.nl/nl/persons/felipe-de-azeredo-coutinho-xavier/", QIcon(":/icons/research.svg"));

  dialog.addSocialMediaLink("Website", "https://felipeacxavier.github.io", QIcon(":/icons/me.svg"));

  dialog.exec();
}
