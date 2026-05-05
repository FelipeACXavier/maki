#pragma once

#include <QColor>
#include <QHash>
#include <QString>
#include <QTabBar>

#include "logging.h"

/**
 * @brief Represents a badge that can be displayed on a tab.
 */
struct TabBadge {
  bool visible = false;  /// Indicates if the badge is visible.
  QString text;          /// Text to display on the badge.
  int count = 0;         /// Count value for numeric badges.
  QColor background = QColor("#d14343");  /// Background color of the badge.
  QColor foreground = Qt::white;  /// Foreground color of the badge.
  logging::LogLevel level = logging::LogLevel::Info;  /// Logging level associated with the badge.
};

/**
 * @brief A QTabBar subclass that supports displaying badges on tabs.
 */
class BadgedTabBar : public QTabBar {
  Q_OBJECT

public:
  /**
   * @brief Constructs a new BadgedTabBar instance.
   *
   * @param parent The parent widget of this tab bar.
   */
  explicit BadgedTabBar(QWidget* parent = nullptr);

  /**
   * @brief Handles the insertion of a new tab.
   *
   * @param index Index at which the tab is inserted.
   */
  void tabInserted(int index) override;

  /**
   * @brief Clears the badge for a specific tab.
   *
   * @param index Index of the tab to clear the badge from.
   */
  void clearTabBadge(int index);

  /**
   * @brief Sets the count value for a numeric badge on a specific tab.
   *
   * @param index Index of the tab to set the badge count for.
   */
  void setTabBadgeCount(int index);

  /**
   * @brief Sets an error badge with a count value on a specific tab.
   *
   * @param index Index of the tab to set the error badge count for.
   */
  void setTabErrorBadgeCount(int index);

  /**
   * @brief Sets a dot badge on a specific tab.
   *
   * @param index Index of the tab to set the dot badge for.
   */
  void setTabDot(int index);

  /**
   * @brief Sets an error dot badge on a specific tab.
   *
   * @param index Index of the tab to set the error dot badge for.
   */
  void setTabErrorDot(int index);

protected:
  /**
   * @brief Handles the paint event for the tab bar.
   *
   * @param event The paint event.
   */
  void paintEvent(QPaintEvent* event) override;

private:
  /**
   * @brief Paints a badge on a specific tab.
   *
   * @param painter The QPainter used to draw the badge.
   * @param index Index of the tab to paint the badge on.
   * @param tabRect Rectangle representing the tab's area.
   * @param badge The TabBadge to be painted.
   */
  void paintBadge(QPainter& painter, int index, const QRect& tabRect, const TabBadge& badge);

  QHash<int, TabBadge> mBadges;  /// Hash map storing badges for each tab.
};
