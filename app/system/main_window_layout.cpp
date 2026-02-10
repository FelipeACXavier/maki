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
#include <QSplitter>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QUndoGroup>
#include <QVBoxLayout>

// Custom widgets
#include "app_configs.h"
#include "logging.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "theme.h"
#include "widgets/behaviour_menu.h"
#include "widgets/properties/fields_menu.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/system_menu.h"

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
  mLeftPanel = new QTabWidget();

  mStructureTab = new QWidget();
  QVBoxLayout* structureLayout = new QVBoxLayout(mStructureTab);
  mStructureToolBox = new QToolBox(mStructureTab);
  structureLayout->addWidget(mStructureToolBox);
  mStructureTab->setLayout(structureLayout);
  mLeftPanel->addTab(mStructureTab, tr("Structure"));

  mBehaviourTab = new QWidget();
  QVBoxLayout* behaviourLayout = new QVBoxLayout(mBehaviourTab);
  mBehaviourToolBox = new QToolBox(mBehaviourTab);
  behaviourLayout->addWidget(mBehaviourToolBox);
  mBehaviourTab->setLayout(behaviourLayout);
  mLeftPanel->addTab(mBehaviourTab, tr("Behaviour"));

  mLeftPanel->setTabToolTip(0, tr("Structure"));
  mLeftPanel->setTabToolTip(1, tr("Component behaviour"));

  mIcons.append({mLeftPanel->tabBar(), ":/icons/structure.svg", 0});
  mIcons.append({mLeftPanel->tabBar(), ":/icons/behaviour.svg", 1});

  mLeftPanel->tabBar()->setIconSize(QSize(18, 18));

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

  // ---------------------------------------------
  mGenerationButton = new QPushButton("");
  mIcons.append({mGenerationButton, ":/icons/verify.svg"});

  mGenerationButton->setToolTip("Verify system");
  mGenerationButton->setToolTipDuration(2000);
  mGenerationButton->setFixedSize(30, 30);

  headerLayout->addWidget(mGenerationButton);

  // ----------------------------------------------------------------
  mSimulateButton = new QPushButton("");
  mIcons.append({mSimulateButton, ":/icons/play.svg"});
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
  mIcons.append({mDeployButton, ":/icons/deploy.svg"});

  mDeployButton->setToolTip("Deploy program to selected application");
  mDeployButton->setToolTipDuration(2000);
  mDeployButton->setFixedSize(30, 30);

  headerLayout->addWidget(mDeployButton);

  // ---------------------------------------------
  mCanvasPanel = new QTabWidget();
  mCanvasPanel->setTabsClosable(true);
  mCanvasPanel->setMovable(true);

  CanvasView* canvasView = new CanvasView();

  mCanvasPanel->addTab(canvasView, "System view");
  mCanvasPanel->setCurrentWidget(canvasView);

  // Remove the close button from the system tab
  mCanvasPanel->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

  // Top right buttons
  QWidget* corner = new QWidget();
  auto* layout = new QHBoxLayout(corner);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(4);

  // First button
  mProcessTabButton = new QPushButton(corner);
  mProcessTabButton->setObjectName("MainWindowButton");
  mProcessTabButton->setToolTip(tr("Show process tab"));
  connect(mProcessTabButton, &QPushButton::pressed, [this] { updateProperty(mProcessTabButton, Config::HAS_ACTIVITY, false); });
  mIcons.append({mProcessTabButton, ":/icons/terminal.svg"});

  // Second button
  mBrowserTabButton = new QPushButton(corner);
  mBrowserTabButton->setObjectName("MainWindowButton");
  mBrowserTabButton->setToolTip(tr("Show simulation tab"));
  connect(mBrowserTabButton, &QPushButton::pressed, [this] { updateProperty(mBrowserTabButton, Config::HAS_ACTIVITY, false); });
  mIcons.append({mBrowserTabButton, ":/icons/display.svg"});

  layout->addWidget(mProcessTabButton);
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

  mBottomPanel = new QTabWidget();
  mBottomPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mBottomPanel->setMinimumHeight(320);

  // ===================================================================
  QWidget* infoContainer = new QWidget(mBottomPanel);
  QVBoxLayout* infoLayout = new QVBoxLayout(infoContainer);
  infoLayout->setContentsMargins(2, 2, 2, 2);
  infoLayout->setSpacing(0);

  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(Fonts::Property);
  mInfoText->setHtml(
      "<table width='100%'><tr>"
      "  <td style='vertical-align:top;'>"
      "    <h2>Welcome to " +
      Config::APPLICATION_NAME +
      "</h2>"
      "    <p>MAKI is a low-code platform that helps you design and orchestrate your systems.</p>"
      "    <p><b>To get started:</b></p>"
      "    <ul>"
      "      <li>Drag a structural block from the palette onto the Canvas.</li>"
      "      <li>Right-click and select \"Edit behavior\".</li>"
      "      <li>Connect blocks to define the data and control flow.</li>"
      "      <li>Double-click a block to inspect or adjust its properties.</li>"
      "    </ul>"
      "    <p>For extra help, hover over any control to see a tooltip, or explore the <b>Help</b> menu.</p>"
      "    <p style='color:#888; font-size: 16px'>"
      "      Tip: You can always reset the layout or theme from the <b>Settings</b> menu if things get messy."
      "    </p>"
      "  </td>"
      // IMAGE COLUMN
      "  <td style='text-align: right; vertical-align: top; padding-top: 10px; padding-right: 10px'>"
      "    <img src=':/app_icons/maki.png' width='100' style='border-radius:80px; box-shadow: 0px 3px 8px rgba(0,0,0,0.25);' />"
      "  </td>"
      "</tr></table>");

  infoLayout->addWidget(mInfoText);

  mBottomPanel->addTab(infoContainer, tr("Info"));
  mIcons.append({mBottomPanel->tabBar(), ":/icons/info.svg", 0});

  buildLogTab();
  mIcons.append({mBottomPanel->tabBar(), ":/icons/logs.svg", 1});

  mCentralSplitter->addWidget(mBottomPanel);
  mCentralSplitter->setCollapsible(0, false);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowLayout::buildRightPanel()
{
  mRightPanel = new QSplitter(Qt::Vertical);
  mRightPanel->setMinimumWidth(250);
  mRightPanel->setMaximumWidth(400);

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

  mNavigationTab->addTab(mSystemMenu, tr("System"));

  mFlowMenu = new FlowMenu(mNavigationTab);

  QTreeWidgetItem* systemFlows = new QTreeWidgetItem(mFlowMenu);
  systemFlows->setText(0, tr("System flows"));
  mFlowMenu->addTopLevelItem(systemFlows);

  QTreeWidgetItem* componentFlows = new QTreeWidgetItem(mFlowMenu);
  componentFlows->setText(0, tr("Component flows"));
  mFlowMenu->addTopLevelItem(componentFlows);

  mNavigationTab->addTab(mFlowMenu, tr("Flow"));

  mIcons.append({mNavigationTab->tabBar(), ":/icons/system.svg", 0});
  mIcons.append({mNavigationTab->tabBar(), ":/icons/flows.svg", 1});

  // ----------------------------------------------------------------------
  // Properties Menu
  mPropertiesTab = new QTabWidget();

  mPropertiesMenu = new PropertiesMenu(mPropertiesTab);
  mPropertiesTab->addTab(mPropertiesMenu, tr("Properties"));

  // mFieldsMenu = new FieldsMenu(mPropertiesTab);
  // mPropertiesTab->addTab(mFieldsMenu, tr("Events"));

  // mBehaviourMenu = new BehaviourMenu(mPropertiesTab);
  // mPropertiesTab->addTab(mBehaviourMenu, tr("Behaviour"));

  mIcons.append({mPropertiesTab->tabBar(), ":/icons/properties.svg", 0});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/fields.svg", 1});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/behaviour.svg", 2});

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

  mActionNew = new QAction(tr("New"), this);
  file->addAction(mActionNew);

  mActionOpen = new QAction(tr("Open"), this);
  file->addAction(mActionOpen);

  mActionSave = new QAction(tr("Save"), this);
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(tr("Save As"), this);
  file->addAction(mActionSaveAs);

  // ----------------------------------------------------------
  // Edit menu
  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  mActionUndo = mUndoGroup->createUndoAction(this, tr("&Undo"));
  edit->addAction(mActionUndo);

  mActionRedo = mUndoGroup->createRedoAction(this, tr("&Redo"));
  edit->addAction(mActionRedo);

  // ----------------------------------------------------------
  // View menu
  QMenu* view = mMenuBar->addMenu(tr("View"));
  mOpenComponentsPanel = new QAction(tr("Components panel"), this);
  view->addAction(mOpenComponentsPanel);

  mOpenPropertiesPanel = new QAction(tr("Properties panel"), this);
  view->addAction(mOpenPropertiesPanel);

  mOpenInfoPanel = new QAction(tr("Information panel"), this);
  view->addAction(mOpenInfoPanel);

  // ----------------------------------------------------------
  // Diagram menu
  QMenu* window = mMenuBar->addMenu(tr("Diagram"));

  mActionGenerate = new QAction(tr("Generate"), this);
  window->addAction(mActionGenerate);

  // ----------------------------------------------------------
  // Settings menu
  QMenu* settings = mMenuBar->addMenu(tr("Settings"));
  mGeneratorMenu = new QMenu(tr("Generator"));
  settings->addMenu(mGeneratorMenu);

  mOpenAllSettings = new QAction(tr("Open all settings"), this);
  settings->addAction(mOpenAllSettings);

  // ----------------------------------------------------------
  // Help menu
  QMenu* help = mMenuBar->addMenu(tr("Help"));
  mAboutAction = new QAction(tr("About"), this);
  help->addAction(mAboutAction);

  setMenuBar(mMenuBar);
}

void MainWindowLayout::buildLogTab()
{
  QWidget* logContainer = new QWidget(mBottomPanel);
  QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  logLayout->setContentsMargins(2, 2, 2, 2);
  logLayout->setSpacing(0);

  // Toolbar
  QToolBar* logToolBar = new QToolBar(logContainer);
  logToolBar->setObjectName("LogToolBar");
  logToolBar->setMovable(false);
  logToolBar->setFloatable(false);
  logToolBar->setFont(Fonts::SmallTab);

  // =======================================================================================
  // Log level selector
  QPushButton* errorButton = new QPushButton("");
  errorButton->setObjectName("TextAndIcon");
  errorButton->setText(" Errors");
  errorButton->setCheckable(true);
  errorButton->setToolTip("View only the errors");
  errorButton->setToolTipDuration(2000);
  mIcons.append({errorButton, ":/icons/error.svg", 0, QColor("red")});

  QPushButton* warningButton = new QPushButton("");
  warningButton->setObjectName("TextAndIcon");
  warningButton->setText(" Warnings");
  warningButton->setCheckable(true);
  warningButton->setToolTip("View only the warnings");
  warningButton->setToolTipDuration(2000);
  mIcons.append({warningButton, ":/icons/warning.svg", 0, QColor("yellow")});

  QStackedWidget* logViews = new QStackedWidget();

  mLogText = new QTextBrowser(logContainer);
  mLogText->setFont(Fonts::MonoSpace);

  mErrorLogText = new QTextBrowser(logContainer);
  mErrorLogText->setFont(Fonts::MonoSpace);

  mWarningLogText = new QTextBrowser(logContainer);
  mWarningLogText->setFont(Fonts::MonoSpace);

  // =======================================================================================
  // Clear button
  QPushButton* clearButton = new QPushButton();
  clearButton->setToolTip("Clear the logs");
  clearButton->setToolTipDuration(2000);
  mIcons.append({clearButton, ":/icons/clear.svg"});

  connect(clearButton, &QPushButton::pressed, this, [this]() {
    mLogText->clear();
    mErrorLogText->clear();
    mWarningLogText->clear();
  });

  // =======================================================================================
  // Set the layout
  QWidget* group = new QWidget();
  QHBoxLayout* layout = new QHBoxLayout(group);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(5);
  layout->setAlignment(Qt::AlignCenter);

  layout->addWidget(errorButton);
  layout->addWidget(warningButton);
  layout->addStretch();
  layout->addWidget(clearButton);

  logToolBar->addWidget(group);

  logViews->addWidget(mLogText);
  logViews->addWidget(mErrorLogText);
  logViews->addWidget(mWarningLogText);

  // TODO: Make these indices less magical
  connect(errorButton, &QPushButton::toggled, this, [logViews, warningButton](bool checked) {
    if (checked)
    {
      // Highest priority
      logViews->setCurrentIndex(1);
    }
    else
    {
      if (warningButton->isChecked())
        logViews->setCurrentIndex(2);
      else
        logViews->setCurrentIndex(0);
    }
  });

  connect(warningButton, &QPushButton::toggled, this, [logViews, errorButton](bool checked) {
    if (checked)
    {
      if (!errorButton->isChecked())
        logViews->setCurrentIndex(2);
    }
    else
    {
      logViews->setCurrentIndex(0);
    }
  });

  // Assemble the complete tab
  logLayout->addWidget(logToolBar);
  logLayout->addSpacing(2);
  logLayout->addWidget(logViews);

  mBottomPanel->addTab(logContainer, tr("Log"));
}

void MainWindowLayout::applyTheme()
{
  if (mLeftPanel)
  {
    mLeftPanel->setObjectName("LeftPanel");
    mLeftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mLeftPanel->tabBar()->setExpanding(true);

    auto tabWidth = Config::getValueFromTheme("@left_tab_width");
    auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
    auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

    int count = mLeftPanel->tabBar()->count();
    if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
    {
      int width = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
      mLeftPanel->setMaximumWidth(width);
      mLeftPanel->setFixedWidth(width);
    }
    else
    {
      LOG_WARNING("Failed to derive left panel size from theme");
    }
  }

  if (mCanvasPanel)
  {
    mCanvasPanel->setObjectName("CanvasPanel");
    mCanvasPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mCanvasPanel->tabBar()->setExpanding(true);
  }

  if (mBottomPanel)
  {
    mBottomPanel->setObjectName("InfoPanel");
    mBottomPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mBottomPanel->tabBar()->setExpanding(true);
  }

  if (mRightPanel)
  {
    int navigationTabWidth = 0;
    int propertiesTabWidth = 0;

    if (mNavigationTab)
    {
      mNavigationTab->setObjectName("RightPanel");
      mNavigationTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mNavigationTab->tabBar()->setExpanding(true);

      auto tabWidth = Config::getValueFromTheme("@right_tab_width");
      auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
      auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

      int count = mNavigationTab->tabBar()->count();
      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        navigationTabWidth = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
        mNavigationTab->setMinimumWidth(navigationTabWidth);
      }
      else
      {
        LOG_WARNING("Failed to derive navigation panel size from theme");
      }
    }

    if (mPropertiesTab)
    {
      mPropertiesTab->setObjectName("RightPanel");
      mPropertiesTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mPropertiesTab->tabBar()->setExpanding(true);

      auto tabWidth = Config::getValueFromTheme("@right_tab_width");
      auto tabPadding = Config::getValueFromTheme("@tab_w_padding");
      auto tabBorderSize = Config::getValueFromTheme("@tab_border_size");

      int count = mPropertiesTab->tabBar()->count();
      if (tabWidth.isValid() && tabPadding.isValid() && tabBorderSize.isValid())
      {
        propertiesTabWidth = (count * tabWidth.toInt()) + (2 * count * tabPadding.toInt()) + (2 * count * tabBorderSize.toInt());
        mPropertiesTab->setMinimumWidth(propertiesTabWidth);
      }
      else
      {
        LOG_WARNING("Failed to derive properties panel size from theme");
      }
    }

    mRightPanel->setMinimumWidth(std::max(navigationTabWidth, propertiesTabWidth));
  }
}

void MainWindowLayout::onThemeChanged(const AppearanceSettings& settings)
{
  updateIconTheme(mIcons);
  mMenuBar->setNativeMenuBar(settings.nativeMenuBar);
}

QWidget* MainWindowLayout::createHeaderComboBox(QComboBox* comboBox, const QString& iconPath, const QString& tooltip)
{
  QWidget* wrapper = new QWidget();
  QHBoxLayout* wLayout = new QHBoxLayout(wrapper);
  wLayout->setContentsMargins(0, 0, 0, 0);
  wLayout->setSpacing(0);

  QLabel* iconLabel = new QLabel();
  iconLabel->setObjectName("HeaderButton");
  iconLabel->setToolTip(tooltip);
  iconLabel->setToolTipDuration(2000);
  mIcons.append({iconLabel, iconPath});

  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  comboBox->setToolTip(tooltip);
  comboBox->setToolTipDuration(2000);
  comboBox->setObjectName("HeaderComboBox");

  wLayout->addWidget(iconLabel);
  wLayout->addWidget(comboBox);

  return wrapper;
}

void MainWindowLayout::toggleGenerationButton(bool running)
{
  auto it = std::find_if(mIcons.begin(), mIcons.end(), [&](const WidgetWithIcon& item) { return item.widget == mGenerationButton; });
  if (it != mIcons.end())
  {
    if (running)
    {
      mGenerationButton->setToolTip("Cancel current generation");
      updateProperty(mProcessTabButton, Config::HAS_ACTIVITY, true);
      it->path = ":/icons/pause.svg";
    }
    else
    {
      mGenerationButton->setToolTip("Run with the selected options");
      updateProperty(mProcessTabButton, Config::HAS_ACTIVITY, false);
      it->path = ":/icons/play.svg";
    }
  }

  updateIconTheme(mIcons);
}

void MainWindowLayout::toggleDeployButton(bool running)
{
}
