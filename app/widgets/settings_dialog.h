#pragma once

#include <memory>

#include "base_dialog.h"
#include "result.h"
#include "settings_manager.h"
#include "widgets/widget_factory.h"

class QSpinBox;
class QCheckBox;
class QComboBox;
class QStackedWidget;
class QTreeWidget;
class QLineEdit;
class QToolButton;
class QVBoxLayout;

namespace maki
{
class IntegerWidget;
class BooleanWidget;
class SpinWidget;
class SelectorWidget;
class StringWidget;
class ButtonWidget;
class GridGroup;
}  // namespace maki

/**
 * Provides a dialog that allows the user to set and view the application settings.
 */
class SettingsDialog : public BaseDialog
{
  Q_OBJECT
public:
  SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, QWidget* parent = nullptr);

private slots:
  /**
   * Called when the user confirms the new settings.
   */
  void apply();

private:
  struct SelectorPage
  {
    QTreeWidgetItem* selector;
    QWidget* page;
  };

  // ------------------------------------------
  // Layouting
  QStackedWidget* mPages = nullptr;
  QTreeWidget* mPageSelector = nullptr;

  // ------------------------------------------
  // Actual settings
  std::shared_ptr<SettingsManager> mSettingsManager = nullptr;

  // General
  maki::BooleanWidget* mRestoreLastSession = nullptr;
  maki::BooleanWidget* mAutosaveEnabled = nullptr;
  maki::SpinWidget* mAutosaveMinutes = nullptr;
  maki::BooleanWidget* mConfirmOnClose = nullptr;
  maki::BooleanWidget* mEnableDebugLogs = nullptr;
  maki::IntegerWidget* mRecentHistorySize = nullptr;
  maki::BooleanWidget* mShowWelcomeMessage = nullptr;

  // Appearance
  maki::SelectorWidget* mThemeCombo = nullptr;
  maki::SpinWidget* mUiScale = nullptr;
  maki::BooleanWidget* mNativeMenuBar = nullptr;
  maki::BooleanWidget* mShowGrid = nullptr;
  maki::SpinWidget* mNodeCornerRadius = nullptr;
  maki::GridGroup* mColorGrid = nullptr;
  maki::StringWidget* mUserThemeName = nullptr;

  // Generation
  maki::StringWidget* mGenerationDirEdit = nullptr;
  maki::ButtonWidget* mGenerationBrowseBtn = nullptr;

  // Plugins
  QVector<PluginInfo> mPluginSettings;

  // User theme
  maki::ThemeVars mTheme;

  // ------------------------------------------
  // Methods
  void saveToSettings();

  /**
   * Construct the standard settings page and assign the selector accordingly.
   * @param pageName This will be the page title and the text in the selector.
   * @param iconName The icon of the page. It will be reflected in the selector.
   * @param resetCallback The callback for when the user clicks the reset button.
   * @param parent The parent widget.
   * @return A struct with pointers to the create page and the respective selector. It can be assigned as auto[page, selector] = addPage(...);
   */
  SelectorPage addPage(const QString& pageName, const QString& iconName, std::function<void()> resetCallback, QTreeWidgetItem* parent = nullptr);

  VoidResult createGeneralPage();
  VoidResult createAppearancePage();
  VoidResult createGenerationPage();
  VoidResult createPluginPages();

  void updatePluginSetting(const QString& pluginId, const QString& key, QVariant value);
  void updateColorGrid();
  QString toColorLabel(QString label) const;
  maki::ColorWidget* widgetByLabel(const QString& label, const QGridLayout* grid) const;
};
