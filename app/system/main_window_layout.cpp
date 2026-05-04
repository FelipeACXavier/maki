#include "main_window_layout.h"

#include <qpalette.h>

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
#include "theme.h"
#include "widgets/badged_tab_bar.h"
#include "widgets/badged_tab_widget.h"
#include "widgets/frame.h"
#include "widgets/log_table_widget.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"
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

  mStructureTab = new StyledFrame();
  mStructureTab->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  mStructureTab->setBorderRole(StyledFrame::BorderRole::Mid);
  mStructureTab->setRadius(5);
  mStructureTab->setBorderWidth(1);

  QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
  structureLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  mStructureTab->setLayout(structureLayout);

  mStructureScrollArea = new QScrollArea(this);
  mStructureScrollArea->setWidgetResizable(true);
  mStructureScrollArea->setFrameShape(QFrame::NoFrame);
  mStructureScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mStructureScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mStructureScrollArea->setWidget(mStructureTab);

  auto sindex = mPalette->addTab(mStructureScrollArea, QIcon(":/icons/structure.svg"), tr("Structure"));
  mTranslatable.push_back({mPalette->tabBar(), "Structure", sindex});

  mBehaviourTab = new StyledFrame();
  mBehaviourTab->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  mBehaviourTab->setBorderRole(StyledFrame::BorderRole::Mid);
  mBehaviourTab->setRadius(5);
  mBehaviourTab->setBorderWidth(1);

  QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
  behaviourLayout->setContentsMargins(
      Config::CONTENT_PADDING, Config::CONTENT_PADDING,
      Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  mBehaviourTab->setLayout(behaviourLayout);

  mBehaviourScrollArea = new QScrollArea(this);
  mBehaviourScrollArea->setWidgetResizable(true);
  mBehaviourScrollArea->setFrameShape(QFrame::NoFrame);
  mBehaviourScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mBehaviourScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  mBehaviourScrollArea->setWidget(mBehaviourTab);

  auto bindex = mPalette->addTab(mBehaviourScrollArea, QIcon(":/icons/behaviour.svg"), tr("Behavior"));
  mTranslatable.push_back({mPalette->tabBar(), "Behavior", bindex});

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

  QWidget* header = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setSpacing(5);
  headerLayout->setAlignment(Qt::AlignCenter);
  headerLayout->addSpacing(24);

  // ---------------------------------------------
  mGenerationButton = new QPushButton("");
  mGenerationButton->setIcon(QIcon(":/icons/verify.svg"));
  mGenerationButton->setToolTip("Verify system");
  mGenerationButton->setToolTipDuration(2000);
  mGenerationButton->setFixedSize(30, 30);

  headerLayout->addWidget(mGenerationButton);

  // ----------------------------------------------------------------
  mSimulateButton = new QPushButton("");
  mSimulateButton->setIcon(QIcon(":/icons/play.svg"));
  mSimulateButton->setToolTip("Simulate system");
  mSimulateButton->setToolTipDuration(2000);
  mSimulateButton->setFixedSize(30, 30);

  headerLayout->addWidget(mSimulateButton);

  // ----------------------------------------------------------------
  mGeneratorOption = new QComboBox();

  auto generatorOptionsWrapper = createHeaderComboBox(mGeneratorOption, ":/icons/generator.svg", "Verification plugin");
  headerLayout->addWidget(generatorOptionsWrapper);

  // ----------------------------------------------------------------
  mDeployButton = new QPushButton("");
  mDeployButton->setIcon(QIcon(":/icons/deploy.svg"));

  mDeployButton->setToolTip("Deploy program to selected application");
  mDeployButton->setToolTipDuration(2000);
  mDeployButton->setFixedSize(30, 30);

  headerLayout->addWidget(mDeployButton);

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
  mCanvasPanel = new QTabWidget();

  CanvasView* canvasView = new CanvasView();

  mCanvasPanel->addTab(canvasView, QIcon(":/icons/structure.svg"), "System view");
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
  connect(mBrowserTabButton, &QPushButton::pressed, [this] { updateProperty(mBrowserTabButton, Config::HAS_ACTIVITY, false); });

  layout->addWidget(mBrowserTabButton);

  // Add the whole widget to the corner
  mCanvasPanel->setCornerWidget(corner, Qt::TopRightCorner);

  // =================================================================
  QWidget* canvasContainer = new QWidget();
  QVBoxLayout* canvasLayout = new QVBoxLayout(canvasContainer);
  canvasLayout->setContentsMargins(0, 0, 0, 0);
  canvasLayout->setSpacing(0);

  // Add header + tabs
  canvasLayout->addWidget(header);
  canvasLayout->addWidget(mCanvasPanel);

  // Now add the container to the splitter
  mCentralSplitter->addWidget(canvasContainer);

  auto* bottomContainer = new QWidget();
  QVBoxLayout* bottomLayout = new QVBoxLayout(bottomContainer);
  bottomLayout->setContentsMargins(0, 0, 0, 0);
  bottomLayout->setSpacing(0);

  auto* bottomNavContainer = new QWidget();
  QHBoxLayout* bottomNavLayout = new QHBoxLayout(bottomNavContainer);
  bottomNavLayout->setContentsMargins(0, 0, 0, 0);
  bottomNavLayout->setSpacing(0);

  mBottomNavigation = new oclero::qlementine::NavigationBar(bottomContainer);
  mBottomPanel = new QStackedWidget(bottomContainer);

  bottomNavLayout->addWidget(mBottomNavigation);
  bottomNavLayout->addStretch();
  bottomLayout->addWidget(bottomNavContainer);
  bottomLayout->addWidget(mBottomPanel);

  // ===================================================================
  QWidget* infoContainer = new QWidget(mBottomPanel);
  QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
  infoLayout->setContentsMargins(2, 2, 2, 2);
  infoLayout->setSpacing(0);

  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(Fonts::Property);
  mInfoText->setHtml(createDefaultMessage());

  infoLayout->addWidget(mInfoText);

  mBottomNavigation->addItem(tr("Info"), QIcon(":/icons/info.svg"));
  mBottomPanel->addWidget(infoContainer);

  auto* logContainer = new StyledFrame(mBottomPanel);
  logContainer->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  logContainer->setBorderRole(StyledFrame::BorderRole::None);
  logContainer->setRadius(0);
  logContainer->setBorderWidth(0);

  QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  logLayout->setContentsMargins(2, 2, 2, 2);
  logLayout->setSpacing(0);

  mLogTable = new LogTableWidget(logContainer);
  logLayout->addWidget(mLogTable);

  mBottomNavigation->addItem(tr("Log"), QIcon(":/icons/logs.svg"));
  mBottomPanel->addWidget(logContainer);

  mProcessTab = new ProcessTab(mBottomPanel);
  mBottomNavigation->addItem(tr("Generation"), QIcon(":/icons/terminal.svg"));
  mBottomPanel->addWidget(mProcessTab);

  mBottomPanel->setCurrentIndex(0);
  connect(mBottomNavigation, &oclero::qlementine::NavigationBar::currentIndexChanged, [this]() {
    mBottomPanel->setCurrentIndex(mBottomNavigation->currentIndex());
  });

  mCentralSplitter->addWidget(bottomContainer);
  mCentralSplitter->setCollapsible(0, false);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowLayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);
  mRightPanel->setMinimumWidth(MINIMUM_MENU_WIDTH);
  mRightPanel->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  // ----------------------------------------------------------------------
  // Navigation Menu
  mNavigationTab = new QTabWidget();

  mSystemMenu = new SystemMenu(mNavigationTab);
  mSystemMenu->setColumnCount(2);
  mSystemMenu->setHeaderLabels({tr("Name"), tr("Type")});
  mSystemMenu->header()->setAlternatingRowColors(true);
  mSystemMenu->header()->setSectionResizeMode(0, QHeaderView::Stretch);

  mSystemMenu->setColumnWidth(1, 150);
  mSystemMenu->header()->setStretchLastSection(false);
  mSystemMenu->header()->setSectionResizeMode(1, QHeaderView::Fixed);
  mSystemMenu->header()->setTextElideMode(Qt::ElideRight);
  mSystemMenu->header()->setSectionsMovable(false);

  auto sindex = mNavigationTab->addTab(mSystemMenu, QIcon(":/icons/system.svg"), tr("System"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "System", sindex});

  mFileMenu = new GeneratedFilesPanel(mNavigationTab);
  auto findex = mNavigationTab->addTab(mFileMenu, QIcon(":/icons/rectangle-list.svg"), tr("Files"));
  mTranslatable.push_back({mNavigationTab->tabBar(), "Files", findex});

  // ----------------------------------------------------------------------
  // Properties Menu
  mPropertiesTab = new QTabWidget();
  mPropertiesTab->setMinimumHeight(500);

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

  mActionNew = new QAction(tr("New"), this);
  mTranslatable.push_back({mActionNew, "New"});
  file->addAction(mActionNew);

  mActionOpen = new QAction(tr("Open"), this);
  mTranslatable.push_back({mActionOpen, "Open"});
  file->addAction(mActionOpen);

  mActionOpenRecent = file->addMenu(tr("Open Recent"));
  mTranslatable.push_back({mActionOpenRecent, "Open Recent"});
  mActionOpenRecent->setMaximumWidth(MAXIMUM_MENU_WIDTH);

  file->addSeparator();

  mActionSave = new QAction(tr("Save"), this);
  mTranslatable.push_back({mActionSave, "Save"});
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(tr("Save As"), this);
  mTranslatable.push_back({mActionSaveAs, "Save As"});
  file->addAction(mActionSaveAs);

  file->addSeparator();

  mActionImportLibrary = new QAction(tr("Install library"), this);
  mTranslatable.push_back({mActionImportLibrary, "Install library"});
  file->addAction(mActionImportLibrary);

  mActionInstallPlugin = new QAction(tr("Install plugin"), this);
  mTranslatable.push_back({mActionInstallPlugin, "Install plugin"});
  file->addAction(mActionInstallPlugin);

  file->addSeparator();
  mActionExit = new QAction(tr("Exit"), this);
  mTranslatable.push_back({mActionExit, "Exit"});
  file->addAction(mActionExit);

  // ----------------------------------------------------------
  // Edit menu
  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  mTranslatable.push_back({edit, "Edit"});

  mActionUndo = mUndoGroup->createUndoAction(this, tr("Undo"));
  mTranslatable.push_back({mActionUndo, "Undo"});
  edit->addAction(mActionUndo);

  mActionRedo = mUndoGroup->createRedoAction(this, tr("Redo"));
  mTranslatable.push_back({mActionRedo, "Redo"});
  edit->addAction(mActionRedo);

  edit->addSeparator();

  mActionCopy = new QAction(tr("Copy"), this);
  mTranslatable.push_back({mActionCopy, "Copy"});
  mActionCopy->setEnabled(false);
  edit->addAction(mActionCopy);

  mActionPaste = new QAction(tr("Paste"), this);
  mTranslatable.push_back({mActionPaste, "Paste"});
  mActionPaste->setEnabled(false);
  edit->addAction(mActionPaste);

  mActionCut = new QAction(tr("Cut"), this);
  mTranslatable.push_back({mActionCut, "Cut"});
  mActionCut->setEnabled(false);
  edit->addAction(mActionCut);

  mActionDelete = new QAction(tr("Delete"), this);
  mTranslatable.push_back({mActionDelete, "Delete"});
  mActionDelete->setEnabled(false);
  edit->addAction(mActionDelete);

  // ----------------------------------------------------------
  // View menu
  QMenu* view = mMenuBar->addMenu(tr("View"));
  mTranslatable.push_back({view, "View"});

  mActionZoomIn = new QAction(tr("Zoom In"), this);
  mTranslatable.push_back({mActionZoomIn, "Zoom In"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomOut = new QAction(tr("Zoom Out"), this);
  mTranslatable.push_back({mActionZoomOut, "Zoom Out"});
  mActionZoomOut->setEnabled(false);
  view->addAction(mActionZoomOut);

  mActionResetZoom = new QAction(tr("Reset Zoom"), this);
  mTranslatable.push_back({mActionResetZoom, "Reset Zoom"});
  mActionResetZoom->setEnabled(false);
  view->addAction(mActionResetZoom);

  mActionFitToScreen = new QAction(tr("Fit to Screen"), this);
  mTranslatable.push_back({mActionFitToScreen, "Fit to Screen"});
  mActionFitToScreen->setEnabled(false);
  view->addAction(mActionFitToScreen);

  view->addSeparator();

  mActionZoomIn = new QAction(tr("Toggle Grid"), this);
  mTranslatable.push_back({mActionZoomIn, "Toggle Grid"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  mActionZoomIn = new QAction(tr("Toggle Snap to Grid"), this);
  mTranslatable.push_back({mActionZoomIn, "Toggle Snap to Grid"});
  mActionZoomIn->setEnabled(false);
  view->addAction(mActionZoomIn);

  QMenu* showMenu = view->addMenu(tr("Show/Hide"));
  mTranslatable.push_back({showMenu, "Show/Hide"});
  view->addMenu(showMenu);

  mOpenInfoPanel = new QAction(tr("Information panel"), this);
  mOpenInfoPanel->setIcon(QIcon(":/icons/invisible.svg"));
  mTranslatable.push_back({mOpenInfoPanel, "Information panel"});
  showMenu->addAction(mOpenInfoPanel);
  connect(mOpenInfoPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mBottomPanel, mOpenInfoPanel);
  });

  mOpenComponentsPanel = new QAction(tr("Components panel"), this);
  mOpenComponentsPanel->setIcon(QIcon(":/icons/invisible.svg"));
  mTranslatable.push_back({mOpenComponentsPanel, "Components panel"});
  showMenu->addAction(mOpenComponentsPanel);
  connect(mOpenComponentsPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mLeftPanel, mOpenComponentsPanel);
  });

  mOpenPropertiesPanel = new QAction(tr("Properties panel"), this);
  mOpenPropertiesPanel->setIcon(QIcon(":/icons/invisible.svg"));
  mTranslatable.push_back({mOpenPropertiesPanel, "Properties panel"});
  showMenu->addAction(mOpenPropertiesPanel);
  connect(mOpenPropertiesPanel, &QAction::triggered, [this] {
    togglePanelVisibility(mRightPanel, mOpenPropertiesPanel);
  });

  mSpecialTabsMenu = view->addMenu(tr("Special tabs"));
  mTranslatable.push_back({mSpecialTabsMenu, "Special tabs"});
  view->addMenu(mSpecialTabsMenu);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* window = mMenuBar->addMenu(tr("Diagram"));
  mTranslatable.push_back({window, "Diagram"});

  // mActionAddTask = new QAction(tr("Add Node"), this);
  // view->addAction(mOpenPropertiesPanel);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* tools = mMenuBar->addMenu(tr("Tools"));
  mTranslatable.push_back({tools, "Tools"});

  mGeneratorMenu = new QMenu(tr("Generator"));
  mTranslatable.push_back({mGeneratorMenu, "Generator"});
  tools->addMenu(mGeneratorMenu);

  mActionGenerate = new QAction(tr("Verify"), this);
  mTranslatable.push_back({mActionGenerate, "Verify"});
  window->addAction(mActionGenerate);

  mActionSimulate = new QAction(tr("Simulate"), this);
  mTranslatable.push_back({mActionSimulate, "Simulate"});
  window->addAction(mActionSimulate);

  // ----------------------------------------------------------
  // Settings menu
  // QMenu* settings = mMenuBar->addMenu(tr("Settings"));

  // ----------------------------------------------------------
  // Help menu
  QMenu* help = mMenuBar->addMenu(tr("Help"));
  mTranslatable.push_back({help, "Help"});

  mActionDocumentation = new QAction(tr("Documentation"), this);
  mTranslatable.push_back({mActionDocumentation, "Documentation"});
  mActionDocumentation->setEnabled(false);
  help->addAction(mActionDocumentation);

  mActionQuickStartGuide = new QAction(tr("Quick Start Guide"), this);
  mTranslatable.push_back({mActionQuickStartGuide, "Quick Start Guide"});
  mActionQuickStartGuide->setEnabled(false);
  help->addAction(mActionQuickStartGuide);

  mOpenAllSettings = new QAction(tr("Open All Settings"), this);
  mTranslatable.push_back({mOpenAllSettings, "Open All Settings"});
  help->addAction(mOpenAllSettings);

  mActionShortcuts = new QAction(tr("Shortcuts"), this);
  mTranslatable.push_back({mActionShortcuts, "Shortcuts"});
  mActionShortcuts->setEnabled(false);
  help->addAction(mActionShortcuts);

  mActionReportIssue = new QAction(tr("Report Issue"), this);
  mTranslatable.push_back({mActionReportIssue, "Report Issue"});
  mActionReportIssue->setEnabled(false);
  help->addAction(mActionReportIssue);

  mAboutAction = new QAction(tr("About"), this);
  mTranslatable.push_back({mAboutAction, "About"});
  help->addAction(mAboutAction);

  setMenuBar(mMenuBar);
}

int MainWindowLayout::setTabBarWidth(QTabBar* bar, int minWidth, int minBorder, int minPadding)
{
  QFontMetrics fm(bar->font());
  int iconSize = bar->iconSize().width();
  int maxWidth = minWidth;

  // Check the width of all tabs and get the largest
  for (int i = 0; i < bar->count(); ++i)
  {
    int textWidth = fm.horizontalAdvance(bar->tabText(i)) + iconSize + (2 * minPadding);
    maxWidth = qMax(textWidth, maxWidth);
  }

  // Set the width of the tabs
  // applyStyle(bar, QString("QTabBar::tab { width: %1 }").arg(maxWidth));

  int singleBarWidth = (maxWidth) + (2 * minBorder) + (2 * minPadding);

  return bar->count() * singleBarWidth;
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

void MainWindowLayout::onThemeChanged(const AppearanceSettings& settings)
{
  mMenuBar->setNativeMenuBar(settings.nativeMenuBar);
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

  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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
    mGenerationButton->setToolTip("Cancel current generation");
    mGenerationButton->setIcon(QIcon(":/icons/pause.svg"));
  }
  else
  {
    mGenerationSpinner->setSpinning(false);
    mGenerationSpinner->setVisible(false);
    mGenerationButton->setToolTip("Verify system");
    mGenerationButton->setIcon(QIcon(":/icons/verify.svg"));
  }
}

void MainWindowLayout::toggleDeployButton(bool running)
{
  if (running)
  {
    mSimulateButton->setToolTip("Cancel current simulation");
    mSimulateButton->setIcon(QIcon(":/icons/pause.svg"));
  }
  else
  {
    mSimulateButton->setToolTip("Simulate system");
    mSimulateButton->setIcon(QIcon(":/icons/play.svg"));
  }
}

void MainWindowLayout::togglePanelVisibility(QWidget* panel, QAction* action)
{
  if (panel->isHidden())
  {
    panel->show();
    action->setIcon(addIconWithColor(":/icons/invisible.svg", Qt::white));
  }
  else
  {
    panel->hide();
    action->setIcon(addIconWithColor(":/icons/visible.svg", Qt::white));
  }
}
