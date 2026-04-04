#pragma once

#include <QTabWidget>

#include "badged_tab_bar.h"

class BadgedTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  explicit BadgedTabWidget(QWidget* parent = nullptr);

  BadgedTabBar* badgedTabBar() const;

private:
  BadgedTabBar* mBadgedTabBar = nullptr;
};