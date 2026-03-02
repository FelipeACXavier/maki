#include "settings_dialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "logging.h"
#include "style_helpers.h"
#include "theme.h"
#include "widget_factory.h"

SettingsDialog::SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent)
    : BaseDialog(title, 1.4, 0.7, parent)
    , mSettingsManager(manager)
{
  auto* mainLayout = new QHBoxLayout();
  mainLayout->setSpacing(0);

  connect(mSettingsManager.get(), &SettingsManager::themeChanged, this, &BaseDialog::onThemeChanged);

  // Left: navigation list
  mPageSelector = new QTreeWidget(this);
  mPageSelector->setObjectName("SettingsList");
  mPageSelector->setHeaderHidden(true);
  mPageSelector->setIconSize(QSize(16, 16));
  mPageSelector->setIndentation(14);
  mPageSelector->setRootIsDecorated(true);
  mPageSelector->setItemsExpandable(true);
  mPageSelector->setExpandsOnDoubleClick(false);  // single click selects
  mPageSelector->setFocusPolicy(Qt::NoFocus);
  mPageSelector->setUniformRowHeights(true);

  // Right: stacked mPages
  mPages = new QStackedWidget(this);

  // Add categories
  LOG_WARN_ON_FAILURE(createGeneralPage());
  LOG_WARN_ON_FAILURE(createAppearancePage());
  LOG_WARN_ON_FAILURE(createGenerationPage());
  LOG_WARN_ON_FAILURE(createPluginPages());

  mainLayout->addWidget(mPageSelector, 1);
  mainLayout->addWidget(mPages, 4);

  // Select page when user clicks an item
  connect(mPageSelector, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* current, QTreeWidgetItem* /* previous */) {
    if (!current)
      return;

    // Non-leaf category items may not map to a page.
    const auto pageIndex = current->data(0, Qt::UserRole).toInt();
    if (pageIndex >= 0)
      mPages->setCurrentIndex(pageIndex);
  });

  layout()->addLayout(mainLayout);

  auto buttonBox = createButtons("Apply", "Close");
  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::apply);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  apply();
}

SettingsDialog::SelectorPage SettingsDialog::addPage(const QString& pageName, const QString& iconName, std::function<void()> resetCallback, QTreeWidgetItem* parent)
{
  QWidget* page = new QWidget;
  page->setObjectName("SettingsPage");

  QVBoxLayout* layout = new QVBoxLayout(page);
  layout->setContentsMargins(6, 6, 6, 6);

  auto* headerRow = new QHBoxLayout();
  headerRow->setContentsMargins(10, 0, 10, 0);

  QLabel* titleIcon = new QLabel();
  addIcon(titleIcon, iconName);

  QLabel* title = new QLabel(pageName);
  title->setObjectName("PageTitle");

  auto* resetButton = new QPushButton(page);
  resetButton->setObjectName("TextAndIcon");
  resetButton->setText(tr(" Reset"));
  resetButton->setToolTip(tr("Reset settings for this page"));
  addIcon(resetButton, ":/icons/reset.svg");

  connect(resetButton, &QPushButton::pressed, resetCallback);

  headerRow->addWidget(titleIcon);
  headerRow->addWidget(title);
  headerRow->addStretch();
  headerRow->addWidget(resetButton);

  QFrame* line = new QFrame(page);
  line->setObjectName("HLine");
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  // Content layout
  auto contentLayout = new QVBoxLayout();
  contentLayout->setContentsMargins(20, 5, 5, 5);
  contentLayout->setObjectName("ContentArea");

  layout->addLayout(headerRow);
  layout->addWidget(line);
  layout->addLayout(contentLayout);

  // Add page to the collection of pages
  int index = mPages->addWidget(page);

  // Update the selector with new page)
  auto* selector = parent == nullptr ? new QTreeWidgetItem(mPageSelector) : new QTreeWidgetItem(parent);
  selector->setText(0, pageName);
  selector->setData(0, Qt::UserRole, index);
  mIcons.append({mPageSelector, iconName, 0, Config::FOREGROUND, mPageSelector->indexFromItem(selector)});

  return SelectorPage{selector, page};
}

VoidResult SettingsDialog::createGeneralPage()
{
  auto generalSettings = mSettingsManager->general();
  auto [selector, page] = addPage(tr("General"), ":/icons/general.svg", [this] {
    auto defaultSettings = GeneralSettings();

    mAutosaveMinutes->setValue(defaultSettings.autosaveIntervalMinutes);
    mAutosaveEnabled->setValue(defaultSettings.autosaveEnabled);
    mRecentHistorySize->setValue(defaultSettings.recentHistorySize);
    mRestoreLastSession->setValue(defaultSettings.restoreLastSession);
    mConfirmOnClose->setValue(defaultSettings.confirmOnCloseWithExecution);
    mEnableDebugLogs->setValue(defaultSettings.enableDebugLogs);

    mSettingsManager->setGeneral(defaultSettings);
  });

  mAutosaveMinutes = new maki::SpinWidget(tr("Autosave interval"), generalSettings.autosaveIntervalMinutes, page, 1, 120);
  mAutosaveMinutes->addDescription("Between 1 and 120 minutes");
  mAutosaveMinutes->setSuffix(" minutes");
  mAutosaveEnabled = new maki::BooleanWidget(tr("Enable autosave"), generalSettings.autosaveEnabled, page);

  auto autoSaveLayout = new maki::WidgetGroup(tr("Autosave"), page);
  autoSaveLayout->addWidget(mAutosaveEnabled);
  autoSaveLayout->addWidget(mAutosaveMinutes);

  mRecentHistorySize = new maki::IntegerWidget(tr("Recent History Size"), QString("%1").arg(generalSettings.recentHistorySize), page, INT32_MIN, INT32_MAX);

  mRestoreLastSession = new maki::BooleanWidget(tr("Restore last session on startup"), generalSettings.restoreLastSession, page);
  mConfirmOnClose = new maki::BooleanWidget(tr("Confirm before closing editor with running execution"), generalSettings.confirmOnCloseWithExecution, page);
  mShowWelcomeMessage = new maki::BooleanWidget(tr("Show welcome message"), generalSettings.showWelcomeMessage, page);

  auto closingLayout = new maki::WidgetGroup(tr("Opening/Closing"), page);
  closingLayout->addWidget(mRestoreLastSession);
  closingLayout->addWidget(mConfirmOnClose);
  closingLayout->addWidget(mShowWelcomeMessage);
  closingLayout->addWidget(mRecentHistorySize);

  mEnableDebugLogs = new maki::BooleanWidget(tr("Enable debug logs"), generalSettings.enableDebugLogs, page);
  auto logLayout = new maki::WidgetGroup(tr("Logging and Notifications"), page);
  logLayout->addWidget(mEnableDebugLogs);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(autoSaveLayout);
  layout->addWidget(closingLayout);
  layout->addWidget(logLayout);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createAppearancePage()
{
  auto appearance = mSettingsManager->appearance();
  auto [selector, page] = addPage(tr("Appearance"), ":/icons/appearance.svg", [this] {
    auto defaultSettings = AppearanceSettings();

    mThemeCombo->setValue(defaultSettings.theme);
    mNativeMenuBar->setValue(defaultSettings.nativeMenuBar);
    mUiScale->setValue(defaultSettings.uiScalePercent);
    mNodeCornerRadius->setValue(defaultSettings.nodeCornerRadius);
    mShowGrid->setValue(defaultSettings.showCanvasGrid);

    mSettingsManager->setAppearance(defaultSettings);
  });

  // First entry: system theme (no QSS)
  mThemeCombo = new maki::SelectorWidget(tr("Theme"), page);
  mThemeCombo->addItem(tr("System theme"), "system");

  // Then all discovered themes
  for (const Config::ThemeInfo& info : mSettingsManager->availableThemes())
  {
    QString label = info.displayName;
    if (info.isUser)
      label += tr(" (user)");

    mThemeCombo->addItem(label, info.id);
  }
  mThemeCombo->setValue(appearance.theme);

  mNativeMenuBar = new maki::BooleanWidget(tr("Use native menubar"), appearance.nativeMenuBar, page);

  auto themeLayout = new maki::WidgetGroup(tr("Theming"), page);
  themeLayout->addWidget(mThemeCombo);
  themeLayout->addWidget(mNativeMenuBar);

  auto colorSelector = new maki::ColorWidget(tr("Test color"), "#A52429", page);
  themeLayout->addWidget(colorSelector);

  mIcons.append({colorSelector->widget(), Config::getValueFromTheme("@eyedropper_icon").toString()});

  mUiScale = new maki::SpinWidget(tr("UI scale"), appearance.uiScalePercent, page, 80, 200);
  mUiScale->setSuffix(tr(" %"));

  mNodeCornerRadius = new maki::SpinWidget(tr("Node corner radius"), appearance.nodeCornerRadius, page, 0, 30);
  mNodeCornerRadius->setSuffix(tr(" pixels"));

  mShowGrid = new maki::BooleanWidget(tr("Show canvas grid"), appearance.showCanvasGrid, page);

  auto editorLayout = new maki::WidgetGroup(tr("Theming"), page);
  editorLayout->addWidget(mUiScale);
  editorLayout->addWidget(mShowGrid);
  editorLayout->addWidget(mNodeCornerRadius);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(themeLayout);
  layout->addWidget(editorLayout);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createGenerationPage()
{
  auto generation = mSettingsManager->generation();
  auto [selector, page] = addPage(tr("Generation"), ":/icons/generator.svg", [this] {
    mSettingsManager->setGeneration(GenerationSettings());
  });

  auto* pathRow = new QWidget(page);

  mGenerationDirEdit = new maki::StringWidget(tr("Generation output folder"), generation.generationDir, pathRow);
  mGenerationDirEdit->addDescription(tr("\"/generated/<plugin name>\" will be appended to this path"));

  mGenerationBrowseBtn = new maki::ButtonWidget("...", pathRow);
  mGenerationBrowseBtn->setToolTip(tr("Choose folder"));

  auto* rowLayout = new QHBoxLayout(pathRow);
  rowLayout->setContentsMargins(0, 0, 0, 0);

  rowLayout->addWidget(mGenerationDirEdit, /*stretch=*/1);
  rowLayout->addWidget(mGenerationBrowseBtn);

  // hook up buttons
  connect(mGenerationBrowseBtn, &maki::ButtonWidget::valueChanged, this, [this] {
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Select default generation folder"));
    if (!dir.isEmpty())
      mGenerationDirEdit->setValue(dir);
  });

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(pathRow);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createPluginPages()
{
  // -----------------------------------------------------------------
  // Load Plugin settings
  mPluginSettings = mSettingsManager->plugins();
  LOG_DEBUG("Loaded from settings with %d plugins", mPluginSettings.size());

  // Add top level plugin page
  auto [topSelector, topPage] = addPage(tr("Plugins"), ":/icons/plugin.svg", [this] {
    // mSettingsManager->setGeneration(GenerationSettings());
  });

  // -------------------------------------------------------------------------
  // Plugins table
  auto* tableLabel = new QLabel(tr("Installed plugins"), topPage);

  QTableView* table = new QTableView(topPage);
  QStandardItemModel* model = new QStandardItemModel(0, 3);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Version"));
  model->setHorizontalHeaderItem(2, new QStandardItem("Enabled"));

  table->setModel(model);

  table->verticalHeader()->setVisible(false);
  table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
  table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  table->setSelectionMode(QAbstractItemView::ExtendedSelection);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setAlternatingRowColors(true);

  // -------------------------------------------------------------------------
  // Add new plugin
  auto* buttonRow = new QHBoxLayout();

  auto* addBtn = new maki::ButtonWidget("Add", topPage);
  addBtn->setFixedWidth(200);
  addIcon(addBtn, ":/icons/plus.svg");

  auto* removeBtn = new maki::ButtonWidget("Remove", topPage);
  addIcon(removeBtn, ":/icons/clear.svg");
  removeBtn->setFixedWidth(200);
  removeBtn->setEnabled(false);

  connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, topPage, [=]() {
    removeBtn->setEnabled(table->selectionModel()->hasSelection());
  });

  buttonRow->addStretch();
  buttonRow->addWidget(addBtn);
  buttonRow->addWidget(removeBtn);

  // -------------------------------------------------------------------------
  // Go through all the plugins and build the page
  for (const auto& plugin : mPluginSettings)
  {
    auto pluginId = plugin.name;
    auto settings = plugin.settings;

    int newRow = model->rowCount();
    model->insertRow(newRow);

    QStandardItem* nameItem = new QStandardItem(pluginId);
    nameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    model->setItem(newRow, 0, nameItem);

    QStandardItem* versionItem = new QStandardItem(plugin.version.toString());
    versionItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    model->setItem(newRow, 1, versionItem);

    QStandardItem* item = new QStandardItem(true);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    model->setItem(newRow, 2, item);

    auto [selector, page] = addPage(pluginId, ":/icons/plugin.svg", [] {
      // mSettingsManager->setGeneration(GenerationSettings());
    },
                                    topSelector);

    QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
    layout->setSpacing(2);

    for (const auto& setting : settings)
    {
      if (setting.getType() == Types::PropertyTypes::INTEGER)
      {
        int min = setting.getMetadata().contains("min") ? setting.getMetadata()["min"].toInt() : INT32_MIN;
        int max = setting.getMetadata().contains("max") ? setting.getMetadata()["max"].toInt() : INT32_MAX;
        auto* field = new maki::IntegerWidget(setting.getLabel(), setting.getDefaultValue().toString(), page, min, max);
        field->addDescription(setting.getDescription());

        connect(field, &maki::IntegerWidget::valueChanged, this, [this, pluginId, setting](const int value) {
          updatePluginSetting(pluginId, setting.getKey(), value);
        });

        layout->addWidget(field);
      }
      else if (setting.getType() == Types::PropertyTypes::BOOLEAN)
      {
        auto* field = new maki::BooleanWidget(setting.getLabel(), setting.getDefaultValue().toBool(), page);
        field->addDescription(setting.getDescription());

        connect(field, &maki::BooleanWidget::valueChanged, this, [this, pluginId, setting](const int value) {
          updatePluginSetting(pluginId, setting.getKey(), value);
        });

        layout->addWidget(field);
      }
    }

    // Make sure the widgets are pushed up
    layout->addStretch();
  }

  QVBoxLayout* layout = topPage->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(tableLabel);
  layout->addWidget(table);
  layout->addLayout(buttonRow);
  layout->addStretch();

  return VoidResult();
}

void SettingsDialog::saveToSettings()
{
  GeneralSettings general;
  general.autosaveEnabled = mAutosaveEnabled->getValue();
  general.enableDebugLogs = mEnableDebugLogs->getValue();
  general.recentHistorySize = mRecentHistorySize->getValue();
  general.showWelcomeMessage = mShowWelcomeMessage->getValue();
  general.restoreLastSession = mRestoreLastSession->getValue();
  general.autosaveIntervalMinutes = mAutosaveMinutes->getValue();
  general.confirmOnCloseWithExecution = mConfirmOnClose->getValue();

  AppearanceSettings appearance;
  appearance.theme = mThemeCombo->getValue();
  appearance.uiScalePercent = mUiScale->getValue();
  appearance.showCanvasGrid = mShowGrid->getValue();
  appearance.nativeMenuBar = mNativeMenuBar->getValue();
  appearance.nodeCornerRadius = mNodeCornerRadius->getValue();

  GenerationSettings generation;
  generation.generationDir = mGenerationDirEdit->getValue();

  mSettingsManager->setGeneral(general);
  mSettingsManager->setAppearance(appearance);
  mSettingsManager->setGeneration(generation);
  mSettingsManager->setPlugins(mPluginSettings);
}

void SettingsDialog::apply()
{
  saveToSettings();
  updateIconTheme(mIcons);
}

void SettingsDialog::updatePluginSetting(const QString& pluginId, const QString& key, QVariant value)
{
  for (auto& plugin : mPluginSettings)
  {
    if (plugin.name != pluginId)
      continue;

    for (auto& set : plugin.settings)
      if (set.getKey() == key)
      {
        set.setValue(value);
        return;
      }
  }
}
