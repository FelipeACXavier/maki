#pragma once

#include <memory>

#include "base_dialog.h"
#include "result.h"
#include "settings_manager.h"

class QSpinBox;
class QCheckBox;
class QComboBox;
class QStackedWidget;
class QListWidget;
class QLineEdit;
class QToolButton;
class QVBoxLayout;

class SettingsDialog : public BaseDialog
{
  Q_OBJECT
public:
  SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent = nullptr);

private slots:
  void apply();

private:
  // ------------------------------------------
  // Layouting
  QStackedWidget* mPages = nullptr;
  QListWidget* mPageSelector = nullptr;

  // ------------------------------------------
  // Actual settings
  std::shared_ptr<SettingsManager> mSettingsManager = nullptr;

  // General
  QCheckBox* mRestoreLastSession = nullptr;
  QCheckBox* mAutosaveEnabled = nullptr;
  QSpinBox* mAutosaveMinutes = nullptr;
  QCheckBox* mConfirmOnClose = nullptr;
  QCheckBox* mEnableDebugLogs = nullptr;

  // Appearance
  QComboBox* mThemeCombo = nullptr;
  QSpinBox* mUiScale = nullptr;
  QCheckBox* mShowGrid = nullptr;
  QSpinBox* mNodeCornerRadius = nullptr;

  // Generation
  QLineEdit* mGenerationDirEdit = nullptr;
  QToolButton* mGenerationBrowseBtn = nullptr;
  QToolButton* mGenerationResetBtn = nullptr;

  // ------------------------------------------
  // Methods
  void saveToSettings();
  void loadFromSettings();

  QWidget* addPage(const QString& pageName, const QString& iconNeame, std::function<void()> resetCallback);

  VoidResult createGeneralPage();
  VoidResult createAppearancePage();
  VoidResult createGenerationPage();
};
