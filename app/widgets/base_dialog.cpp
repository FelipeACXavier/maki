#include "base_dialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QRect>
#include <QScreen>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "style_helpers.h"
#include "theme.h"

constexpr double MinScreenFraction = 0.10;
constexpr double TargetScreenFraction = 0.50;

BaseDialog::BaseDialog(const QString& title, double ratio, double screenFraction, QWidget* parent)
    : QDialog(parent)
{
  setWindowTitle(title);

  // Create and set dialog layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  setSize(ratio, screenFraction);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
  if (targetW > int(screenW * fraction))
  {
    targetW = int(screenW * screenFraction);
    targetH = int(targetW / ratio);
  }

  resize(targetW, targetH);
  setFixedSize(targetW, targetH);
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
    okButton->setFixedWidth(qMax<int>(150, textWidth));
    okButton->setIcon(QIcon(":/icons/accept.svg"));
  }

  auto* cancelBtn = buttonBox->button(QDialogButtonBox::Close);
  if (cancelBtn)
  {
    cancelBtn->setFont(Fonts::Main);
    cancelBtn->setText(" " + cancel);

    auto textWidth = metrics.horizontalAdvance(cancelBtn->text());
    cancelBtn->setFixedWidth(qMax<int>(150, textWidth));
    cancelBtn->setIcon(QIcon(":/icons/reject.svg"));
  }

  return buttonBox;
}
