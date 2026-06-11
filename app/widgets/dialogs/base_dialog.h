#pragma once

#include <QDialog>

#include "style_helpers.h"

class QDialogButtonBox;

/**
 * @brief Base dialog class with common functionality.
 */
class BaseDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new BaseDialog instance.
   *
   * @param title The title of the dialog.
   * @param ratio The size ratio for the dialog.
   * @param screenFraction The fraction of the screen to use for the dialog.
   * @param parent The parent widget.
   */
  BaseDialog(const QString& title, double ratio, double screenFraction, QWidget* parent = nullptr);

protected:
  /**
   * @brief Sets the size of the dialog based on the given ratio and screen fraction.
   *
   * @param ratio The size ratio for the dialog.
   * @param screenFraction The fraction of the screen to use for the dialog.
   */
  void setSize(double ratio, double screenFraction);

  /**
   * @brief Sets the size of the dialog based on the given ratio, screen fraction, and height fraction.
   *
   * @param ratio The size ratio for the dialog.
   * @param screenFraction The fraction of the screen to use for the dialog.
   * @param heightFraction The fraction of the height to use for the dialog.
   */
  void setSize(double ratio, double screenFraction, qreal heightFraction);

  /**
   * @brief Returns the layout of the dialog.
   *
   * @return QVBoxLayout* Pointer to the layout.
   */
  QVBoxLayout* layout();

  /**
   * @brief Creates a button box with OK and Cancel buttons.
   *
   * @param ok The text for the OK button.
   * @param cancel The text for the Cancel button.
   * @return QDialogButtonBox* Pointer to the created button box.
   */
  QDialogButtonBox* createButtons(const QString& ok, const QString& cancel);

  /**
   * @brief Defines the width limits for the dialog
   *
   * @param minWidth The minimum width allowed
   */
  void limitWidth(int minWidth);

  /**
   * @brief Defines the heigth limits for the dialog
   *
   * @param minHeight The minimum height allowed
   */
  void limitHeight(int minHeight);

private:
  int mMinimunWidth;
  int mMinimunHeight;
};
