#pragma once

#include <QTabWidget>

#include "badged_tab_bar.h"

/**
 * @brief A custom tab widget with a badge bar.
 */
class BadgedTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new BadgedTabWidget.
   *
   * @param parent The parent widget, if any.
   */
  explicit BadgedTabWidget(QWidget* parent = nullptr);

  /**
   * @brief Retrieves the badge tab bar associated with this tab widget.
   *
   * @return A pointer to the BadgedTabBar instance.
   */
  BadgedTabBar* badgedTabBar() const;

private:
  BadgedTabBar* mBadgedTabBar = nullptr;  /// Pointer to the badge tab bar.
};
