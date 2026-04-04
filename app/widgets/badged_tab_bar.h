#pragma once

#include <QColor>
#include <QHash>
#include <QString>
#include <QTabBar>

#include "logging.h"

struct TabBadge
{
  bool visible = false;
  QString text;
  int count = 0;
  QColor background = QColor("#d14343");
  QColor foreground = Qt::white;
  logging::LogLevel level = logging::LogLevel::Info;
};

class BadgedTabBar : public QTabBar
{
  Q_OBJECT

public:
  explicit BadgedTabBar(QWidget* parent = nullptr);

  void tabInserted(int index) override;

  void clearTabBadge(int index);
  void setTabBadgeCount(int index);
  void setTabErrorBadgeCount(int index);
  void setTabDot(int index);
  void setTabErrorDot(int index);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  void paintBadge(QPainter& painter, int index, const QRect& tabRect, const TabBadge& badge);

  QHash<int, TabBadge> mBadges;
};