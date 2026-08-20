#include "base_dialog.h"

#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QRect>
#include <QScreen>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "app_configs.h"
#include "style_helpers.h"

constexpr double MinScreenFraction = 0.10;
constexpr double TargetScreenFraction = 0.50;

BaseDialog::BaseDialog(const QString& title, double ratio, double screenFraction, QWidget* parent)
    : QDialog(parent)
    , mMinimunWidth(-1)
    , mMinimunHeight(-1)
{
  setWindowTitle(title);

  // Create and set dialog layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  setSize(ratio, screenFraction);
}

void BaseDialog::setSize(double ratio, double screenFraction)
{
  setSize(ratio, screenFraction, TargetScreenFraction);
}

void BaseDialog::setSize(double ratio, double screenFraction, qreal heightFraction)
{
  // Set the dialog size dynamically
  QScreen* screen = this->screen();
  if (!screen)
    screen = QGuiApplication::primaryScreen();

  QRect avail = screen->availableGeometry();
  int screenW = avail.width();
  int screenH = avail.height();

  int targetH = int(screenH * heightFraction);

  const qreal fraction = qMax<qreal>(MinScreenFraction, screenFraction);
  targetH = qBound(int(screenH * MinScreenFraction), targetH, int(screenH * fraction));

  int targetW = int(targetH * ratio);
  if (mMinimunWidth > 0)
    targetW = qMax<int>(mMinimunWidth, targetW);

  if (targetW > int(screenW * fraction) && mMinimunWidth < 0)
  {
    targetW = int(screenW * screenFraction);
    targetH = int(targetW / ratio);
  }

  resize(targetW, targetH);
  setMinimumSize(targetW, targetH);
}

QVBoxLayout* BaseDialog::layout()
{
  return qobject_cast<QVBoxLayout*>(QDialog::layout());
}

QDialogButtonBox* BaseDialog::createButtons(const QString& ok, const QString& cancel)
{
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);
  layout()->addWidget(buttonBox);

  QFontMetricsF metrics(Fonts::Main);
  auto okButton = buttonBox->button(QDialogButtonBox::Ok);
  if (okButton)
  {
    okButton->setDefault(false);
    okButton->setAutoDefault(false);

    okButton->setFont(Fonts::Main);
    okButton->setText(" " + ok);

    auto textWidth = metrics.horizontalAdvance(okButton->text());
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    okButton->setMinimumWidth(textWidth + 30);
    okButton->setIcon(QIcon(":/icons/accept.svg"));
  }

  auto* cancelBtn = buttonBox->button(QDialogButtonBox::Close);
  if (cancelBtn)
  {
    cancelBtn->setFont(Fonts::Main);
    cancelBtn->setText(" " + cancel);
    cancelBtn->setAutoDefault(false);

    auto textWidth = metrics.horizontalAdvance(cancelBtn->text());
    cancelBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    cancelBtn->setMinimumWidth(textWidth + 30);
    cancelBtn->setIcon(QIcon(":/icons/reject.svg"));
  }

  return buttonBox;
}

void BaseDialog::limitWidth(int minWidth)
{
  mMinimunWidth = minWidth;
}

void BaseDialog::limitHeight(int minHeight)
{
  mMinimunHeight = minHeight;
}

void BaseDialog::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
  {
    focusNextChild();
    event->accept();
    return;
  }

  QDialog::keyPressEvent(event);
}