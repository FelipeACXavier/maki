#include "badged_tab_widget.h"

BadgedTabWidget::BadgedTabWidget(QWidget* parent)
    : QTabWidget(parent)
{
  mBadgedTabBar = new BadgedTabBar(this);
  setTabBar(mBadgedTabBar);
}

BadgedTabBar* BadgedTabWidget::badgedTabBar() const
{
  return mBadgedTabBar;
}