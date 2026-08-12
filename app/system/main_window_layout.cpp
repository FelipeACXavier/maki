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
#include "widgets/type_editor.h"
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

    auto sindex = mPalette->addTab(sContainer, QIcon(":/icons/structure.svg"), tr("Structure"));
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

    auto bindex = mPalette->addTab(bContainer, QIcon(":/icons/behaviour.svg"), tr("Behavior"));
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

    auto bindex = mPalette->addTab(pContainer, QIcon(":/icons/behaviour.svg"), tr("Pipeline"));
    mPalette->setTabVisible(bindex, false);
    mTranslatable.push_back({mPalette->tabBar(), "Pipeline", bindex});
  }

  // Rest of palette
  mPalette->tabBar()->setIconSize(Config::SMALL_BUTTON_SIZE);

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

  QWidget* header = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setSpacing(5);
  headerLayout->setAlignment(Qt::AlignCenter);
  headerLayout->addSpacing(24);

  // ----------------------------------------------------------------
  mPipelineRun = new ExecuteButton(header);
  mPipelineRun->setIcon(iconFromTheme("exaile-play"));
  mPipelineRun->setToolTip(tr("Run pipeline"));

  headerLayout->addWidget(mPipelineRun);

  // ---------------------------------------------
  auto* spinnerContainer = new QWidget(header);
  spinnerContainer->setFixedSize(24, 24);

  auto* spinnerLayout = new QHBoxLayout(spinnerContainer);
  spinnerLayout->setContentsMargins(0, 0, 0, 0);

  mGenerationSpinner = new oclero::qlementine::LoadingSpinner(this);
  mGenerationSpinner->setVisible(false);

  spinnerLayout->addWidget(mGenerationSpinner);
  headerLayout->addWidget(spinnerContainer);

  // ---------------------------------------------
  mCanvasPanel = new QTabWidget(mCentralSplitter);

  CanvasView* canvasView = new CanvasView(mCanvasPanel);

  mCanvasPanel->addTab(canvasView, QIcon(":/icons/structure.svg"), tr("System"));
  mCanvasPanel->setCurrentWidget(canvasView);

  // Top right buttons
  QWidget* corner = new QWidget();
  auto* layout = new QHBoxLayout(corner);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  // First button
  mBrowserTabButton = new QPushButton(corner);
  mBrowserTabButton->setIcon(QIcon(":/icons/display.svg"));
  mBrowserTabButton->setToolTip(tr("Show simulation tab"));
  mTranslatable.push_back({mBrowserTabButton, "Show simulation tab", 0, true});

  layout->addWidget(mBrowserTabButton);

  // Add the whole widget to the corner
  mCanvasPanel->setCornerWidget(corner, Qt::TopRightCorner);

  QWidget* canvasContainer = new QWidget();
  QVBoxLayout* canvasLayout = new QVBoxLayout(canvasContainer);
  canvasLayout->setContentsMargins(0, 0, 0, 0);
  canvasLayout->setSpacing(0);

  // Add header + tabs
  canvasLayout->addWidget(header);
  canvasLayout->addWidget(mCanvasPanel);

  // Now add the container to the splitter
  mCentralSplitter->addWidget(canvasContainer);

  // =================================================================
  // Bottom panel
  mBottomContainer = new QWidget();
  QVBoxLayout* bottomLayout = new QVBoxLayout(mBottomContainer);
  bottomLayout->setContentsMargins(theme.spacing, theme.spacing / 2, theme.spacing, theme.borderWidth);
  bottomLayout->setSpacing(theme.spacing);

  auto* bottomNavContainer = new QWidget();
  QHBoxLayout* bottomNavLayout = new QHBoxLayout(bottomNavContainer);
  bottomNavLayout->setContentsMargins(0, 0, 0, 0);
  bottomNavLayout->setSpacing(0);

  mBottomNavigation = new oclero::qlementine::NavigationBar(mBottomContainer);
  mBottomNavigation->setMaximumHeight(40);
  mBottomPanel = new QStackedWidget(mBottomContainer);

  bottomNavLayout->addWidget(mBottomNavigation);
  bottomNavLayout->addStretch();
  bottomLayout->addWidget(bottomNavContainer);
  bottomLayout->addWidget(mBottomPanel);

  // ===================================================================
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
  mInfoText->setFocusPolicy(Qt::FocusPolicy::NoFocus);

  infoLayout->addWidget(mInfoText);

  mBottomNavigation->addItem(tr("Info"), QIcon(":/icons/info.svg"));
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

  mBottomNavigation->addItem(tr("Log"), QIcon(":/icons/logs.svg"));
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

  mBottomNavigation->addItem(tr("Generation"), QIcon(":/icons/terminal.svg"));
  mBottomPanel->addWidget(pluginContainer);

  // -----------------------------------------------------------------
  // Type editor
  auto* typeContainer = new StyledFrame(mBottomPanel);
  typeContainer->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  typeContainer->setBorderRole(StyledFrame::BorderRole::Mid);
  typeContainer->setRadius(theme.borderRadius);
  typeContainer->setBorderWidth(theme.borderWidth);

  QVBoxLayout* typeLayout = new QVBoxLayout(typeContainer);
  typeLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.borderWidth);
  typeLayout->setSpacing(theme.spacing);

  auto typeEditor = new maki::TypeEditor(typeContainer);
  typeLayout->addWidget(typeEditor);

  mBottomNavigation->addItem(tr("Type Editor"), QIcon(":/icons/document-edit.svg"));
  mBottomPanel->addWidget(typeContainer);

  // -----------------------------------------------------------------
  // Final detals
  connect(mBottomNavigation, &oclero::qlementine::NavigationBar::currentIndexChanged,
          [this]() { mBottomPanel->setCurrentIndex(mBottomNavigation->currentIndex()); });
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

  auto sindex = mNavigationTab->addTab(systemContainer, QIcon(":/icons/system_menu.svg"), tr("System"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "System", sindex});

  auto* filesContainer = new QWidget(mNavigationTab);

  QVBoxLayout* filesLayout = new QVBoxLayout(filesContainer);
  filesLayout->setContentsMargins(theme.spacing, theme.spacing, 0, theme.spacing);
  filesLayout->setSpacing(theme.spacing);

  mFileMenu = new GeneratedFilesPanel(mNavigationTab);
  filesLayout->addWidget(mFileMenu);

  auto findex = mNavigationTab->addTab(filesContainer, QIcon(":/icons/tree.svg"), tr("Files"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "Files", findex});

  // ----------------------------------------------------------------------
  // Properties Menu
  mPropertiesTab = new QTabWidget();

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  auto pindex = mPropertiesTab->addTab(mPropertiesMenu, QIcon(":/icons/properties.svg"), tr("Properties"));
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
  // === Menu Bar ===
  mMenuBar = new QMenuBar();
  mMenuBar->setNativeMenuBar(true);

  // ----------------------------------------------------------
  // File menu
  QMenu* file = mMenuBar->addMenu(tr("File"));
  mTranslatable.push_back({file, "File"});

  mActionNew = new QAction(iconFromTheme("document-new"), tr("New"), this);
  mTranslatable.push_back({mActionNew, "New"});
  file->addAction(mActionNew);

  mActionOpen = new QAction(iconFromTheme("document-open"), tr("Open"), this);
  mTranslatable.push_back({mActionOpen, "Open"});
  file->addAction(mActionOpen);

  mActionOpenRecent = file->addMenu(iconFromTheme("document-open-recent"), tr("Open Recent"));
  mTranslatable.push_back({mActionOpenRecent, "Open Recent"});
  mActionOpenRecent->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  file->addSeparator();

  mActionSave = new QAction(iconFromTheme("document-save"), tr("Save"), this);
  mTranslatable.push_back({mActionSave, "Save"});
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(iconFromTheme("document-save-as"), tr("Save As"), this);
  mTranslatable.push_back({mActionSaveAs, "Save As"});
  file->addAction(mActionSaveAs);

  file->addSeparator();

  mActionImportLibrary = new QAction(iconFromTheme("install"), tr("Install library"), this);
  mTranslatable.push_back({mActionImportLibrary, "Install library"});
  file->addAction(mActionImportLibrary);

  mActionInstallPlugin = new QAction(iconFromTheme("plugins"), tr("Install plugin"), this);
  mTranslatable.push_back({mActionInstallPlugin, "Install plugin"});
  file->addAction(mActionInstallPlugin);

  file->addSeparator();
  mActionExit = new QAction(iconFromTheme("application-exit"), tr("Exit"), this);
  mTranslatable.push_back({mActionExit, "Exit"});
  file->addAction(mActionExit);

  // ----------------------------------------------------------
  // Edit menu
  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  mTranslatable.push_back({edit, "Edit"});

  mActionUndo = mUndoGroup->createUndoAction(this, tr("Undo"));
  mActionUndo->setIcon(iconFromTheme("edit-undo"));
  mTranslatable.push_back({mActionUndo, "Undo"});
  edit->addAction(mActionUndo);

  mActionRedo = mUndoGroup->createRedoAction(this, tr("Redo"));
  mActionRedo->setIcon(iconFromTheme("edit-redo"));
  mTranslatable.push_back({mActionRedo, "Redo"});
  edit->addAction(mActionRedo);

  edit->addSeparator();

  mActionCopy = new QAction(iconFromTheme("edit-copy"), tr("Copy"), this);
  mTranslatable.push_back({mActionCopy, "Copy"});
  mActionCopy->setEnabled(false);
  edit->addAction(mActionCopy);

  mActionPaste = new QAction(iconFromTheme("edit-paste"), tr("Paste"), this);
  mTranslatable.push_back({mActionPaste, "Paste"});
  mActionPaste->setEnabled(false);
  edit->addAction(mActionPaste);

  mActionCut = new QAction(iconFromTheme("edit-cut"), tr("Cut"), this);
  mTranslatable.push_back({mActionCut, "Cut"});
  mActionCut->setEnabled(false);
  edit->addAction(mActionCut);

  mActionDelete = new QAction(iconFromTheme("edit-delete"), tr("Delete"), this);
  mTranslatable.push_back({mActionDelete, "Delete"});
  mActionDelete->setEnabled(false);
  edit->addAction(mActionDelete);

  // ----------------------------------------------------------
  // View menu
  QMenu* view = mMenuBar->addMenu(tr("View"));
  mTranslatable.push_back({view, "View"});

  mActionZoomIn = new QAction(iconFromTheme("zoom-in"), tr("Zoom In"), this);
  mTranslatable.push_back({mActionZoomIn, "Zoom In"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomOut = new QAction(iconFromTheme("zoom-out"), tr("Zoom Out"), this);
  mTranslatable.push_back({mActionZoomOut, "Zoom Out"});
  mActionZoomOut->setEnabled(false);
  view->addAction(mActionZoomOut);

  mActionResetZoom = new QAction(iconFromTheme("zoom-original"), tr("Reset Zoom"), this);
  mTranslatable.push_back({mActionResetZoom, "Reset Zoom"});
  mActionResetZoom->setEnabled(false);
  view->addAction(mActionResetZoom);

  mActionFitToScreen = new QAction(iconFromTheme("zoom-fit-best"), tr("Fit to Screen"), this);
  mTranslatable.push_back({mActionFitToScreen, "Fit to Screen"});
  mActionFitToScreen->setEnabled(false);
  view->addAction(mActionFitToScreen);

  view->addSeparator();

  mActionToggleToasts = new QAction(iconFromTheme("notifications"), tr("Minimize Toasts"), this);
  mTranslatable.push_back({mActionToggleToasts, "Minimize Toasts"});
  mActionToggleToasts->setCheckable(true);
  mActionToggleToasts->setChecked(false);
  connect(mActionToggleToasts, &QAction::toggled,
          [this](bool toggled) { mActionToggleToasts->setIcon(iconFromTheme(toggled ? "notifications-disabled" : "notifications")); });
  view->addAction(mActionToggleToasts);

  view->addSeparator();

  QMenu* showMenu = view->addMenu(tr("Show/Hide"));
  showMenu->setIcon(iconFromTheme("view-visible"));
  mTranslatable.push_back({showMenu, "Show/Hide"});
  view->addMenu(showMenu);

  mOpenComponentsPanel = new QAction(tr("Components panel"), this);
  mOpenComponentsPanel->setIcon(iconFromTheme("view-visible"));
  mTranslatable.push_back({mOpenComponentsPanel, "Components panel"});
  showMenu->addAction(mOpenComponentsPanel);
  connect(mOpenComponentsPanel, &QAction::triggered, [this] { togglePanelVisibility(mLeftPanel, mOpenComponentsPanel); });

  mOpenInfoPanel = new QAction(tr("Information panel"), this);
  mOpenInfoPanel->setIcon(iconFromTheme("view-visible"));
  mTranslatable.push_back({mOpenInfoPanel, "Information panel"});
  showMenu->addAction(mOpenInfoPanel);
  connect(mOpenInfoPanel, &QAction::triggered, [this] { togglePanelVisibility(mBottomContainer, mOpenInfoPanel); });

  mOpenPropertiesPanel = new QAction(tr("Properties panel"), this);
  mOpenPropertiesPanel->setIcon(iconFromTheme("view-visible"));
  mTranslatable.push_back({mOpenPropertiesPanel, "Properties panel"});
  showMenu->addAction(mOpenPropertiesPanel);
  connect(mOpenPropertiesPanel, &QAction::triggered, [this] { togglePanelVisibility(mRightPanel, mOpenPropertiesPanel); });

  mSpecialTabsMenu = view->addMenu(iconFromTheme("special-effects-symbolic"), tr("Special tabs"));
  mTranslatable.push_back({mSpecialTabsMenu, "Special tabs"});
  view->addMenu(mSpecialTabsMenu);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* diagram = mMenuBar->addMenu(tr("Diagram"));
  mTranslatable.push_back({diagram, "Diagram"});

  mActionAutoRoute = new QAction(tr("Auto-route"), this);
  diagram->addAction(mActionAutoRoute);

  // ----------------------------------------------------------
  // Tools menu
  QMenu* tools = mMenuBar->addMenu(tr("Tools"));
  mTranslatable.push_back({tools, "Tools"});

  mGeneratorMenu = new QMenu(tr("Generator"));
  mGeneratorMenu->setIcon(iconFromTheme("tool-magic-symbolic"));
  mTranslatable.push_back({mGeneratorMenu, "Generator"});
  tools->addMenu(mGeneratorMenu);

  mActionGenerate = new QAction(iconFromTheme("document-edit-decrypt-verify"), tr("Verify"), this);
  mTranslatable.push_back({mActionGenerate, "Verify"});
  tools->addAction(mActionGenerate);

  mActionSimulate = new QAction(iconFromTheme("exaile-play"), tr("Simulate"), this);
  mTranslatable.push_back({mActionSimulate, "Simulate"});
  tools->addAction(mActionSimulate);

  mActionToggleGrid = new QAction(iconFromTheme("grid-rectangular"), tr("Toggle Grid"), this);
  mTranslatable.push_back({mActionToggleGrid, "Toggle Grid"});
  mActionToggleGrid->setEnabled(false);
  tools->addAction(mActionToggleGrid);

  mActionToggleSnapToGrid = new QAction(iconFromTheme("snap"), tr("Toggle Snap to Grid"), this);
  mTranslatable.push_back({mActionToggleSnapToGrid, "Toggle Snap to Grid"});
  mActionToggleSnapToGrid->setEnabled(false);
  tools->addAction(mActionToggleSnapToGrid);

  // ----------------------------------------------------------
  // Help menu
  QMenu* help = mMenuBar->addMenu(tr("Help"));
  mTranslatable.push_back({help, "Help"});

  mActionDocumentation = new QAction(iconFromTheme("help-browser"), tr("Documentation"), this);
  mTranslatable.push_back({mActionDocumentation, "Documentation"});
  mActionDocumentation->setEnabled(false);
  help->addAction(mActionDocumentation);

  mActionQuickStartGuide = new QAction(iconFromTheme("show-guides"), tr("Quick Start Guide"), this);
  mTranslatable.push_back({mActionQuickStartGuide, "Quick Start Guide"});
  mActionQuickStartGuide->setEnabled(false);
  help->addAction(mActionQuickStartGuide);

  mOpenAllSettings = new QAction(iconFromTheme("preferences-system"), tr("Open All Settings"), this);
  mTranslatable.push_back({mOpenAllSettings, "Open All Settings"});
  help->addAction(mOpenAllSettings);

  mActionShortcuts = new QAction(iconFromTheme("help-keyboard-shortcuts"), tr("Shortcuts"), this);
  mTranslatable.push_back({mActionShortcuts, "Shortcuts"});
  mActionShortcuts->setEnabled(false);
  help->addAction(mActionShortcuts);

  mActionReportIssue = new QAction(iconFromTheme("tools-report-bug"), tr("Report Issue"), this);
  mTranslatable.push_back({mActionReportIssue, "Report Issue"});
  mActionReportIssue->setEnabled(false);
  help->addAction(mActionReportIssue);

  mAboutAction = new QAction(iconFromTheme("help-about"), tr("About"), this);
  mTranslatable.push_back({mAboutAction, "About"});
  help->addAction(mAboutAction);

  setMenuBar(mMenuBar);
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
      mInfoText->resize(mInfoText->width(), height);
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
      // No need for a minimum height
      mPropertiesTab->resize(mPropertiesTab->width(), 450);

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
        LOG_WARNING("Unsupported widget: {}", item.widget->metaObject()->className());
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
    action->setIcon(iconFromTheme("view-visible"));
  }
  else
  {
    panel->hide();
    action->setIcon(iconFromTheme("view-hidden"));
  }
}
