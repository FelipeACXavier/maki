#pragma once

#include <memory>
#include <oclero/qlementine.hpp>

#include "base_dialog.h"
#include "language_manager.h"
#include "result.h"
#include "settings_manager.h"
#include "theme_editor.h"
#include "widgets/theme_editor.h"
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
  /**
   * Constructs a new instance of SettingsDialog.
   * @param title The title of the dialog.
   * @param manager A shared pointer to the SettingsManager.
   * @param languageManager A shared pointer to the LanguageManager.
   * @param parent The parent widget.
   */
  SettingsDialog(const QString& title, std::shared_ptr<SettingsManager> manager, std::shared_ptr<LanguageManager> languageManager,
                 QWidget* parent = nullptr);

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
  QStackedWidget* mPages = nullptr;      /// Pointer to the stacked widget for managing pages.
  QTreeWidget* mPageSelector = nullptr;  /// Pointer to the tree widget for selecting pages.

  // ------------------------------------------
  // Actual settings
  std::shared_ptr<SettingsManager> mSettingsManager = nullptr;  /// Shared pointer to the SettingsManager.
  std::shared_ptr<LanguageManager> mLanguageManager = nullptr;  /// Shared pointer to the LanguageManager.

  // General
  maki::BooleanWidget* mRestoreLastSession = nullptr;  /// Widget for enabling/disabling restoring last session.
  maki::BooleanWidget* mAutosaveEnabled = nullptr;     /// Widget for enabling/disabling autosave.
  maki::SpinWidget* mAutosaveMinutes = nullptr;        /// Widget for setting autosave interval in minutes.
  maki::BooleanWidget* mConfirmOnClose = nullptr;      /// Widget for enabling/disabling confirmation on close.
  maki::BooleanWidget* mEnableDebugLogs = nullptr;     /// Widget for enabling debug logs.
  maki::IntegerWidget* mRecentHistorySize = nullptr;   /// Widget for setting recent history size.
  maki::BooleanWidget* mShowWelcomeMessage = nullptr;  /// Widget for enabling/disabling welcome message.
  maki::SelectorWidget* mLanguageCombo = nullptr;      /// Widget for selecting language.

  // Appearance
  maki::SelectorWidget* mThemeCombo = nullptr;           /// Widget for selecting theme.
  maki::SpinWidget* mUiScale = nullptr;                  /// Widget for setting UI scale.
  maki::BooleanWidget* mNativeMenuBar = nullptr;         /// Widget for enabling/disabling native menu bar.
  maki::BooleanWidget* mShowGrid = nullptr;              /// Widget for enabling/disabling grid display.
  maki::BooleanWidget* mStartLogTableFilters = nullptr;  /// Widget for enabling/disabling log table filters on start up.
  maki::SpinWidget* mNodeCornerRadius = nullptr;         /// Widget for setting node corner radius.
  maki::SpinWidget* mNumberOfColumns = nullptr;          /// Widget for setting the palette number of columns
  maki::GridGroup* mColorGrid = nullptr;                 /// Group widget for color settings.
  maki::StringWidget* mUserThemeName = nullptr;          /// Widget for entering user theme name.
  maki::SelectorWidget* mTransitionShape = nullptr;      /// Widget for selecting the transition shape.
  oclero::qlementine::ThemeEditorWidget* mThemeEditor = nullptr;

  // Generation
  maki::StringWidget* mGenerationDirEdit = nullptr;    /// Widget for editing generation directory.
  maki::ButtonWidget* mGenerationBrowseBtn = nullptr;  /// Button widget for browsing generation directory.

  // Plugins
  PluginSettings mPluginSettings;                  /// Vector of plugin settings.
  maki::SelectorWidget* mDefaultPlugin = nullptr;  /// Widget for editing the default plugin
  maki::BooleanWidget* mMergeLogs = nullptr;       /// Widget for editing the default plugin

  oclero::qlementine::Theme mTheme;  /// Current theme.

  // ------------------------------------------
  // Methods
  void saveToSettings();  /// Saves the current settings to the manager.

  /**
   * Constructs a standard settings page and assigns the selector accordingly.
   * @param pageName This will be the page title and the text in the selector.
   * @param iconName The icon of the page. It will be reflected in the selector.
   * @param resetCallback The callback for when the user clicks the reset button.
   * @param parent The parent widget.
   * @return A struct with pointers to the created page and the respective selector. It can be assigned as auto[page, selector] = addPage(...);
   */
  SelectorPage addPage(const QString& pageName, const QString& iconName, std::function<void()> resetCallback, QTreeWidgetItem* parent = nullptr);

  VoidResult createGeneralPage();     /// Creates the general settings page.
  VoidResult createAppearancePage();  /// Creates the appearance settings page.
  VoidResult createGenerationPage();  /// Creates the generation settings page.
  VoidResult createPluginPages();     /// Creates the plugin settings pages.

  void updatePluginSetting(const QString& pluginId, const QString& key, QVariant value);  /// Updates a plugin setting.
};
