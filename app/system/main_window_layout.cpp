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
#include <QVBoxLayout>

// Custom widgets
#include "app_configs.h"
#include "style_helpers.h"
#include "system/canvas_view.h"
#include "theme.h"
#include "widgets/behaviour_menu.h"
#include "widgets/properties/fields_menu.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/structure/flow_menu.h"
#include "widgets/structure/system_menu.h"

MainWindowlayout::MainWindowlayout(QWidget* parent)
    : QMainWindow(parent)
{
  buildMainWindow();
  // qApp->installEventFilter(this);
}

void MainWindowlayout::buildMainWindow()
{
  // Central widget
  mCentralWidget = new QWidget(this);
  QHBoxLayout* mainLayout = new QHBoxLayout(mCentralWidget);

  // Main horizontal splitter
  mMainSplitter = new QSplitter(Qt::Horizontal, mCentralWidget);

  buildLeftPanel();
  buildCentralPanel();
  buildRightPanel();

  mainLayout->addWidget(mMainSplitter);
  mCentralWidget->setLayout(mainLayout);
  setCentralWidget(mCentralWidget);

  buildMenuBar();

  applyTheme();
}

void MainWindowlayout::buildLeftPanel()
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

void MainWindowlayout::buildCentralPanel()
{
  mCentralSplitter = new QSplitter(Qt::Vertical);

  QWidget* header = new QWidget();
  QHBoxLayout* headerLayout = new QHBoxLayout(header);
  headerLayout->setContentsMargins(0, 8, 0, 8);  // top/bottom spacing
  headerLayout->setSpacing(5);
  headerLayout->setAlignment(Qt::AlignCenter);

  // ---------------------------------------------
  mGenerationButton = new QPushButton("");
  mIcons.append({mGenerationButton, ":/icons/play.svg"});

  mGenerationButton->setToolTip("Run with the selected options");
  mGenerationButton->setToolTipDuration(2000);
  mGenerationButton->setFixedSize(30, 30);

  headerLayout->addWidget(mGenerationButton);

  // ----------------------------------------------------------------
  mGenerationOption = new QComboBox();
  mGenerationOption->addItem(tr("Generate"), "Generate");
  mGenerationOption->addItem(tr("Verify"), "Verify");
  mGenerationOption->addItem(tr("Simulate"), "Simulate");

  auto generationOptionsWrapper = createHeaderComboBox(mGenerationOption, ":/icons/generator.svg");
  headerLayout->addWidget(generationOptionsWrapper);

  // ----------------------------------------------------------------
  mGeneratorOption = new QComboBox();

  auto generatorOptionsWrapper = createHeaderComboBox(mGeneratorOption, ":/icons/generation_options.svg");
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

  mCanvasPanel->addTab(canvasView, "System");
  mCanvasPanel->setCurrentWidget(canvasView);

  // Remove the close button from the system tab
  mCanvasPanel->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

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
  mBottomPanel->setFixedHeight(320);

  // ===================================================================
  // Info tab
  mInfoText = new QTextBrowser(mBottomPanel);
  mInfoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);
  mInfoText->setFont(Fonts::Property);
  mInfoText->setText(
      "<h2>Welcome to " + Config::APPLICATION_NAME +
      "</h2>"

      "<p>"
      "  MAKI is a low-code platform that helps you design and orchestrate your systems."
      "</p>"
      ""
      "<p><b>To get started:</b></p>"
      "<ul>"
      "  <li>Drag a structural block from the palette onto the Canvas.</li>"
      "  <li>Right-click and select \"Edit behavior\"</li>"
      "  <li>Connect blocks to define the data and control flow.</li>"
      "  <li>Double-click a block to inspect or adjust its properties.</li>"
      "</ul>"
      ""
      "<p>"
      "  For extra help, hover over any control to see a tooltip, or explore the <b>Help</b> menu in the navigation bar."
      "</p>"
      ""
      "<p style='color:#888; font-size: 16px'>"
      "  Tip: You can always reset the layout or theme from the <b>Settings</b> menu if things get messy."
      "</p>");

  mBottomPanel->addTab(mInfoText, tr("Info"));
  mIcons.append({mBottomPanel->tabBar(), ":/icons/info.svg", 0});

  buildLogTab();
  mIcons.append({mBottomPanel->tabBar(), ":/icons/logs.svg", 1});

  mCentralSplitter->addWidget(mBottomPanel);

  mMainSplitter->addWidget(mCentralSplitter);
}

void MainWindowlayout::buildRightPanel()
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

  mFieldsMenu = new FieldsMenu(mPropertiesTab);
  mPropertiesTab->addTab(mFieldsMenu, tr("Events"));

  mBehaviourMenu = new BehaviourMenu(mPropertiesTab);
  mPropertiesTab->addTab(mBehaviourMenu, tr("Behaviour"));

  mIcons.append({mPropertiesTab->tabBar(), ":/icons/properties.svg", 0});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/fields.svg", 1});
  mIcons.append({mPropertiesTab->tabBar(), ":/icons/behaviour.svg", 2});

  mRightPanel->addWidget(mNavigationTab);
  mRightPanel->addWidget(mPropertiesTab);

  mMainSplitter->addWidget(mRightPanel);
}

void MainWindowlayout::buildMenuBar()
{
  // === Menu Bar ===
  mMenuBar = new QMenuBar();

  QMenu* file = mMenuBar->addMenu(tr("File"));

  mActionNew = new QAction(tr("New"), this);
  file->addAction(mActionNew);

  mActionOpen = new QAction(tr("Open"), this);
  file->addAction(mActionOpen);

  mActionSave = new QAction(tr("Save"), this);
  file->addAction(mActionSave);

  mActionSaveAs = new QAction(tr("Save As"), this);
  file->addAction(mActionSaveAs);

  QMenu* edit = mMenuBar->addMenu(tr("Edit"));
  mOpenAllSettings = new QAction(tr("Open all settings"), this);
  edit->addAction(mOpenAllSettings);

  QMenu* view = mMenuBar->addMenu(tr("View"));

  QMenu* window = mMenuBar->addMenu(tr("Diagram"));

  mActionGenerate = new QAction(tr("Generate"), this);
  window->addAction(mActionGenerate);

  QMenu* settings = mMenuBar->addMenu(tr("Settings"));
  mGeneratorMenu = new QMenu(tr("Generator"));
  settings->addMenu(mGeneratorMenu);

  QMenu* help = mMenuBar->addMenu(tr("Help"));

  setMenuBar(mMenuBar);
}

void MainWindowlayout::buildLogTab()
{
  QWidget* logContainer = new QWidget(mBottomPanel);
  QVBoxLayout* logLayout = new QVBoxLayout(logContainer);
  logLayout->setContentsMargins(2, 2, 2, 2);

  // Toolbar
  QToolBar* logToolBar = new QToolBar(logContainer);
  logToolBar->setObjectName("LogToolBar");
  logToolBar->setMovable(false);
  logToolBar->setFloatable(false);
  logToolBar->setFont(Fonts::SmallTab);

  // =======================================================================================
  // Log level selector
  QPushButton* errorButton = new QPushButton("");
  errorButton->setObjectName("CheckableButton");
  errorButton->setCheckable(true);
  errorButton->setToolTip("View only the errors");
  errorButton->setToolTipDuration(2000);
  mIcons.append({errorButton, ":/icons/error.svg"});

  QPushButton* warningButton = new QPushButton("");
  warningButton->setObjectName("CheckableButton");
  warningButton->setCheckable(true);
  warningButton->setToolTip("View only the warnings");
  warningButton->setToolTipDuration(2000);
  mIcons.append({warningButton, ":/icons/warning.svg"});

  QStackedWidget* logViews = new QStackedWidget();

  mLogText = new QTextBrowser(mBottomPanel);
  mLogText->setFont(Fonts::MonoSpace);

  mErrorLogText = new QTextBrowser(mBottomPanel);
  mErrorLogText->setFont(Fonts::MonoSpace);

  mWarningLogText = new QTextBrowser(mBottomPanel);
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
  logLayout->addWidget(logViews);

  mBottomPanel->addTab(logContainer, tr("Log"));
}

void MainWindowlayout::applyTheme()
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

void MainWindowlayout::themeChanged()
{
  for (auto item : mIcons)
  {
    if (item.widget)
    {
      if (auto label = qobject_cast<QLabel*>(item.widget))
      {
        label->setPixmap(applyColorToIcon(item.path, Config::FOREGROUND).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      }
      if (auto button = qobject_cast<QPushButton*>(item.widget))
      {
        button->setIcon(addIconWithColor(item.path, Config::FOREGROUND));
      }
      else if (auto tabBar = qobject_cast<QTabBar*>(item.widget))
      {
        if (item.index < tabBar->count())
          tabBar->setTabIcon(item.index, addIconWithColor(item.path, Config::FOREGROUND));
      }
    }
  }
}

QWidget* MainWindowlayout::createHeaderComboBox(QComboBox* comboBox, const QString& iconPath)
{
  QWidget* wrapper = new QWidget();
  QHBoxLayout* wLayout = new QHBoxLayout(wrapper);
  wLayout->setContentsMargins(0, 0, 0, 0);
  wLayout->setSpacing(0);

  QLabel* iconLabel = new QLabel();
  iconLabel->setObjectName("HeaderButton");
  mIcons.append({iconLabel, iconPath});

  comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  comboBox->setObjectName("HeaderComboBox");

  wLayout->addWidget(iconLabel);
  wLayout->addWidget(comboBox);

  return wrapper;
}