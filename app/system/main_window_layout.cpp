#include "main_window_layout.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QUndoGroup>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/IconWidget.hpp>

// Custom widgets
#include "app_configs.h"
#include "logging.h"
#include "process_tab.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "widgets/dropdown_button.h"
#include "widgets/frame.h"
#include "widgets/log_table_widget.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/file_menu.h"
#include "widgets/structure/system_menu.h"
#include "widgets/widget_factory.h"

static constexpr int MINIMUM_MENU_WIDTH = 250;
static constexpr int MAXIMUM_MENU_WIDTH = 400;

MainWindowLayout::MainWindowLayout(QWidget* parent)
    : QMainWindow(parent)
{
  buildMainWindow();
}

void MainWindowLayout::buildMainWindow()
{
  // Central widget
  mCentralWidget = new QWidget(this);
  QHBoxLayout* mainLayout = new QHBoxLayout(mCentralWidget);
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
  {
    const auto theme = qlementineStyle->theme();
    mainLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
    mainLayout->setSpacing(theme.spacing);
  }

  // Main horizontal splitter
  mMainSplitter = new QSplitter(Qt::Horizontal, mCentralWidget);

  // Application undo group
  mUndoGroup = new QUndoGroup(this);

  buildLeftPanel();
  buildCentralPanel();
  buildRightPanel();

  mainLayout->addWidget(mMainSplitter);
  mCentralWidget->setLayout(mainLayout);
  setCentralWidget(mCentralWidget);

  buildMenuBar();

  applyTheme();
}

void MainWindowLayout::buildLeftPanel()
{
  mLeftPanel = new QSplitter(Qt::Vertical, mMainSplitter);
  mPalette = new QTabWidget(mLeftPanel);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  // -----------------------------------------------------------------------------
  // Structure
  {
    auto* sContainer = new QWidget(mPalette);
    auto* scLayout = new QVBoxLayout(sContainer);
    scLayout->setContentsMargins(0, theme.spacing, theme.spacing, 0);
    scLayout->setSpacing(theme.spacing);

    mStructureTab = new StyledFrame();
    mStructureTab->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    mStructureTab->setBorderRole(StyledFrame::BorderRole::Mid);
    mStructureTab->setRadius(theme.borderRadius);
    mStructureTab->setBorderWidth(theme.borderWidth);
    mStructureTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
    structureLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
    mStructureTab->setLayout(structureLayout);

    mStructureScrollArea = new QScrollArea(this);
    mStructureScrollArea->setWidgetResizable(true);
    mStructureScrollArea->setFrameShape(QFrame::NoFrame);
    mStructureScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mStructureScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mStructureScrollArea->setWidget(mStructureTab);

    scLayout->addWidget(mStructureScrollArea);

    auto sindex = mPalette->addTab(sContainer, iconFromTheme("structure"), tr("Structure"));
    mTranslatable.push_back({mPalette->tabBar(), "Structure", sindex});
  }
  // -----------------------------------------------------------------------------
  // Behaviour
  {
    auto* bContainer = new QWidget(mPalette);
    auto* bcLayout = new QVBoxLayout(bContainer);
    bcLayout->setContentsMargins(0, theme.spacing, theme.spacing, 0);
    bcLayout->setSpacing(theme.spacing);

    mBehaviourTab = new StyledFrame();
    mBehaviourTab->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    mBehaviourTab->setBorderRole(StyledFrame::BorderRole::Mid);
    mBehaviourTab->setRadius(theme.borderRadius);
    mBehaviourTab->setBorderWidth(theme.borderWidth);

    QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
    behaviourLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
    mBehaviourTab->setLayout(behaviourLayout);

    mBehaviourScrollArea = new QScrollArea(this);
    mBehaviourScrollArea->setWidgetResizable(true);
    mBehaviourScrollArea->setFrameShape(QFrame::NoFrame);
    mBehaviourScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mBehaviourScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mBehaviourScrollArea->setWidget(mBehaviourTab);

    bcLayout->addWidget(mBehaviourScrollArea);

    auto bindex = mPalette->addTab(bContainer, iconFromTheme("behaviour"), tr("Behavior"));
    mTranslatable.push_back({mPalette->tabBar(), "Behavior", bindex});
  }
  // -----------------------------------------------------------------------------
  // Pipeline
  {
    auto* pContainer = new QWidget(mPalette);
    auto* pcLayout = new QVBoxLayout(pContainer);
    pcLayout->setContentsMargins(0, theme.spacing, theme.spacing, 0);
    pcLayout->setSpacing(theme.spacing);

    mPipelineTab = new StyledFrame();
    mPipelineTab->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    mPipelineTab->setBorderRole(StyledFrame::BorderRole::Mid);
    mPipelineTab->setRadius(theme.borderRadius);
    mPipelineTab->setBorderWidth(theme.borderWidth);

    QVBoxLayout* behaviourLayout = new QVBoxLayout(mPipelineTab);
    behaviourLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
    mPipelineTab->setLayout(behaviourLayout);

    mPipelineScrollArea = new QScrollArea(this);
    mPipelineScrollArea->setWidgetResizable(true);
    mPipelineScrollArea->setFrameShape(QFrame::NoFrame);
    mPipelineScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mPipelineScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mPipelineScrollArea->setWidget(mPipelineTab);

    pcLayout->addWidget(mPipelineScrollArea);

    auto bindex = mPalette->addTab(pContainer, iconFromTheme("behaviour"), tr("Pipeline"));
    mPalette->setTabVisible(bindex, false);
    mTranslatable.push_back({mPalette->tabBar(), "Pipeline", bindex});
  }

  // Rest of palette
  mPalette->tabBar()->setIconSize(QSize(16, 16));

  mPaletteSearch = new maki::SearchWidget(tr("Filter nodes"), mLeftPanel);
  mPaletteSearch->hide();

  connect(mPaletteSearch, &maki::SearchWidget::dismissed, this, [this] {
    mPaletteSearch->hide();
    if (mPalette->currentIndex() == 0)
      mStructureScrollArea->setFocus();
    else
      mBehaviourScrollArea->setFocus();
  });

  mLeftPanel->addWidget(mPalette);
  mLeftPanel->addWidget(mPaletteSearch);

  mMainSplitter->addWidget(mLeftPanel);
}

void MainWindowLayout::buildCentralPanel()
{
  mCentralSplitter = new QSplitter(Qt::Vertical);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  mHeaderWidget = new QWidget(mCentralSplitter);
  QHBoxLayout* headerLayout = new QHBoxLayout(mHeaderWidget);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setSpacing(5);
  headerLayout->setAlignment(Qt::AlignCenter);
  headerLayout->addSpacing(24);

  // ----------------------------------------------------------------
  headerLayout->addStretch();
  mPipelineRun = new DropDownButton(mHeaderWidget);
  mPipelineRun->setIcon(iconFromTheme("exaile-play"));
  mPipelineRun->setToolTip(tr("Run pipeline"));

  headerLayout->addWidget(mPipelineRun);

  // ---------------------------------------------
  auto* spinnerContainer = new QWidget(mHeaderWidget);
  spinnerContainer->setFixedSize(24, 24);

  auto* spinnerLayout = new QHBoxLayout(spinnerContainer);
  spinnerLayout->setContentsMargins(0, 0, 0, 0);

  mGenerationSpinner = new oclero::qlementine::LoadingSpinner(this);
  mGenerationSpinner->setVisible(false);

  spinnerLayout->addWidget(mGenerationSpinner);
  headerLayout->addWidget(spinnerContainer);
  headerLayout->addStretch();

  // ---------------------------------------------
  mCanvasPanel = new QTabWidget(mCentralSplitter);

  CanvasView* canvasView = new CanvasView(mCanvasPanel);

  mCanvasPanel->addTab(canvasView, iconFromTheme("structure"), tr("System"));
  mCanvasPanel->setCurrentWidget(canvasView);

  QWidget* canvasContainer = new QWidget();
  QVBoxLayout* canvasLayout = new QVBoxLayout(canvasContainer);
  canvasLayout->setContentsMargins(0, 0, 0, 0);
  canvasLayout->setSpacing(0);

  // Add header + tabs
  canvasLayout->addWidget(mHeaderWidget);
  canvasLayout->addWidget(mCanvasPanel);

  // Now add the container to the splitter
  mCentralSplitter->addWidget(canvasContainer);

  // =================================================================
  // Bottom panel
  mBottomContainer = new QWidget();
  QVBoxLayout* bottomLayout = new QVBoxLayout(mBottomContainer);
  bottomLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.borderWidth);
  bottomLayout->setSpacing(theme.spacing);

  auto* bottomNavContainer = new QWidget();
  QHBoxLayout* bottomNavLayout = new QHBoxLayout(bottomNavContainer);
  bottomNavLayout->setContentsMargins(0, 0, 0, 0);
  bottomNavLayout->setSpacing(0);

  mBottomNavigation = new oclero::qlementine::NavigationBar(mBottomContainer);
  mBottomPanel = new QStackedWidget(mBottomContainer);

  bottomNavLayout->addWidget(mBottomNavigation);
  bottomNavLayout->addStretch();
  bottomLayout->addWidget(bottomNavContainer);
  bottomLayout->addWidget(mBottomPanel);

  // ===================================================================
  // QWidget* infoContainer = new QWidget(mBottomPanel);
  auto* infoContainer = new StyledFrame(mBottomPanel);
  infoContainer->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  infoContainer->setBorderRole(StyledFrame::BorderRole::Mid);
  infoContainer->setRadius(theme.borderRadius);
  infoContainer->setBorderWidth(theme.borderWidth);

  QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
  infoLayout->setContentsMargins(theme.borderWidth, theme.borderWidth, theme.borderWidth, theme.borderWidth);
  infoLayout->setSpacing(0);

  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(theme.fontRegular);
  mInfoText->setHtml(createDefaultMessage());

  infoLayout->addWidget(mInfoText);

  mBottomNavigation->addItem(tr("Info"), iconFromTheme("info"));
  mBottomPanel->addWidget(infoContainer);

  // -----------------------------------------------------------------
  // Log container
  auto* logContainer = new StyledFrame(mBottomPanel);
  logContainer->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  logContainer->setBorderRole(StyledFrame::BorderRole::Mid);
  logContainer->setRadius(theme.borderRadius);
  logContainer->setBorderWidth(theme.borderWidth);

  QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  logLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.borderWidth);
  logLayout->setSpacing(theme.spacing);

  mLogTable = new LogTableWidget(logContainer);
  logLayout->addWidget(mLogTable);

  mBottomNavigation->addItem(tr("Log"), iconFromTheme("logs"));
  mBottomPanel->addWidget(logContainer);

  // -----------------------------------------------------------------
  // Process tab
  auto* pluginContainer = new StyledFrame(mBottomPanel);
  pluginContainer->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  pluginContainer->setBorderRole(StyledFrame::BorderRole::Mid);
  pluginContainer->setRadius(theme.borderRadius);
  pluginContainer->setBorderWidth(theme.borderWidth);

  QVBoxLayout* pluginLayout = new QVBoxLayout(pluginContainer);
  pluginLayout->setContentsMargins(theme.borderWidth, theme.borderWidth, theme.borderWidth, theme.borderWidth);
  pluginLayout->setSpacing(0);

  mProcessTab = new ProcessTab(pluginContainer);
  pluginLayout->addWidget(mProcessTab);

  mBottomNavigation->addItem(tr("Generation"), iconFromTheme("terminal"));
  mBottomPanel->addWidget(pluginContainer);

  // -----------------------------------------------------------------
  // Final detals
  connect(mBottomNavigation, &oclero::qlementine::NavigationBar::currentIndexChanged, [this]() {
    mBottomPanel->setCurrentIndex(mBottomNavigation->currentIndex());
  });
  mBottomPanel->setCurrentIndex(0);

  mCentralSplitter->addWidget(mBottomContainer);
  mCentralSplitter->setCollapsible(0, false);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowLayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);
  mRightPanel->setMinimumWidth(MINIMUM_MENU_WIDTH);
  mRightPanel->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  // ----------------------------------------------------------------------
  // Navigation Menu
  mNavigationTab = new QTabWidget();
  auto* systemContainer = new QWidget(mNavigationTab);

  QVBoxLayout* systemLayout = new QVBoxLayout(systemContainer);
  systemLayout->setContentsMargins(theme.spacing, theme.spacing, 0, theme.spacing);
  systemLayout->setSpacing(theme.spacing);

  mSystemMenu = new SystemMenu(systemContainer);
  systemLayout->addWidget(mSystemMenu);

  auto sindex = mNavigationTab->addTab(systemContainer, iconFromTheme("system_menu"), tr("System"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "System", sindex});

  auto* filesContainer = new QWidget(mNavigationTab);

  QVBoxLayout* filesLayout = new QVBoxLayout(filesContainer);
  filesLayout->setContentsMargins(theme.spacing, theme.spacing, 0, theme.spacing);
  filesLayout->setSpacing(theme.spacing);

  mFileMenu = new GeneratedFilesPanel(mNavigationTab);
  filesLayout->addWidget(mFileMenu);

  auto findex = mNavigationTab->addTab(filesContainer, iconFromTheme("tree"), tr("Files"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "Files", findex});

  // ----------------------------------------------------------------------
  // Properties Menu
  mPropertiesTab = new QTabWidget();
  mPropertiesTab->setMinimumHeight(500);

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  auto pindex = mPropertiesTab->addTab(mPropertiesMenu, iconFromTheme("properties"), tr("Properties"));
  mTranslatable.push_back({mPropertiesTab->tabBar(), "Properties", pindex});

  mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mPropertiesTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mPropertiesTab->tabBar()->setIconSize(QSize(16, 16));
  mNavigationTab->tabBar()->setIconSize(QSize(16, 16));

  mRightPanel->addWidget(mNavigationTab);
  mRightPanel->addWidget(mPropertiesTab);

  mMainSplitter->addWidget(mRightPanel);
}

void MainWindowLayout::buildMenuBar()
{
#ifndef __EMSCRIPTEN__
  // === Menu Bar ===
  mMenuBar = new QMenuBar(this);
  mMenuBar->setNativeMenuBar(true);

  mMenuBar->addMenu(createFileMenu(mMenuBar));
  mMenuBar->addMenu(createEditMenu(mMenuBar));
  mMenuBar->addMenu(createViewMenu(mMenuBar));
  mMenuBar->addMenu(createDiagramMenu(mMenuBar));
  mMenuBar->addMenu(createToolsMenu(mMenuBar));
  mMenuBar->addMenu(createHelpMenu(mMenuBar));

  setMenuBar(mMenuBar);
#else
  mMenuButton = new QToolButton(this);
  mMenuButton->setIcon(iconFromTheme("bars"));
  // mMenuButton->setPopupMode(ToolButtonPopupMode::);

  auto* menu = new QMenu(mMenuButton);

  menu->addMenu(createFileMenu(menu));
  menu->addMenu(createEditMenu(menu));
  menu->addMenu(createViewMenu(menu));
  menu->addMenu(createDiagramMenu(menu));
  menu->addMenu(createToolsMenu(menu));
  menu->addMenu(createHelpMenu(menu));

  connect(mMenuButton, &QToolButton::clicked, this, [this, menu] {
    const QPoint pos = mMenuButton->mapToGlobal(QPoint(
        mMenuButton->width() - mMenuButton->sizeHint().width(),
        mMenuButton->height()));

    menu->popup(pos);
  });

  if (mHeaderWidget)
    mHeaderWidget->layout()->addWidget(mMenuButton);
#endif
}

QMenu* MainWindowLayout::createFileMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // File menu
  auto* file = new QMenu(tr("File"), parent);
  mTranslatable.push_back({file, "File"});

  mActionNew = new QAction(iconFromTheme("document-new", false), tr("New"), this);
  mTranslatable.push_back({mActionNew, "New"});
  file->addAction(mActionNew);

  mActionOpen = new QAction(iconFromTheme("document-open", false), tr("Open"), this);
  mTranslatable.push_back({mActionOpen, "Open"});
  file->addAction(mActionOpen);

  mActionOpenRecent = file->addMenu(iconFromTheme("document-open-recent", false), tr("Open Recent"));
  mTranslatable.push_back({mActionOpenRecent, "Open Recent"});
  mActionOpenRecent->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  file->addSeparator();

  mActionSave = new QAction(iconFromTheme("document-save", false), tr("Save"), this);
  mTranslatable.push_back({mActionSave, "Save"});
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(iconFromTheme("document-save-as", false), tr("Save As"), this);
  mTranslatable.push_back({mActionSaveAs, "Save As"});
  file->addAction(mActionSaveAs);

  file->addSeparator();

  mActionImportLibrary = new QAction(iconFromTheme("install", false), tr("Install library"), this);
  mTranslatable.push_back({mActionImportLibrary, "Install library"});
  file->addAction(mActionImportLibrary);

  mActionInstallPlugin = new QAction(iconFromTheme("plugins", false), tr("Install plugin"), this);
  mTranslatable.push_back({mActionInstallPlugin, "Install plugin"});
  file->addAction(mActionInstallPlugin);

  file->addSeparator();
  mActionExit = new QAction(iconFromTheme("application-exit", false), tr("Exit"), this);
  mTranslatable.push_back({mActionExit, "Exit"});
  file->addAction(mActionExit);

  return file;
}

QMenu* MainWindowLayout::createEditMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // Edit menu
  auto* edit = new QMenu(tr("Edit"), parent);
  mTranslatable.push_back({edit, "Edit"});

  mActionUndo = mUndoGroup->createUndoAction(this, tr("Undo"));
  mActionUndo->setIcon(iconFromTheme("edit-undo", false));
  mTranslatable.push_back({mActionUndo, "Undo"});
  edit->addAction(mActionUndo);

  mActionRedo = mUndoGroup->createRedoAction(this, tr("Redo"));
  mActionRedo->setIcon(iconFromTheme("edit-redo", false));
  mTranslatable.push_back({mActionRedo, "Redo"});
  edit->addAction(mActionRedo);

  edit->addSeparator();

  mActionCopy = new QAction(iconFromTheme("edit-copy", false), tr("Copy"), this);
  mTranslatable.push_back({mActionCopy, "Copy"});
  mActionCopy->setEnabled(false);
  edit->addAction(mActionCopy);

  mActionPaste = new QAction(iconFromTheme("edit-paste", false), tr("Paste"), this);
  mTranslatable.push_back({mActionPaste, "Paste"});
  mActionPaste->setEnabled(false);
  edit->addAction(mActionPaste);

  mActionCut = new QAction(iconFromTheme("edit-cut", false), tr("Cut"), this);
  mTranslatable.push_back({mActionCut, "Cut"});
  mActionCut->setEnabled(false);
  edit->addAction(mActionCut);

  mActionDelete = new QAction(iconFromTheme("edit-delete", false), tr("Delete"), this);
  mTranslatable.push_back({mActionDelete, "Delete"});
  mActionDelete->setEnabled(false);
  edit->addAction(mActionDelete);

  return edit;
}

QMenu* MainWindowLayout::createViewMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // View menu
  auto* view = new QMenu(tr("View"), parent);
  mTranslatable.push_back({view, "View"});

  mActionZoomIn = new QAction(iconFromTheme("zoom-in", false), tr("Zoom In"), this);
  mTranslatable.push_back({mActionZoomIn, "Zoom In"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomOut = new QAction(iconFromTheme("zoom-out", false), tr("Zoom Out"), this);
  mTranslatable.push_back({mActionZoomOut, "Zoom Out"});
  mActionZoomOut->setEnabled(false);
  view->addAction(mActionZoomOut);

  mActionResetZoom = new QAction(iconFromTheme("zoom-original", false), tr("Reset Zoom"), this);
  mTranslatable.push_back({mActionResetZoom, "Reset Zoom"});
  mActionResetZoom->setEnabled(false);
  view->addAction(mActionResetZoom);

  mActionFitToScreen = new QAction(iconFromTheme("zoom-fit-best", false), tr("Fit to Screen"), this);
  mTranslatable.push_back({mActionFitToScreen, "Fit to Screen"});
  mActionFitToScreen->setEnabled(false);
  view->addAction(mActionFitToScreen);

  view->addSeparator();

  QMenu* showMenu = view->addMenu(tr("Show/Hide"));
  showMenu->setIcon(iconFromTheme("view-visible", false));
  mTranslatable.push_back({showMenu, "Show/Hide"});
  view->addMenu(showMenu);

  mOpenComponentsPanel = new QAction(tr("Components panel"), this);
  mOpenComponentsPanel->setIcon(iconFromTheme("view-visible", false));
  mTranslatable.push_back({mOpenComponentsPanel, "Components panel"});
  showMenu->addAction(mOpenComponentsPanel);
  connect(mOpenComponentsPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mLeftPanel, mOpenComponentsPanel);
  });

  mOpenInfoPanel = new QAction(tr("Information panel"), this);
  mOpenInfoPanel->setIcon(iconFromTheme("view-visible", false));
  mTranslatable.push_back({mOpenInfoPanel, "Information panel"});
  showMenu->addAction(mOpenInfoPanel);
  connect(mOpenInfoPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mBottomContainer, mOpenInfoPanel);
  });

  mOpenPropertiesPanel = new QAction(tr("Properties panel"), this);
  mOpenPropertiesPanel->setIcon(iconFromTheme("view-visible", false));
  mTranslatable.push_back({mOpenPropertiesPanel, "Properties panel"});
  showMenu->addAction(mOpenPropertiesPanel);
  connect(mOpenPropertiesPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mRightPanel, mOpenPropertiesPanel);
  });

  mSpecialTabsMenu = view->addMenu(iconFromTheme("plugins", false), tr("Plugin tabs"));
  mTranslatable.push_back({mSpecialTabsMenu, "Plugin tabs"});
  view->addMenu(mSpecialTabsMenu);

  return view;
}

QMenu* MainWindowLayout::createDiagramMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // Diagram menu
  auto* diagram = new QMenu(tr("Diagram"), parent);
  mTranslatable.push_back({diagram, "Diagram"});

  mActionAutoRoute = new QAction(iconFromTheme("auto-route", false), tr("Auto-route"), this);
  diagram->addAction(mActionAutoRoute);

  return diagram;
}

QMenu* MainWindowLayout::createToolsMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // Tools menu
  auto* tools = new QMenu(tr("Tools"), parent);
  mTranslatable.push_back({tools, "Tools"});

  mGeneratorMenu = new QMenu(tr("Generator"));
  mGeneratorMenu->setIcon(iconFromTheme("special-effects-symbolic", false));
  mTranslatable.push_back({mGeneratorMenu, "Generator"});
  tools->addMenu(mGeneratorMenu);

  mActionGenerate = new QAction(iconFromTheme("document-edit-decrypt-verify", false), tr("Verify"), this);
  mTranslatable.push_back({mActionGenerate, "Verify"});
  tools->addAction(mActionGenerate);

  mActionSimulate = new QAction(iconFromTheme("exaile-play", false), tr("Simulate"), this);
  mTranslatable.push_back({mActionSimulate, "Simulate"});
  tools->addAction(mActionSimulate);

  mActionToggleGrid = new QAction(iconFromTheme("grid-rectangular", false), tr("Toggle Grid"), this);
  mTranslatable.push_back({mActionToggleGrid, "Toggle Grid"});
  mActionToggleGrid->setEnabled(false);
  tools->addAction(mActionToggleGrid);

  mActionToggleSnapToGrid = new QAction(iconFromTheme("snap", false), tr("Toggle Snap to Grid"), this);
  mTranslatable.push_back({mActionToggleSnapToGrid, "Toggle Snap to Grid"});
  mActionToggleSnapToGrid->setEnabled(false);
  tools->addAction(mActionToggleSnapToGrid);

  return tools;
}

QMenu* MainWindowLayout::createHelpMenu(QWidget* parent)
{
  // ----------------------------------------------------------
  // Help menu
  auto* help = new QMenu(tr("Help"), parent);
  mTranslatable.push_back({help, "Help"});

  mActionDocumentation = new QAction(iconFromTheme("help-browser", false), tr("Documentation"), this);
  mTranslatable.push_back({mActionDocumentation, "Documentation"});
  mActionDocumentation->setEnabled(false);
  help->addAction(mActionDocumentation);

  mActionQuickStartGuide = new QAction(iconFromTheme("show-guides", false), tr("Quick Start Guide"), this);
  mTranslatable.push_back({mActionQuickStartGuide, "Quick Start Guide"});
  mActionQuickStartGuide->setEnabled(false);
  help->addAction(mActionQuickStartGuide);

  mOpenAllSettings = new QAction(iconFromTheme("preferences-system", false), tr("Open All Settings"), this);
  mTranslatable.push_back({mOpenAllSettings, "Open All Settings"});
  help->addAction(mOpenAllSettings);

  mActionShortcuts = new QAction(iconFromTheme("help-keyboard-shortcuts", false), tr("Shortcuts"), this);
  mTranslatable.push_back({mActionShortcuts, "Shortcuts"});
  mActionShortcuts->setEnabled(false);
  help->addAction(mActionShortcuts);

  mActionReportIssue = new QAction(iconFromTheme("tools-report-bug", false), tr("Report Issue"), this);
  mTranslatable.push_back({mActionReportIssue, "Report Issue"});
  mActionReportIssue->setEnabled(false);
  help->addAction(mActionReportIssue);

  mAboutAction = new QAction(iconFromTheme("help-about", false), tr("About"), this);
  mTranslatable.push_back({mAboutAction, "About"});
  help->addAction(mAboutAction);

  return help;
}

int MainWindowLayout::setTabBarWidth(QTabBar* bar, int minWidth, int minBorder, int minPadding)
{
  QFontMetrics fm(bar->font());
  int iconSize = bar->iconSize().width();
  int maxWidth = minWidth;
  int count = 0;

  // Check the width of all tabs and get the largest
  for (int i = 0; i < bar->count(); ++i)
  {
    if (!bar->isTabEnabled(i) || !bar->isTabVisible(i))
      continue;

    int textWidth = fm.horizontalAdvance(bar->tabText(i)) + iconSize + (2 * minPadding);
    maxWidth = qMax(textWidth, maxWidth);
    count++;
  }

  int singleBarWidth = (maxWidth) + (2 * minBorder) + (2 * minPadding);

  return count * singleBarWidth;
}

void MainWindowLayout::applyTheme()
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  const auto tabPadding = theme.spacing;
  const auto tabBorderSize = theme.borderWidth;
  const auto minWidth = 100;

  if (mLeftPanel && mPalette)
  {
    mLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    int width = setTabBarWidth(mPalette->tabBar(), minWidth, tabPadding, tabBorderSize);
    mPalette->setMinimumWidth(width);

    mPalette->tabBar()->setExpanding(false);
    mPalette->tabBar()->setDocumentMode(true);
    mPalette->tabBar()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    mPalette->tabBar()->setFocusPolicy(Qt::NoFocus);
    mPalette->tabBar()->setTabsClosable(false);
    mPalette->tabBar()->setMovable(false);
    mPalette->tabBar()->setChangeCurrentOnDrag(true);
  }

  if (mCanvasPanel)
  {
    mCanvasPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mCanvasPanel->setTabsClosable(true);
    mCanvasPanel->tabBar()->setExpanding(false);
    mCanvasPanel->tabBar()->setDocumentMode(true);
    mCanvasPanel->tabBar()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mCanvasPanel->tabBar()->setFocusPolicy(Qt::NoFocus);
    mCanvasPanel->tabBar()->setTabsClosable(true);
    mCanvasPanel->tabBar()->setMovable(true);
    mCanvasPanel->tabBar()->setChangeCurrentOnDrag(false);
    mCanvasPanel->tabBar()->setUsesScrollButtons(false);

    // Remove the close button from the system tab
    mCanvasPanel->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
    mCanvasPanel->tabBar()->setIconSize(QSize(16, 16));

    // Qlementine causes a weird issue where the text disappears when a tab is moved
    connect(mCanvasPanel->tabBar(), &QTabBar::tabMoved, this, [this](int /* from */, int /* to */) {
      for (int i = 0; i < mCanvasPanel->count(); ++i)
        mCanvasPanel->setTabText(i, mCanvasPanel->tabText(i));
    });
  }

  if (mInfoText)
  {
    // Update the info text so the welcome message fits
    QTimer::singleShot(0, this, [this, theme] {
      int documentHeight = int(std::ceil(mInfoText->document()->documentLayout()->documentSize().height()));
      int height = documentHeight + mInfoText->contentsMargins().top() + mInfoText->contentsMargins().bottom() + theme.spacing;
      mInfoText->setMinimumHeight(height);
    });
  }

  if (mRightPanel)
  {
    int navigationTabWidth = 0;
    int propertiesTabWidth = 0;

    if (mNavigationTab)
    {
      mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      navigationTabWidth = setTabBarWidth(mNavigationTab->tabBar(), minWidth, tabPadding, tabBorderSize);
      mNavigationTab->setMinimumWidth(navigationTabWidth);

      mNavigationTab->tabBar()->setExpanding(false);
      mNavigationTab->tabBar()->setDocumentMode(true);
      mNavigationTab->tabBar()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
      mNavigationTab->tabBar()->setFocusPolicy(Qt::NoFocus);
      mNavigationTab->tabBar()->setTabsClosable(false);
      mNavigationTab->tabBar()->setMovable(false);
      mNavigationTab->tabBar()->setChangeCurrentOnDrag(false);
      mNavigationTab->tabBar()->setUsesScrollButtons(false);
    }

    if (mPropertiesTab)
    {
      mPropertiesTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      propertiesTabWidth = setTabBarWidth(mPropertiesTab->tabBar(), minWidth, tabPadding, tabBorderSize);
      mPropertiesTab->setMinimumWidth(propertiesTabWidth);

      mPropertiesTab->tabBar()->setExpanding(false);
      mPropertiesTab->tabBar()->setDocumentMode(true);
      mPropertiesTab->tabBar()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
      mPropertiesTab->tabBar()->setFocusPolicy(Qt::NoFocus);
      mPropertiesTab->tabBar()->setTabsClosable(false);
      mPropertiesTab->tabBar()->setMovable(false);
      mPropertiesTab->tabBar()->setChangeCurrentOnDrag(false);
      mPropertiesTab->tabBar()->setUsesScrollButtons(false);
    }

    mRightPanel->setMinimumWidth(std::max(navigationTabWidth, propertiesTabWidth));

    // Set initial height ratio
    mRightPanel->setSizes({MAXIMUM_MENU_WIDTH, 600});
  }
}

void MainWindowLayout::onLanguageChanged()
{
  for (auto& item : mTranslatable)
  {
    if (item.widget)
    {
      if (auto label = qobject_cast<QLabel*>(item.widget))
      {
        label->setText(tr(item.text));
      }
      else if (auto button = qobject_cast<QPushButton*>(item.widget))
      {
        if (item.tooltip)
          button->setToolTip(tr(item.text));
      }
      else if (auto tabBar = qobject_cast<QTabBar*>(item.widget))
      {
        if (item.index < tabBar->count())
          tabBar->setTabText(item.index, tr(item.text));
      }
      else if (auto action = qobject_cast<QAction*>(item.widget))
      {
        action->setText(tr(item.text));
      }
      else if (auto menu = qobject_cast<QMenu*>(item.widget))
      {
        menu->setTitle(tr(item.text));
      }
      else
      {
        LOG_WARNING("Unsupported widget: %s", item.widget->metaObject()->className());
      }
    }
  }

  // Reapply theme so the new labels fit, at least initially
  applyTheme();
}

QWidget* MainWindowLayout::createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip)
{
  QWidget* wrapper = new QWidget();
  QHBoxLayout* wLayout = new QHBoxLayout(wrapper);
  wLayout->setContentsMargins(0, 0, 0, 0);
  wLayout->setSpacing(4);

  auto* iconLabel = new oclero::qlementine::IconWidget(QIcon(iconPath), QSize(16, 16));
  iconLabel->setToolTip(tooltip);
  iconLabel->setToolTipDuration(2000);

  comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  comboBox->setMinimumWidth(150);
  comboBox->setFixedHeight(30);
  comboBox->setToolTip(tooltip);
  comboBox->setToolTipDuration(2000);

  wLayout->addWidget(iconLabel);
  wLayout->addWidget(comboBox);

  return wrapper;
}

void MainWindowLayout::toggleGenerationButton(bool running)
{
  if (running)
  {
    mGenerationSpinner->setSpinning(true);
    mGenerationSpinner->setVisible(true);
    mPipelineRun->setToolTip(tr("Cancel current pipeline"));
  }
  else
  {
    mGenerationSpinner->setSpinning(false);
    mGenerationSpinner->setVisible(false);
    mPipelineRun->setToolTip(tr("Run pipeline"));
  }

  mPipelineRun->setRunning(running);
}

void MainWindowLayout::togglePanelVisibility(QWidget* panel, QAction* action)
{
  if (panel->isHidden())
  {
    panel->show();
    action->setIcon(iconFromTheme("view-visible", false));
  }
  else
  {
    panel->hide();
    action->setIcon(iconFromTheme("view-hidden", false));
  }
}
