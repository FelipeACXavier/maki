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
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include "app_configs.h"
#include "app_paths.h"
#include "frame.h"
#include "logging.h"
#include "scroll_area.h"
#include "section.h"
#include "style_helpers.h"
#include "widget_factory.h"

SettingsDialog::SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, std::shared_ptr<LanguageManager> languageManager, QWidget* parent)
    : BaseDialog(title, 1.4, 0.7, parent)
    , mSettingsManager(manager)
    , mLanguageManager(languageManager)
{
  auto* mainLayout = new QHBoxLayout();
  mainLayout->setSpacing(0);

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
  mPageSelector->setFixedWidth(size().width() / 5);

  // Right: stacked mPages
  mPages = new QStackedWidget(this);
  mPages->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Add categories
  LOG_WARN_ON_FAILURE(createGeneralPage());
  LOG_WARN_ON_FAILURE(createAppearancePage());
  LOG_WARN_ON_FAILURE(createGenerationPage());
  LOG_WARN_ON_FAILURE(createPluginPages());

  mainLayout->addWidget(mPageSelector);
  mainLayout->addWidget(mPages);

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
  QWidget* page = new QWidget(this);
  page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* layout = new QVBoxLayout(page);
  layout->setContentsMargins(6, 6, 6, 6);

  auto* headerRow = new QHBoxLayout();
  headerRow->setContentsMargins(10, 0, 10, 0);

  auto icon = iconFromTheme(iconName, true);
  auto* titleIcon = new oclero::qlementine::IconWidget(icon, QSize(16, 16), page);
  auto* title = new oclero::qlementine::Label(pageName, page);
  title->setRole(oclero::qlementine::TextRole::H3);

  auto* resetButton = new QPushButton(page);
  resetButton->setText(" " + tr("Reset"));
  resetButton->setToolTip(tr("Reset settings for this page"));
  resetButton->setIcon(QIcon(":/icons/reset.svg"));

  connect(resetButton, &QPushButton::pressed, resetCallback);

  headerRow->addWidget(titleIcon);
  headerRow->addWidget(title);
  headerRow->addStretch();
  headerRow->addWidget(resetButton);

  QFrame* line = new QFrame(page);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  // Content layout
  auto scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto content = new QWidget(this);
  content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

  auto contentLayout = new QVBoxLayout(content);
  contentLayout->setContentsMargins(20, 5, 20, 5);
  contentLayout->setObjectName("ContentArea");

  scrollArea->setWidget(content);

  layout->addLayout(headerRow);
  layout->addWidget(line);
  layout->addWidget(scrollArea);

  // Add page to the collection of pages
  int index = mPages->addWidget(page);

  // Update the selector with new page)
  auto* selector = parent == nullptr ? new QTreeWidgetItem(mPageSelector) : new QTreeWidgetItem(parent);
  selector->setText(0, pageName);
  selector->setData(0, Qt::UserRole, index);
  selector->setIcon(0, icon);

  return SelectorPage{selector, page};
}

VoidResult SettingsDialog::createGeneralPage()
{
  auto generalSettings = mSettingsManager->general();
  auto [selector, page] = addPage(tr("General"), "general", [this] {
    auto defaultSettings = GeneralSettings();

    mAutosaveMinutes->setValue(defaultSettings.autosaveIntervalMinutes);
    mAutosaveEnabled->setValue(defaultSettings.autosaveEnabled);
    mRecentHistorySize->setValue(defaultSettings.recentHistorySize);
    mRestoreLastSession->setValue(defaultSettings.restoreLastSession);
    mConfirmOnClose->setValue(defaultSettings.confirmOnCloseWithExecution);
    mEnableDebugLogs->setValue(defaultSettings.enableDebugLogs);

    mSettingsManager->setGeneral(defaultSettings);
  });

  maki::WidgetAlignment alignment = {
      .type = maki::WidgetAlignment::Type::INLINE,
      .direction = maki::WidgetAlignment::Direction::SPREAD,
      .labelWidth = 300,
  };
  auto languageLayout = new maki::WidgetGroup(tr("Language"), page);
  mLanguageCombo = new maki::SelectorWidget(tr("Set language"), alignment, page);
  for (const LanguageManager::LanguageOption& info : mLanguageManager->availableLanguages())
    mLanguageCombo->addItem(info.label, info.code);

  mLanguageCombo->setValue(generalSettings.language);
  mLanguageCombo->addDescription(tr("Requires a restart to take full effect"));

  languageLayout->addWidget(mLanguageCombo);

  mAutosaveMinutes = new maki::SpinWidget(tr("Autosave interval"), generalSettings.autosaveIntervalMinutes, page, alignment, 1, 120);
  mAutosaveMinutes->addDescription("Between 1 and 120 minutes");
  mAutosaveMinutes->setSuffix(" minutes");
  mAutosaveEnabled = new maki::BooleanWidget(tr("Enable autosave"), generalSettings.autosaveEnabled, alignment, page);

  auto autoSaveLayout = new maki::WidgetGroup(tr("Autosave"), page);
  autoSaveLayout->addWidget(mAutosaveEnabled);
  autoSaveLayout->addWidget(mAutosaveMinutes);

  mRecentHistorySize = new maki::IntegerWidget(tr("Recent History Size"), QString("%1").arg(generalSettings.recentHistorySize), alignment, page, INT32_MIN, INT32_MAX);

  mRestoreLastSession = new maki::BooleanWidget(tr("Restore last session on startup"), generalSettings.restoreLastSession, alignment, page);
  mConfirmOnClose = new maki::BooleanWidget(tr("Confirm before closing editor with running execution"), generalSettings.confirmOnCloseWithExecution, alignment, page);
  mShowWelcomeMessage = new maki::BooleanWidget(tr("Show welcome message"), generalSettings.showWelcomeMessage, alignment, page);

  auto closingLayout = new maki::WidgetGroup(tr("Opening/Closing"), page);
  closingLayout->addWidget(mRestoreLastSession);
  closingLayout->addWidget(mConfirmOnClose);
  closingLayout->addWidget(mShowWelcomeMessage);
  closingLayout->addWidget(mRecentHistorySize);

  mEnableDebugLogs = new maki::BooleanWidget(tr("Enable debug logs"), generalSettings.enableDebugLogs, alignment, page);
  auto logLayout = new maki::WidgetGroup(tr("Logging and Notifications"), page);
  logLayout->addWidget(mEnableDebugLogs);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(languageLayout);
  layout->addWidget(autoSaveLayout);
  layout->addWidget(closingLayout);
  layout->addWidget(logLayout);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createAppearancePage()
{
  auto appearance = mSettingsManager->appearance();
  auto [selector, page] = addPage(tr("Appearance"), "appearance", [this] {
    auto defaultSettings = AppearanceSettings();

    mThemeCombo->setValue(defaultSettings.theme);
    mNativeMenuBar->setValue(defaultSettings.nativeMenuBar);
    mUiScale->setValue(defaultSettings.uiScalePercent);
    mNodeCornerRadius->setValue(defaultSettings.nodeCornerRadius);
    mNumberOfColumns->setValue(defaultSettings.numberOfColumns);
    mShowGrid->setValue(defaultSettings.showCanvasGrid);
    mStartLogTableFilters->setValue(defaultSettings.startLogFilterExpanded);

    mSettingsManager->setAppearance(defaultSettings);
  });

  // First entry: system theme (no QSS)
  mThemeCombo = new maki::SelectorWidget(tr("Theme"), maki::WidgetAlignment::Inline(), page);
  // Then all discovered themes
  for (const auto& info : mSettingsManager->availableThemes())
  {
    QString label = info.meta.name;
    mThemeCombo->addItem(label, label);
  }

  mThemeCombo->setValue(appearance.theme);
  connect(mThemeCombo, &maki::SelectorWidget::valueChanged, [this](const QString& themeName) {
    if (!mThemeEditor)
      return;

    auto theme = mSettingsManager->themeByName(themeName);
    if (theme.IsSuccess())
      mThemeEditor->setTheme(theme.Value());
  });

  auto themeLayout = new maki::WidgetGroup(tr("Theming"), page);
  themeLayout->addWidget(mThemeCombo);
  themeLayout->addWidget(mNativeMenuBar);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
  {
    auto themeResult = mSettingsManager->themeByName(appearance.theme);
    if (themeResult.IsSuccess())
      mTheme = themeResult.Value();

    auto* editorFrame = new StyledFrame(page);
    editorFrame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    editorFrame->setBorderRole(StyledFrame::BorderRole::Mid);
    editorFrame->setRadius(mTheme.borderRadius);
    editorFrame->setBorderWidth(mTheme.borderWidth);
    editorFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* editorLayout = new QVBoxLayout(editorFrame);
    editorLayout->setContentsMargins(4, 4, 4, 4);
    editorFrame->setLayout(editorLayout);

    mThemeEditor = new oclero::qlementine::ThemeEditorWidget(editorFrame);
    mThemeEditor->setDefaultPath(AppPaths::userThemes());

    connect(mThemeEditor, &oclero::qlementine::ThemeEditorWidget::themeChanged, [this](const oclero::qlementine::Theme& theme) {
      LOG_TRACE("Theme updated");
      mTheme = theme;
    });
    connect(mThemeEditor, &oclero::qlementine::ThemeEditorWidget::themeSaved, [this](const QString& path, const oclero::qlementine::Theme& theme) {
      LOG_TRACE("Theme saved: %s", qPrintable(path));
      mThemeCombo->addItem(theme.meta.name, theme.meta.name);
      mSettingsManager->themeCreated(path);
    });
    connect(mThemeEditor, &oclero::qlementine::ThemeEditorWidget::themeLoaded, [this](const QString& path, const oclero::qlementine::Theme& theme) {
      LOG_TRACE("Theme loaded: %s", qPrintable(path));
      mThemeCombo->addItem(theme.meta.name, theme.meta.name);
      mSettingsManager->themeCreated(path);
    });
    mThemeEditor->setTheme(mTheme);

    editorFrame->setFixedHeight(mThemeEditor->sizeHint().height() + mTheme.borderWidth);
    editorLayout->addWidget(mThemeEditor);

    themeLayout->addWidget(editorFrame);
  }

  maki::WidgetAlignment alignment = {
      .type = maki::WidgetAlignment::Type::INLINE,
      .direction = maki::WidgetAlignment::Direction::SPREAD,
      .labelWidth = 300,
  };
  mNativeMenuBar = new maki::BooleanWidget(tr("Use native menubar"), appearance.nativeMenuBar, alignment, page);

  mUiScale = new maki::SpinWidget(tr("UI scale"), appearance.uiScalePercent, page, alignment, 80, 200);
  mUiScale->setSuffix(" %");

  mNodeCornerRadius = new maki::SpinWidget(tr("Node corner radius"), appearance.nodeCornerRadius, page, alignment, 0, 30);
  mNodeCornerRadius->setSuffix(" pixels");

  mNumberOfColumns = new maki::SpinWidget(tr("Number of columns in the node palette"), appearance.numberOfColumns, page, alignment, 0, 4);
  mNumberOfColumns->setSuffix(" " + tr("columns"));
  mNumberOfColumns->addDescription(tr("Note: The width of the palette will not update automatically"));

  mShowGrid = new maki::BooleanWidget(tr("Show canvas grid"), appearance.showCanvasGrid, alignment, page);
  mStartLogTableFilters = new maki::BooleanWidget(tr("Show log table filters on start"), appearance.startLogFilterExpanded, alignment, page);

  auto editorLayout = new maki::WidgetGroup(tr("UI changes"), page);
  editorLayout->addWidget(mUiScale);
  editorLayout->addWidget(mShowGrid);
  editorLayout->addWidget(mStartLogTableFilters);
  editorLayout->addWidget(mNodeCornerRadius);
  editorLayout->addWidget(mNumberOfColumns);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(themeLayout);
  layout->addWidget(editorLayout);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createGenerationPage()
{
  auto generation = mSettingsManager->generation();
  auto [selector, page] = addPage(tr("Generation"), "run-build", [this] {
    mSettingsManager->setGeneration(GenerationSettings());
  });

  auto* pathRow = new QWidget(page);
  mGenerationDirEdit = new maki::StringWidget(tr("Generation output folder"), generation.generationDir, maki::WidgetAlignment::Inline(), pathRow);
  mGenerationDirEdit->addDescription(tr("\"/<plugin name>\" will be appended to this path"));

  if (generation.generationDir != GenerationSettings().generationDir)
    mGenerationDirEdit->setValue(generation.generationDir);

  mGenerationBrowseBtn = new maki::ButtonWidget("...", pathRow);
  mGenerationBrowseBtn->setToolTip(tr("Choose folder"));

  auto* rowLayout = new QHBoxLayout(pathRow);
  rowLayout->setContentsMargins(0, 0, 0, 0);
  rowLayout->setAlignment(Qt::AlignTop);
  rowLayout->addWidget(mGenerationDirEdit, 1, Qt::AlignTop);
  rowLayout->addWidget(mGenerationBrowseBtn, 0, Qt::AlignTop);

  // hook up buttons
  connect(mGenerationBrowseBtn, &maki::ButtonWidget::valueChanged, this, [this] {
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Select default generation folder"));
    if (!dir.isEmpty())
      mGenerationDirEdit->setValue(dir);
  });

  auto group = new maki::WidgetGroup(tr("General"), page);
  group->addWidget(pathRow);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(group);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createPluginPages()
{
  // -----------------------------------------------------------------
  // Load Plugin settings
  mPluginSettings = mSettingsManager->plugins();
  LOG_DEBUG("Loaded from settings with %d plugins", mPluginSettings.plugins.size());

  // Add top level plugin page
  auto [topSelector, topPage] = addPage(tr("Plugins"), "plugins", [] {
    // mSettingsManager->setGeneration(GenerationSettings());
  });

  // -------------------------------------------------------------------------
  // Plugins table
  auto tableLayout = new maki::WidgetGroup(tr("Installed plugins"), topPage);

  QTableView* table = new QTableView(tableLayout);
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
  table->setAlternatingRowColors(false);

  // -------------------------------------------------------------------------
  // Add new plugin
  auto* buttonRow = new QHBoxLayout();

  auto* addBtn = new maki::ButtonWidget("Add", tableLayout);
  addBtn->setFixedWidth(200);
  addBtn->setIcon(QIcon(":/icons/plus.svg"));

  auto* removeBtn = new maki::ButtonWidget("Remove", tableLayout);
  removeBtn->setIcon(QIcon(":/icons/clear.svg"));
  removeBtn->setMaximumWidth(200);
  removeBtn->setEnabled(false);

  connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, topPage, [removeBtn, table]() {
    removeBtn->setEnabled(table->selectionModel()->hasSelection());
  });

  connect(model, &QStandardItemModel::itemChanged, this, [this, model](QStandardItem* item) {
    if (item->column() != 2)
      return;

    QStandardItem* nameItem = model->item(item->row(), 0);
    if (!nameItem)
      return;

    for (auto& p : mPluginSettings.plugins)
      if (p.name == nameItem->text())
      {
        p.enabled = item->checkState() == Qt::Checked;
        return;
      }
  });

  buttonRow->addStretch();
  buttonRow->addWidget(addBtn);
  buttonRow->addWidget(removeBtn);

  auto generalLayout = new maki::WidgetGroup(tr("Other settings"), topPage);
  mDefaultPlugin = new maki::SelectorWidget(tr("Default plugin"), maki::WidgetAlignment::Inline(), generalLayout);
  mDefaultPlugin->addDescription(tr("MAKI will set this plugin as the default on start"));
  mDefaultPlugin->addItem(Constants::EMPTY_COMBO, "");

  // -------------------------------------------------------------------------
  // Go through all the plugins and build the page
  for (const auto& plugin : mPluginSettings.plugins)
  {
    auto pluginId = plugin.name;
    auto settings = plugin.settings;

    // Add the available plugins to the combo box
    if (plugin.enabled)
      mDefaultPlugin->addItem(pluginId, pluginId);

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
    item->setCheckState(plugin.enabled ? Qt::Checked : Qt::Unchecked);
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    model->setItem(newRow, 2, item);

    auto [selector, page] = addPage(pluginId, plugin.icon, [] {
      // mSettingsManager->setGeneration(GenerationSettings());
    },
                                    topSelector);

    QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
    layout->setSpacing(2);

    maki::WidgetAlignment alignment = {
        .type = maki::WidgetAlignment::Type::INLINE,
        .direction = maki::WidgetAlignment::Direction::SPREAD,
        .labelWidth = 300,
    };
    for (const auto& setting : settings)
    {
      if (setting.getType() == Types::PropertyTypes::INTEGER)
      {
        int min = setting.getMetadata().contains("min") ? setting.getMetadata()["min"].toInt() : INT32_MIN;
        int max = setting.getMetadata().contains("max") ? setting.getMetadata()["max"].toInt() : INT32_MAX;
        auto* field = new maki::IntegerWidget(setting.getLabel(), setting.getDefaultValue().toString(), alignment, page, min, max);
        field->addDescription(setting.getDescription());

        connect(field, &maki::IntegerWidget::valueChanged, this, [this, pluginId, setting](const QString& value) {
          updatePluginSetting(pluginId, setting.getKey(), value);
        });

        layout->addWidget(field);
      }
      else if (setting.getType() == Types::PropertyTypes::BOOLEAN)
      {
        auto* field = new maki::BooleanWidget(setting.getLabel(), setting.getValue().toBool(), alignment, page);
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

  // Make sure the combo box is updated on start
  mDefaultPlugin->setValue(mPluginSettings.defaultPlugin);

  QVBoxLayout* layout = topPage->findChild<QVBoxLayout*>("ContentArea");
  tableLayout->addWidget(table);
  tableLayout->addLayout(buttonRow);
  generalLayout->addWidget(mDefaultPlugin);

  layout->addWidget(tableLayout);
  layout->addWidget(generalLayout);
  layout->addStretch();

  return VoidResult();
}

void SettingsDialog::saveToSettings()
{
  GeneralSettings general;
  general.language = mLanguageCombo->getValue();
  general.autosaveEnabled = mAutosaveEnabled->getValue();
  general.enableDebugLogs = mEnableDebugLogs->getValue();
  general.recentHistorySize = mRecentHistorySize->getValue();
  general.showWelcomeMessage = mShowWelcomeMessage->getValue();
  general.restoreLastSession = mRestoreLastSession->getValue();
  general.autosaveIntervalMinutes = mAutosaveMinutes->getValue();
  general.confirmOnCloseWithExecution = mConfirmOnClose->getValue();
  // The dialog should not mess with the recent files
  general.recentFiles = mSettingsManager->general().recentFiles;

  AppearanceSettings appearance;
  appearance.theme = mThemeCombo->getValue();
  appearance.themeVars = mTheme;
  appearance.uiScalePercent = mUiScale->getValue();
  appearance.showCanvasGrid = mShowGrid->getValue();
  appearance.startLogFilterExpanded = mStartLogTableFilters->getValue();
  appearance.nativeMenuBar = mNativeMenuBar->getValue();
  appearance.nodeCornerRadius = mNodeCornerRadius->getValue();
  appearance.numberOfColumns = mNumberOfColumns->getValue();

  GenerationSettings generation;
  generation.generationDir = mGenerationDirEdit->getValue();

  mPluginSettings.defaultPlugin = mDefaultPlugin->getValue();

  mSettingsManager->setGeneral(general);
  mSettingsManager->setAppearance(appearance);
  mSettingsManager->setGeneration(generation);
  mSettingsManager->setPlugins(mPluginSettings);
  mSettingsManager->applySettings();
}

void SettingsDialog::apply()
{
  saveToSettings();
}

void SettingsDialog::updatePluginSetting(const QString& pluginId, const QString& key, QVariant value)
{
  for (auto& plugin : mPluginSettings.plugins)
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
