#include "progress_bar.h"

#include <QPainter>
#include <QStyleOptionProgressBar>
#include <oclero/qlementine.hpp>

namespace maki
{
ProgressBar::ProgressBar(QWidget* parent)
    : QProgressBar(parent)
{
}

void ProgressBar::setError(bool error)
{
  if (mError == error)
    return;

  mError = error;
  update();
}

void ProgressBar::paintEvent(QPaintEvent* event)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!mError || !qlementineStyle)
  {
    QProgressBar::paintEvent(event);
    return;
  }

  QStyleOptionProgressBar option;
  initStyleOption(&option);

  QPainter painter{this};
  painter.setRenderHint(QPainter::Antialiasing);

  // First let Qlementine draw the normal progress bar and then we override the bar
  // This is a bit annoying but oh well...
  style()->drawControl(QStyle::CE_ProgressBar, &option, &painter, this);

  if (maximum() <= minimum())
    return;

  const QRect contentsRect = style()->subElementRect(QStyle::SE_ProgressBarContents, &option, this);
  const double progress = std::clamp(static_cast<double>(value() - minimum()) / static_cast<double>(maximum() - minimum()), 0.0, 1.0);

  if (progress <= 0.0)
    return;

  QRectF progressRect = contentsRect;
  if (orientation() == Qt::Horizontal)
  {
    const qreal progressWidth = static_cast<qreal>(contentsRect.width()) * progress;
    progressRect.setWidth(progressWidth);

    const bool rightToLeft = layoutDirection() == Qt::RightToLeft;
    const bool growFromRight = invertedAppearance() != rightToLeft;

    if (growFromRight)
      progressRect.moveRight(contentsRect.right());
  }
  else
  {
    const qreal progressHeight = static_cast<qreal>(contentsRect.height()) * progress;
    progressRect.setHeight(progressHeight);

    if (!invertedAppearance())
      progressRect.moveBottom(contentsRect.bottom());
  }

  const QColor errorColor = qlementineStyle->theme().statusColorError;

  painter.setPen(Qt::NoPen);
  painter.setBrush(errorColor);

  // Qlementine uses a pill-like progress bar.
  const qreal radius = std::min(progressRect.width(), progressRect.height()) / 2.0;

  painter.drawRoundedRect(progressRect, radius, radius);

  // The red chunk may have covered the percentage text, so redraw it.
  if (option.textVisible)
    style()->drawControl(QStyle::CE_ProgressBarLabel, &option, &painter, this);
}

}  // namespace maki