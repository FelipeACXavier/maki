#include "dropdown_button.h"

#include <QActionGroup>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionToolButton>

DropDownButton::DropDownButton(QWidget* parent)
    : QToolButton(parent)
{
  auto menu = new QMenu(this);
  menu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  menu->setMinimumWidth(mWidth);

  setMenu(menu);
  setEnabled(true);
  setPopupMode(QToolButton::MenuButtonPopup);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

  setCursor(Qt::PointingHandCursor);
}

QAction* DropDownButton::addAction(const QString& text)
{
  return menu()->addAction(text);
}

QAction* DropDownButton::addAction(const QIcon& icon, const QString& text)
{
  return menu()->addAction(icon, text);
}

void DropDownButton::reset()
{
  menu()->clear();
}

void DropDownButton::setSize(int width, int height)
{
  mWidth = width;
  mHeight = height;
}

int DropDownButton::leadingContentWidth(const oclero::qlementine::Theme&) const
{
  return 0;
}

void DropDownButton::paintLeadingContent(QPainter&, const QRect&, const oclero::qlementine::Theme&)
{
}

QString DropDownButton::displayedText() const
{
  return text();
}

void DropDownButton::paintEvent(QPaintEvent* event)
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
  {
    QToolButton::paintEvent(event);
    return;
  }

  const auto theme = qlementineStyle->theme();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const QRect r = rect().adjusted(theme.borderWidth, theme.borderWidth, -theme.borderWidth, -theme.borderWidth);

  QColor background = theme.backgroundColorMain2;
  if (underMouse() || isDown())
    background = theme.backgroundColorMain3;
  if (!isEnabled())
    background = theme.backgroundColorMain1;

  // Draw the main rectangle, which contains everything else
  painter.setPen(theme.borderColor);
  painter.setBrush(background);
  painter.drawRoundedRect(r, theme.borderRadius, theme.borderRadius);

  constexpr int arrowAreaWidth = 24;
  const int dividerX = r.right() - arrowAreaWidth;
  const int leadingWidth = leadingContentWidth(theme);
  const QRect leadingRect(r.left() + theme.spacing, r.top(), leadingWidth, r.height());

  paintLeadingContent(painter, leadingRect, theme);

  // Draw the label
  const int textLeft = r.left() + theme.spacing + leadingWidth + (leadingWidth > 0 ? theme.spacing : 0);
  const QRect textRect(textLeft, r.top(), dividerX - textLeft - theme.spacing, r.height());

  painter.setPen(isEnabled() ? theme.secondaryColor : theme.secondaryColorDisabled);
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fontMetrics().elidedText(displayedText(), Qt::ElideRight, textRect.width()));

  // Draw the dividing line
  painter.setPen(theme.secondaryColorDisabled);
  painter.drawLine(dividerX, r.top() + theme.spacing, dividerX, r.bottom() - theme.spacing);

  // Draw the dropdown menu arrow
  const QPointF arrowCenter(dividerX + arrowAreaWidth / 2.0, r.center().y() + 1.0);
  QPolygonF arrow;
  arrow << QPointF(arrowCenter.x() - 4, arrowCenter.y() - 2)
        << QPointF(arrowCenter.x() + 4, arrowCenter.y() - 2)
        << QPointF(arrowCenter.x(), arrowCenter.y() + 3);

  painter.setPen(Qt::NoPen);
  painter.setBrush(isEnabled() ? theme.secondaryColor : theme.secondaryColorDisabled);

  painter.drawPolygon(arrow);
}