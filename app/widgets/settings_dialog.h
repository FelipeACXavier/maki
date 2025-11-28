#pragma once

#include <QDialog>
#include <memory>

#include "result.h"
#include "settings_manager.h"

class QSpinBox;
class QCheckBox;
class QComboBox;
class QStackedWidget;
class QListWidget;

class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent = nullptr);

private slots:
  void applyAndClose();

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

  // ------------------------------------------
  // Methods
  void saveToSettings();
  void loadFromSettings();

  QWidget* addPage(const QString& pageName, const QString& iconNeame) const;

  VoidResult createGeneralPage();
  VoidResult createAppearancePage();
};