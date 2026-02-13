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
#include "widget_factory.h"

SettingsDialog::SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent)
    : BaseDialog(title, parent)
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

  // mPageSelector->setCurrentRow(0);

  mainLayout->addWidget(mPageSelector, 1);
  mainLayout->addWidget(mPages, 4);

  // Select page when user clicks an item
  // connect(mPageSelector, &QListWidget::currentRowChanged, mPages, &QStackedWidget::setCurrentIndex);
  connect(mPageSelector, &QTreeWidget::currentItemChanged, this, [this](QTreeWidgetItem* current, QTreeWidgetItem* /* previous */) {
    if (!current)
      return;

    // Non-leaf category items may not map to a page.
    const auto pageIndex = current->data(0, Qt::UserRole).toInt();
    if (pageIndex >= 0)
      mPages->setCurrentIndex(pageIndex);
  });

  // Buttons
  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  auto* okBtn = buttonBox->button(QDialogButtonBox::Ok);
  auto* cancelBtn = buttonBox->button(QDialogButtonBox::Cancel);

  if (okBtn)
  {
    okBtn->setObjectName("TextAndIcon");
    okBtn->setText(" Apply");
    addIcon(okBtn, ":/icons/accept.svg");
  }

  if (cancelBtn)
  {
    cancelBtn->setObjectName("TextAndIcon");
    cancelBtn->setText(" Close");
    addIcon(cancelBtn, ":/icons/reject.svg");
  }

  layout()->addLayout(mainLayout);
  layout()->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::apply);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  loadFromSettings();

  updateIconTheme(mIcons);
}

SettingsDialog::SelectorPage SettingsDialog::addPage(const QString& pageName, const QString& iconName, std::function<void()> resetCallback, QTreeWidgetItem* parent)
{
  QWidget* page = new QWidget;
  page->setObjectName("SettingsPage");

  QVBoxLayout* layout = new QVBoxLayout(page);
  layout->setContentsMargins(6, 6, 6, 6);

  auto* headerRow = new QHBoxLayout();
  QLabel* titleIcon = new QLabel();
  titleIcon->setPixmap(applyColorToIcon(iconName, Config::FOREGROUND).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));

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

  // Update the selector with new page
  auto* selector = parent == nullptr ? new QTreeWidgetItem(mPageSelector) : new QTreeWidgetItem(parent);
  selector->setText(0, pageName);
  selector->setIcon(0, addIconWithColor(iconName, Config::FOREGROUND));
  selector->setData(0, Qt::UserRole, index);

  return SelectorPage{selector, page};
}

VoidResult SettingsDialog::createGeneralPage()
{
  auto [selector, page] = addPage(tr("General"), ":/icons/general.svg", [this] {
    mSettingsManager->setGeneral(GeneralSettings());
    loadFromSettings();
  });

  mRestoreLastSession = new QCheckBox(tr("Restore last session on startup"), page);
  mAutosaveEnabled = new QCheckBox(tr("Enable autosave"), page);

  mAutosaveMinutes = new QSpinBox(page);
  mAutosaveMinutes->setRange(1, 120);
  mAutosaveMinutes->setSuffix(tr(" min"));

  auto autosaveLayout = new QHBoxLayout;
  autosaveLayout->addWidget(new QLabel(tr("Autosave interval:"), page));
  autosaveLayout->addWidget(mAutosaveMinutes);
  autosaveLayout->addStretch();

  mConfirmOnClose = new QCheckBox(tr("Confirm before closing editor with running execution"), page);
  mEnableDebugLogs = new QCheckBox(tr("Enable debug logs"), page);

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addWidget(mRestoreLastSession);
  layout->addWidget(mAutosaveEnabled);
  layout->addLayout(autosaveLayout);
  layout->addWidget(mConfirmOnClose);
  layout->addWidget(mEnableDebugLogs);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createAppearancePage()
{
  auto [selector, page] = addPage(tr("Appearance"), ":/icons/appearance.svg", [this] {
    mSettingsManager->setAppearance(AppearanceSettings());
    loadFromSettings();
  });

  mThemeCombo = new QComboBox(page);

  // First entry: system theme (no QSS)
  mThemeCombo->addItem(tr("System theme"), "system");

  // Then all discovered themes
  for (const Config::ThemeInfo& info : mSettingsManager->availableThemes())
  {
    QString label = info.displayName;
    if (info.isUser)
      label += tr(" (user)");

    mThemeCombo->addItem(label, info.id);
  }

  auto themeLayout = new QHBoxLayout();
  themeLayout->addWidget(new QLabel(tr("Theme:"), page));
  themeLayout->addWidget(mThemeCombo);
  themeLayout->addStretch();

  mNativeMenuBar = new QCheckBox(tr("Use native menubar"), page);

  mUiScale = new QSpinBox(page);
  mUiScale->setRange(80, 200);
  mUiScale->setSuffix(tr(" %"));

  auto scaleLayout = new QHBoxLayout();
  scaleLayout->addWidget(new QLabel(tr("UI scale:"), page));
  scaleLayout->addWidget(mUiScale);
  scaleLayout->addStretch();

  mShowGrid = new QCheckBox(tr("Show canvas grid"), page);

  mNodeCornerRadius = new QSpinBox(page);
  mNodeCornerRadius->setRange(0, 30);

  auto radiusLayout = new QHBoxLayout();
  radiusLayout->addWidget(new QLabel(tr("Node corner radius:"), page));
  radiusLayout->addWidget(mNodeCornerRadius);
  radiusLayout->addStretch();

  QVBoxLayout* layout = page->findChild<QVBoxLayout*>("ContentArea");
  layout->addLayout(themeLayout);
  layout->addWidget(mNativeMenuBar);
  layout->addLayout(scaleLayout);
  layout->addWidget(mShowGrid);
  layout->addLayout(radiusLayout);
  layout->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createGenerationPage()
{
  auto [selector, page] = addPage(tr("Generation"), ":/icons/generator.svg", [this] {
    mSettingsManager->setGeneration(GenerationSettings());
    loadFromSettings();
  });

  // Row: text field + "Browse…" button
  auto* label = new QLabel(tr("Default generation output folder"), page);

  auto* pathRow = new QWidget(page);
  auto* colLayout = new QVBoxLayout(pathRow);
  colLayout->setSpacing(0);

  auto* rowLayout = new QHBoxLayout();
  rowLayout->setContentsMargins(0, 0, 0, 0);

  mGenerationDirEdit = new QLineEdit(pathRow);
  mGenerationDirEdit->setObjectName("generationPathEdit");

  // This is the "faded default value"
  mGenerationDirEdit->setPlaceholderText(mSettingsManager->generation().generationDir);

  auto* hint = new QLabel(tr("\"/generated/<plugin name>\" will be appended to this path"), page);
  hint->setFont(Fonts::Hint);
  hint->setObjectName("HintLabel");

  mGenerationBrowseBtn = new QToolButton(pathRow);
  mGenerationBrowseBtn->setText("…");
  mGenerationBrowseBtn->setToolTip(tr("Choose folder"));

  rowLayout->addWidget(mGenerationDirEdit, /*stretch=*/1);
  rowLayout->addWidget(mGenerationBrowseBtn);

  colLayout->addWidget(label);
  colLayout->addLayout(rowLayout);
  colLayout->addWidget(hint);

  // hook up buttons
  connect(mGenerationBrowseBtn, &QToolButton::clicked, this, [this] {
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Select default generation folder"));
    if (!dir.isEmpty())
      mGenerationDirEdit->setText(dir);
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
    // loadFromSettings();
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

  auto* addBtn = new QPushButton("Add", topPage);
  addBtn->setFixedWidth(200);
  addIcon(addBtn, ":/icons/plus.svg");

  auto* removeBtn = new QPushButton("Remove", topPage);
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

    auto [selector, page] = addPage(pluginId, ":/icons/plugin.svg", [this] {
      // mSettingsManager->setGeneration(GenerationSettings());
      // loadFromSettings();
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

void SettingsDialog::loadFromSettings()
{
  if (!mSettingsManager)
  {
    LOG_ERROR("Something went wrong with the setting manager");
    return;
  }

  const auto general = mSettingsManager->general();
  const auto appearance = mSettingsManager->appearance();
  const auto generation = mSettingsManager->generation();

  // -----------------------------------------------------------------
  // Load General settings
  mRestoreLastSession->setChecked(general.restoreLastSession);
  mAutosaveEnabled->setChecked(general.autosaveEnabled);
  mAutosaveMinutes->setValue(general.autosaveIntervalMinutes);
  mConfirmOnClose->setChecked(general.confirmOnCloseWithExecution);
  mEnableDebugLogs->setChecked(general.enableDebugLogs);

  // -----------------------------------------------------------------
  // Load Appearance settings
  int themeIndex = mThemeCombo->findData(appearance.theme);
  if (themeIndex < 0)
    themeIndex = 0;  // fallback to "system"

  mThemeCombo->setCurrentIndex(themeIndex);

  mUiScale->setValue(appearance.uiScalePercent);
  mShowGrid->setChecked(appearance.showCanvasGrid);
  mNodeCornerRadius->setValue(appearance.nodeCornerRadius);
  mNativeMenuBar->setChecked(appearance.nativeMenuBar);

  // -----------------------------------------------------------------
  // Load Generation settings
  mGenerationDirEdit->setText(generation.generationDir);
}

void SettingsDialog::saveToSettings()
{
  GeneralSettings general;
  general.restoreLastSession = mRestoreLastSession->isChecked();
  general.autosaveEnabled = mAutosaveEnabled->isChecked();
  general.autosaveIntervalMinutes = mAutosaveMinutes->value();
  general.confirmOnCloseWithExecution = mConfirmOnClose->isChecked();
  general.enableDebugLogs = mEnableDebugLogs->isChecked();

  AppearanceSettings appearance;
  appearance.uiScalePercent = mUiScale->value();
  appearance.showCanvasGrid = mShowGrid->isChecked();
  appearance.nodeCornerRadius = mNodeCornerRadius->value();
  appearance.theme = mThemeCombo->currentData().toString();
  appearance.nativeMenuBar = mNativeMenuBar->isChecked();

  GenerationSettings generation;
  generation.generationDir = mGenerationDirEdit->text();

  mSettingsManager->setGeneral(general);
  mSettingsManager->setAppearance(appearance);
  mSettingsManager->setGeneration(generation);
  mSettingsManager->setPlugins(mPluginSettings);
}

void SettingsDialog::apply()
{
  saveToSettings();
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