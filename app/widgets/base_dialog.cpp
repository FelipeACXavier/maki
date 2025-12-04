#include "base_dialog.h"

#include <QRect>
#include <QScreen>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "style_helpers.h"
#include "theme.h"

constexpr double TargetRatio = 1.4;  // width / height, we want wider than taller
constexpr double MinScreenFraction = 0.40;
constexpr double TargetScreenFraction = 0.50;
constexpr double MaxScreenFraction = 0.70;

BaseDialog::BaseDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
{
  setWindowTitle(title);

  // Create and set dialog layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);

  setSize();
}

void BaseDialog::setSize()
{
  // Set the dialog size dynamically
  QScreen* screen = this->screen();
  if (!screen)
    screen = QGuiApplication::primaryScreen();

  QRect avail = screen->availableGeometry();
  int screenW = avail.width();
  int screenH = avail.height();

  // --- STEP 1: Target height from screen fraction ---
  int targetH = int(screenH * TargetScreenFraction);

  // Clamp if screen is too small or too large
  targetH = qBound(int(screenH * MinScreenFraction), targetH, int(screenH * MaxScreenFraction));

  // --- STEP 2: Compute width from aspect ratio ---
  int targetW = int(targetH * TargetRatio);

  // --- STEP 3: Ensure width fits the screen ---
  if (targetW > int(screenW * MaxScreenFraction))  // leave some margin
  {
    targetW = int(screenW * MaxScreenFraction);
    targetH = int(targetW / TargetRatio);
  }

  resize(targetW, targetH);
}

QVBoxLayout* BaseDialog::layout()
{
  return static_cast<QVBoxLayout*>(QDialog::layout());
}

void BaseDialog::addIcon(QWidget* widget, const QString& iconPath)
{
  mIcons.append({widget, iconPath});
}

void BaseDialog::onThemeChanged()
{
  updateIconTheme(mIcons);
}
