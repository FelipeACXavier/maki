#pragma once

#include <QToolButton>

/**
 * @brief A custom QToolButton with a dropdown menu for selecting options.
 */
class DropDownButton : public QToolButton
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new DropDownButton.
   *
   * @param parent The parent widget.
   */
  DropDownButton(QWidget* parent = nullptr);

  /**
   * @brief Adds an option to the dropdown menu.
   *
   * @param name The name of the option.
   */
  void addOption(const QString& name);

  /**
   * @brief Removes an option to the dropdown menu.
   *
   * @param name The name of the option.
   */
  void removeOption(const QString& name);

  /**
   * @brief Sets the currently selected option.
   *
   * @param name The name of the option to select.
   */
  void setCurrentOption(const QString& name);

  /**
   * @brief Gets the currently selected option.
   *
   * @return The name of the currently selected option.
   */
  QString currentOption();

  /**
   * @brief Sets whether the button is running or not.
   *
   * @param running True if the button is running, false otherwise.
   */
  void setRunning(bool running);

  /**
   * @brief Sets the size of the button.
   *
   * @param width The new width.
   * @param height The new height.
   */
  void setSize(int width, int height);

  /**
   * @brief Resets the button to its default state.
   */
  void reset();

signals:
  /**
   * @brief Emitted when an option is requested to be executed.
   *
   * @param option The name of the option.
   */
  void executeRequested(const QString& option);

  /**
   * @brief Emitted when an option is requested to be edited.
   *
   * @param option The name of the option.
   */
  void editOptionRequested(const QString& option);

  /**
   * @brief Emitted when an option is requested to be deleted.
   *
   * @param option The name of the option.
   */
  void deleteOptionRequested(const QString& option);

protected:
  /**
   * @brief Provides a hint about the size of the button.
   *
   * @return The size hint.
   */
  QSize sizeHint() const override;

  /**
   * @brief Handles paint events for the button.
   *
   * @param event The paint event.
   */
  void paintEvent(QPaintEvent* event) override;

private:
  QMenu* mMenu = nullptr;   /// Pointer to the dropdown menu.
  QList<QString> mOptions;  /// List of available options.
  QString mCurrentOption;   /// Currently selected option.
  int mWidth = 150;         /// Width of the button.
  int mHeight = 40;         /// Height of the button.
  bool mRunning = false;    /// Indicates if the button is running.

  /**
   * @brief Rebuilds the dropdown menu based on current options.
   */
  void rebuildMenu();

  /**
   * @brief Updates the text displayed on the button.
   */
  void updateButtonText();

  /**
   * @brief Shows the context menu at a specified point.
   *
   * @param point The position to show the context menu.
   */
  void showContextMenu(const QPoint& point);

  /**
   * @brief Builds a menu with options, optionally including a run option.
   *
   * @param menu The menu to build.
   * @param option The current option.
   * @param addRun True if a run option should be added, false otherwise.
   */
  void buildMenu(QMenu* menu, const QString& option, bool addRun);
};
