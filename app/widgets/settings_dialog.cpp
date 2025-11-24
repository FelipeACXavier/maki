#include "settings_dialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "logging.h"

SettingsDialog::SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent)
    : QDialog(parent)
    , mSettingsManager(manager)
{
  setWindowTitle(title);

  auto* mainLayout = new QHBoxLayout();

  // Left: navigation list
  mPageSelector = new QListWidget(this);
  mPageSelector->setViewMode(QListView::ListMode);
  mPageSelector->setMovement(QListView::Static);
  mPageSelector->setIconSize(QSize(24, 24));
  mPageSelector->setSpacing(4);

  // Right: stacked mPages
  mPages = new QStackedWidget(this);

  // Add categories
  LOG_WARN_ON_FAILURE(createGeneralPage());
  LOG_WARN_ON_FAILURE(createAppearancePage());

  mPageSelector->setCurrentRow(0);

  mainLayout->addWidget(mPageSelector, 1);
  mainLayout->addWidget(mPages, 4);

  // Select page when user clicks an item
  connect(mPageSelector, &QListWidget::currentRowChanged, mPages, &QStackedWidget::setCurrentIndex);

  // Buttons
  auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  auto outerLayout = new QVBoxLayout();
  outerLayout->addLayout(mainLayout);
  outerLayout->addWidget(buttonBox);
  setLayout(outerLayout);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::applyAndClose);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  loadFromSettings();
}

QWidget* SettingsDialog::addPage(const QString& pageName, const QString& iconNeame) const
{
  auto selector = new QListWidgetItem(QIcon(iconNeame), pageName, mPageSelector);
  mPageSelector->addItem(selector);

  QWidget* page = new QWidget;
  auto* layout = new QVBoxLayout(page);
  layout->addWidget(new QLabel(pageName));

  // Add page to the collection of pages
  mPages->addWidget(page);

  return page;
}

VoidResult SettingsDialog::createGeneralPage()
{
  auto page = addPage(tr("General"), ":/icons/general.svg");

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

  static_cast<QVBoxLayout*>(page->layout())->addWidget(mRestoreLastSession);
  static_cast<QVBoxLayout*>(page->layout())->addWidget(mAutosaveEnabled);
  static_cast<QVBoxLayout*>(page->layout())->addLayout(autosaveLayout);
  static_cast<QVBoxLayout*>(page->layout())->addWidget(mConfirmOnClose);
  static_cast<QVBoxLayout*>(page->layout())->addStretch();

  return VoidResult();
}

VoidResult SettingsDialog::createAppearancePage()
{
  auto page = addPage(tr("Appearance"), ":/icons/appearance.svg");

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

  static_cast<QVBoxLayout*>(page->layout())->addLayout(themeLayout);
  static_cast<QVBoxLayout*>(page->layout())->addLayout(scaleLayout);
  static_cast<QVBoxLayout*>(page->layout())->addWidget(mShowGrid);
  static_cast<QVBoxLayout*>(page->layout())->addLayout(radiusLayout);
  static_cast<QVBoxLayout*>(page->layout())->addStretch();

  return VoidResult();
}

void SettingsDialog::loadFromSettings()
{
  if (!mSettingsManager)
  {
    LOG_ERROR("Something went wrong with the setting manager");
    return;
  }

  const auto g = mSettingsManager->general();
  const auto a = mSettingsManager->appearance();

  mRestoreLastSession->setChecked(g.restoreLastSession);
  mAutosaveEnabled->setChecked(g.autosaveEnabled);
  mAutosaveMinutes->setValue(g.autosaveIntervalMinutes);
  mConfirmOnClose->setChecked(g.confirmOnCloseWithExecution);

  int themeIndex = mThemeCombo->findData(a.theme);
  if (themeIndex < 0)
    themeIndex = 0;  // fallback to "system"

  mThemeCombo->setCurrentIndex(themeIndex);

  mUiScale->setValue(a.uiScalePercent);
  mShowGrid->setChecked(a.showCanvasGrid);
  mNodeCornerRadius->setValue(a.nodeCornerRadius);
}

void SettingsDialog::saveToSettings()
{
  GeneralSettings g;
  g.restoreLastSession = mRestoreLastSession->isChecked();
  g.autosaveEnabled = mAutosaveEnabled->isChecked();
  g.autosaveIntervalMinutes = mAutosaveMinutes->value();
  g.confirmOnCloseWithExecution = mConfirmOnClose->isChecked();

  AppearanceSettings a;
  a.uiScalePercent = mUiScale->value();
  a.showCanvasGrid = mShowGrid->isChecked();
  a.nodeCornerRadius = mNodeCornerRadius->value();
  a.theme = mThemeCombo->currentData().toString();

  mSettingsManager->setGeneral(g);
  mSettingsManager->setAppearance(a);
}

void SettingsDialog::applyAndClose()
{
  saveToSettings();
  accept();
}
