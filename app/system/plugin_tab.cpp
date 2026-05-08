#include "plugin_tab.h"

#include <QMenu>

#include "app_configs.h"
#include "iui.h"
#include "logging.h"
#include "plugin_view.h"

PluginTab::PluginTab(QMenu* menu, QObject* parent)
    : QObject(parent)
    , mMenu(menu)
{
}

void PluginTab::updateScene(const QString& name)
{
  if (mTabs.find(name) == mTabs.end())
    return;

  if (!mTabs[name].callback)
    return;

  LOG_WARN_ON_FAILURE(mTabs[name].callback(mTabs[name].scene));
  mTabs[name].view->fitInView(mTabs[name].scene->sceneRect(), Qt::KeepAspectRatio);
}

void PluginTab::openScene(const QString& name)
{
  LOG_DEBUG("Opening scene");

  if (mTabs.find(name) == mTabs.end())
    return;

  auto title = name + " view";
  emit openView(title, mTabs[name].view);
}

maki::Theme PluginTab::currentTheme()
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
    return qlementineToMaki(qlementineStyle->theme());

  return maki::Theme{};
}

maki::Theme PluginTab::qlementineToMaki(const oclero::qlementine::Theme& theme) const
{
  maki::Theme vars;

  // This is a bit stupid, but at least it is explicit and the compiler should be able to optimize it
  vars.backgroundColorMain1 = theme.backgroundColorMain1;
  vars.backgroundColorMain2 = theme.backgroundColorMain2;
  vars.backgroundColorMain3 = theme.backgroundColorMain3;
  vars.backgroundColorMain4 = theme.backgroundColorMain4;
  vars.backgroundColorMainTransparent = theme.backgroundColorMainTransparent;
  vars.backgroundColorWorkspace = theme.backgroundColorWorkspace;
  vars.backgroundColorTabBar = theme.backgroundColorTabBar;
  vars.neutralColor = theme.neutralColor;
  vars.neutralColorHovered = theme.neutralColorHovered;
  vars.neutralColorPressed = theme.neutralColorPressed;
  vars.neutralColorDisabled = theme.neutralColorDisabled;
  vars.neutralColorTransparent = theme.neutralColorTransparent;
  vars.focusColor = theme.focusColor;
  vars.primaryColor = theme.primaryColor;
  vars.primaryColorHovered = theme.primaryColorHovered;
  vars.primaryColorPressed = theme.primaryColorPressed;
  vars.primaryColorDisabled = theme.primaryColorDisabled;
  vars.primaryColorTransparent = theme.primaryColorTransparent;
  vars.primaryColorForeground = theme.primaryColorForeground;
  vars.primaryColorForegroundHovered = theme.primaryColorForegroundHovered;
  vars.primaryColorForegroundPressed = theme.primaryColorForegroundPressed;
  vars.primaryColorForegroundDisabled = theme.primaryColorForegroundDisabled;
  vars.primaryColorForegroundTransparent = theme.primaryColorForegroundTransparent;
  vars.primaryAlternativeColor = theme.primaryAlternativeColor;
  vars.primaryAlternativeColorHovered = theme.primaryAlternativeColorHovered;
  vars.primaryAlternativeColorPressed = theme.primaryAlternativeColorPressed;
  vars.primaryAlternativeColorDisabled = theme.primaryAlternativeColorDisabled;
  vars.primaryAlternativeColorTransparent = theme.primaryAlternativeColorTransparent;
  vars.secondaryColor = theme.secondaryColor;
  vars.secondaryColorHovered = theme.secondaryColorHovered;
  vars.secondaryColorPressed = theme.secondaryColorPressed;
  vars.secondaryColorDisabled = theme.secondaryColorDisabled;
  vars.secondaryColorTransparent = theme.secondaryColorTransparent;
  vars.secondaryColorForeground = theme.secondaryColorForeground;
  vars.secondaryColorForegroundHovered = theme.secondaryColorForegroundHovered;
  vars.secondaryColorForegroundPressed = theme.secondaryColorForegroundPressed;
  vars.secondaryColorForegroundDisabled = theme.secondaryColorForegroundDisabled;
  vars.secondaryColorForegroundTransparent = theme.secondaryColorForegroundTransparent;
  vars.secondaryAlternativeColor = theme.secondaryAlternativeColor;
  vars.secondaryAlternativeColorHovered = theme.secondaryAlternativeColorHovered;
  vars.secondaryAlternativeColorPressed = theme.secondaryAlternativeColorPressed;
  vars.secondaryAlternativeColorDisabled = theme.secondaryAlternativeColorDisabled;
  vars.secondaryAlternativeColorTransparent = theme.secondaryAlternativeColorTransparent;
  vars.statusColorSuccess = theme.statusColorSuccess;
  vars.statusColorSuccessHovered = theme.statusColorSuccessHovered;
  vars.statusColorSuccessPressed = theme.statusColorSuccessPressed;
  vars.statusColorSuccessDisabled = theme.statusColorSuccessDisabled;
  vars.statusColorInfo = theme.statusColorInfo;
  vars.statusColorInfoHovered = theme.statusColorInfoHovered;
  vars.statusColorInfoPressed = theme.statusColorInfoPressed;
  vars.statusColorInfoDisabled = theme.statusColorInfoDisabled;
  vars.statusColorWarning = theme.statusColorWarning;
  vars.statusColorWarningHovered = theme.statusColorWarningHovered;
  vars.statusColorWarningPressed = theme.statusColorWarningPressed;
  vars.statusColorWarningDisabled = theme.statusColorWarningDisabled;
  vars.statusColorError = theme.statusColorError;
  vars.statusColorErrorHovered = theme.statusColorErrorHovered;
  vars.statusColorErrorPressed = theme.statusColorErrorPressed;
  vars.statusColorErrorDisabled = theme.statusColorErrorDisabled;
  vars.statusColorForeground = theme.statusColorForeground;
  vars.statusColorForegroundHovered = theme.statusColorForegroundHovered;
  vars.statusColorForegroundPressed = theme.statusColorForegroundPressed;
  vars.statusColorForegroundDisabled = theme.statusColorForegroundDisabled;
  vars.shadowColor1 = theme.shadowColor1;
  vars.shadowColor2 = theme.shadowColor2;
  vars.shadowColor3 = theme.shadowColor3;
  vars.shadowColorTransparent = theme.shadowColorTransparent;
  vars.borderColor = theme.borderColor;
  vars.borderColorHovered = theme.borderColorHovered;
  vars.borderColorPressed = theme.borderColorPressed;
  vars.borderColorDisabled = theme.borderColorDisabled;
  vars.borderColorTransparent = theme.borderColorTransparent;
  vars.semiTransparentColor1 = theme.semiTransparentColor1;
  vars.semiTransparentColor2 = theme.semiTransparentColor2;
  vars.semiTransparentColor3 = theme.semiTransparentColor3;
  vars.semiTransparentColor4 = theme.semiTransparentColor4;
  vars.semiTransparentColorTransparent = theme.semiTransparentColorTransparent;
  vars.useSystemFonts = theme.useSystemFonts;
  vars.fontSize = theme.fontSize;
  vars.fontSizeMonospace = theme.fontSizeMonospace;
  vars.fontSizeH1 = theme.fontSizeH1;
  vars.fontSizeH2 = theme.fontSizeH2;
  vars.fontSizeH3 = theme.fontSizeH3;
  vars.fontSizeH4 = theme.fontSizeH4;
  vars.fontSizeH5 = theme.fontSizeH5;
  vars.fontSizeS1 = theme.fontSizeS1;
  vars.animationDuration = theme.animationDuration;
  vars.focusAnimationDuration = theme.focusAnimationDuration;
  vars.sliderAnimationDuration = theme.sliderAnimationDuration;
  vars.borderRadius = theme.borderRadius;
  vars.checkBoxBorderRadius = theme.checkBoxBorderRadius;
  vars.menuItemBorderRadius = theme.menuItemBorderRadius;
  vars.menuBarItemBorderRadius = theme.menuBarItemBorderRadius;
  vars.borderWidth = theme.borderWidth;
  vars.controlHeightLarge = theme.controlHeightLarge;
  vars.controlHeightMedium = theme.controlHeightMedium;
  vars.controlHeightSmall = theme.controlHeightSmall;
  vars.controlDefaultWidth = theme.controlDefaultWidth;
  vars.dialMarkLength = theme.dialMarkLength;
  vars.dialMarkThickness = theme.dialMarkThickness;
  vars.dialTickLength = theme.dialTickLength;
  vars.dialTickSpacing = theme.dialTickSpacing;
  vars.dialGrooveThickness = theme.dialGrooveThickness;
  vars.focusBorderWidth = theme.focusBorderWidth;
  vars.iconSize = theme.iconSize;
  vars.iconSizeMedium = theme.iconSizeMedium;
  vars.iconSizeLarge = theme.iconSizeLarge;
  vars.iconSizeExtraSmall = theme.iconSizeExtraSmall;
  vars.sliderTickSize = theme.sliderTickSize;
  vars.sliderTickSpacing = theme.sliderTickSpacing;
  vars.sliderTickThickness = theme.sliderTickThickness;
  vars.sliderGrooveHeight = theme.sliderGrooveHeight;
  vars.progressBarGrooveHeight = theme.progressBarGrooveHeight;
  vars.spacing = theme.spacing;
  vars.scrollBarThicknessFull = theme.scrollBarThicknessFull;
  vars.scrollBarThicknessSmall = theme.scrollBarThicknessSmall;
  vars.scrollBarMargin = theme.scrollBarMargin;
  vars.tabBarPaddingTop = theme.tabBarPaddingTop;
  vars.tabBarTabMaxWidth = theme.tabBarTabMaxWidth;
  vars.tabBarTabMinWidth = theme.tabBarTabMinWidth;
  // Fonts
  vars.fontRegular = theme.fontRegular;
  vars.fontBold = theme.fontBold;
  vars.fontH1 = theme.fontH1;
  vars.fontH2 = theme.fontH2;
  vars.fontH3 = theme.fontH3;
  vars.fontH4 = theme.fontH4;
  vars.fontH5 = theme.fontH5;
  vars.fontCaption = theme.fontCaption;
  vars.fontMonospace = theme.fontMonospace;
  vars.palette = theme.palette;

  return vars;
}

void PluginTab::onThemeChanged()
{
  for (auto it = mTabs.constBegin(); it != mTabs.constEnd(); ++it)
    updateScene(it.key());
}

void PluginTab::registerPlugin(const QString& name, std::function<VoidResult(QGraphicsScene* scene)> callback)
{
  if (mTabs.find(name) != mTabs.end())
  {
    LOG_WARNING("Plugin already registered, ignoring registration");
    return;
  }

  PluginData pd;
  pd.view = new PluginView();
  pd.scene = new QGraphicsScene(pd.view);
  pd.view->setScene(pd.scene);
  pd.callback = callback;

  pd.action = new QAction(tr("Open") + " " + name, mMenu);
  connect(pd.action, &QAction::triggered, [this, name] { openScene(name); });

  mTabs[name] = pd;

  mMenu->addAction(pd.action);
}

void PluginTab::deregisterPlugin(const QString& name)
{
  // Plugin is not registered
  auto tab = mTabs.find(name);
  if (tab == mTabs.end())
    return;

  // Make sure the tab is not available in the menu
  mMenu->removeAction(tab->action);

  // Close the tab in case it is open
  emit closeView(tab->view);

  // Remove the plugin from the list
  delete tab->view;
  mTabs.remove(name);
}

void PluginTab::addTab(const QString& name, QWidget* tabWidget)
{
  auto tab = mTabs.find(name);
  if (tab == mTabs.end())
    return;

  tab->tab = tabWidget;
  emit addBottomTab(name, tabWidget);
}

void PluginTab::closeTab(const QString& name, QWidget* tabWidget)
{
  auto tab = mTabs.find(name);
  if (tab == mTabs.end())
    return;

  emit removeBottomTab(tabWidget);
}
