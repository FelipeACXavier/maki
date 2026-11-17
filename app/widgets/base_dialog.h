#pragma once

#include <QDialog>
#include <QVBoxLayout>

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
  BaseDialog(const QString& title, double widthRatio, double heightRatio, QWidget* parent = nullptr, int maxWidth = 300, int minWidth = 150,
             int maxHeight = 400, int minHeight = 150);

protected:
  /**
   * @brief Sets the size of the dialog based on the given ratio and screen fraction.
   *
   * @param ratio The size ratio for the dialog.
   * @param screenFraction The fraction of the screen to use for the dialog.
   */
  void setSize(double widthRatio, double heightRatio);

  void keyPressEvent(QKeyEvent* event) override;

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
};
