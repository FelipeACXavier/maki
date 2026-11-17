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
#include <oclero/qlementine.hpp>

#include "logging.h"

BaseDialog::BaseDialog(const QString& title, double widthRatio, double heightRatio, QWidget* parent, int maxWidth, int minWidth, int maxHeight, int minHeight)
    : QDialog(parent)
{
  setWindowTitle(title);

  // Create and set dialog layout
  QVBoxLayout* vlayout = new QVBoxLayout();
  setLayout(vlayout);

  setMinimumSize(minWidth, minHeight);
  setMaximumSize(maxWidth, maxHeight);

  setSize(widthRatio, heightRatio);
}

void BaseDialog::setSize(double widthRatio, double heightRatio)
{
  // Set the dialog size dynamically
  QScreen* screen = this->screen();
  if (!screen)
    screen = QGuiApplication::primaryScreen();

  // Make sure we stay within 0 and 1
  double limWidthRatio = qBound<double>(0.0, widthRatio, 1.0);
  double limHeightRatio = qBound<double>(0.0, heightRatio, 1.0);

  QRect avail = screen->availableGeometry();
  const auto targetW = avail.width() * limWidthRatio;
  const auto targetH = avail.height() * limHeightRatio;
  LOG_DEBUG("Setting size: {}, {} -> {}, {}", avail.width(), avail.height(), targetW, targetH);

  setFixedSize(targetW, targetH);
}

QVBoxLayout* BaseDialog::layout()
{
  return qobject_cast<QVBoxLayout*>(QDialog::layout());
}

QDialogButtonBox* BaseDialog::createButtons(const QString& ok, const QString& cancel)
{
  oclero::qlementine::Theme theme;
  if (const auto* style = oclero::qlementine::appStyle())
    theme = style->theme();

  QFontMetricsF metrics(theme.fontRegular);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);

  auto okButton = buttonBox->button(QDialogButtonBox::Ok);
  if (okButton)
  {
    okButton->setDefault(false);
    okButton->setAutoDefault(false);

    okButton->setText(" " + ok);

    auto textWidth = metrics.horizontalAdvance(okButton->text());
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    okButton->setMinimumWidth(textWidth + 30);
    okButton->setIcon(QIcon(":/icons/accept.svg"));
  }

  auto* cancelBtn = buttonBox->button(QDialogButtonBox::Close);
  if (cancelBtn)
  {
    cancelBtn->setText(" " + cancel);
    cancelBtn->setAutoDefault(false);

    auto textWidth = metrics.horizontalAdvance(cancelBtn->text());
    cancelBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    cancelBtn->setMinimumWidth(textWidth + 30);
    cancelBtn->setIcon(QIcon(":/icons/reject.svg"));
  }

  layout()->addWidget(buttonBox);
  return buttonBox;
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
