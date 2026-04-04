#include "badged_tab_bar.h"

#include <QPainter>
#include <QStyleOptionTab>
#include <QStylePainter>

#include "theme.h"

BadgedTabBar::BadgedTabBar(QWidget* parent)
    : QTabBar(parent)
{
  setDrawBase(true);
}

void BadgedTabBar::tabInserted(int index)
{
  QTabBar::tabInserted(index);
  mBadges[index] = TabBadge{};
}

void BadgedTabBar::clearTabBadge(int index)
{
  mBadges[index] = TabBadge{};
  update();
}

void BadgedTabBar::setTabBadgeCount(int index)
{
  if (index < 0 || index >= this->count())
    return;

  const auto infoColor = Config::getValueFromTheme("@info_color");

  TabBadge badge = mBadges[index];

  badge.visible = true;
  badge.foreground = Config::FOREGROUND;
  badge.text = QString::number(++badge.count);
  // Do not override error badges
  if (badge.level >= logging::LogLevel::Warning)
  {
    badge.background = infoColor.isValid() ? QColor(infoColor.toString()) : Config::HIGHLIGHT;
    badge.level = logging::LogLevel::Warning;
  }

  mBadges[index] = badge;
  update();
}

void BadgedTabBar::setTabErrorBadgeCount(int index)
{
  if (index < 0 || index >= this->count())
    return;

  const auto errorColor = Config::getValueFromTheme("@error_color");
  if (!errorColor.isValid())
    return;

  TabBadge badge = mBadges[index];

  badge.visible = true;
  badge.foreground = Config::FOREGROUND;
  badge.text = QString::number(++badge.count);
  badge.background = errorColor.isValid() ? QColor(errorColor.toString()) : Config::HIGHLIGHT;
  badge.level = logging::LogLevel::Error;

  mBadges[index] = badge;
  update();
}

void BadgedTabBar::setTabDot(int index)
{
  if (index < 0 || index >= count())
    return;

  const auto infoColor = Config::getValueFromTheme("@info_color");

  TabBadge badge = mBadges[index];
  badge.visible = true;
  badge.text.clear();
  badge.foreground = Config::FOREGROUND;
  if (badge.level >= logging::LogLevel::Warning)
  {
    badge.background = infoColor.isValid() ? QColor(infoColor.toString()) : Config::HIGHLIGHT;
    badge.level = logging::LogLevel::Warning;
  }

  mBadges[index] = badge;
  update();
}

void BadgedTabBar::setTabErrorDot(int index)
{
  if (index < 0 || index >= count())
    return;

  const auto errorColor = Config::getValueFromTheme("@error_color");

  TabBadge badge = mBadges[index];
  badge.visible = true;
  badge.text.clear();
  badge.background = QColor(errorColor.isValid() ? errorColor.toString() : "#DC6C6D");
  badge.foreground = Config::FOREGROUND;
  badge.level = logging::LogLevel::Error;

  mBadges[index] = badge;
  update();
}

void BadgedTabBar::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);

  QStylePainter painter(this);
  for (int i = 0; i < count(); ++i)
  {
    QStyleOptionTab option;
    initStyleOption(&option, i);
    painter.drawControl(QStyle::CE_TabBarTabShape, option);
    painter.drawControl(QStyle::CE_TabBarTabLabel, option);

    const TabBadge& badge = mBadges[i];
    if (badge.visible)
      paintBadge(painter, i, tabRect(i), badge);
  }
}

void BadgedTabBar::paintBadge(QPainter& painter, int index, const QRect& tabRect, const TabBadge& badge)
{
  Q_UNUSED(index);

  painter.save();
  painter.setRenderHint(QPainter::Antialiasing, true);

  const bool dotOnly = badge.text.trimmed().isEmpty();

  if (dotOnly)
  {
    const int diameter = 10;
    const int x = tabRect.right() - diameter;
    const int y = tabRect.top();

    painter.setPen(Qt::NoPen);
    painter.setBrush(badge.background);
    painter.drawEllipse(QRect(x, y, diameter, diameter));
  }
  else
  {
    QFont f = font();
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() - 1);
    painter.setFont(f);

    QFontMetrics fm(f);
    const int textWidth = fm.horizontalAdvance(badge.text);
    const int h = 16;
    const int w = qMax(16, textWidth + 10);

    const int x = tabRect.right() - w - 8;
    const int y = tabRect.top() + 5;

    QRect badgeRect(x, y, w, h);

    painter.setPen(Qt::NoPen);
    painter.setBrush(badge.background);
    painter.drawRoundedRect(badgeRect, h / 2.0, h / 2.0);

    painter.setPen(badge.foreground);
    painter.drawText(badgeRect, Qt::AlignCenter, badge.text);
  }

  painter.restore();
}