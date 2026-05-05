#pragma once

#include <QPushButton>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "../base_dialog.h"

namespace maki {

/**
 * @brief A dialog for prompting the user with a message and options.
 */
class PromptDialog : public BaseDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructs a new PromptDialog instance.
   *
   * @param title The title of the dialog.
   * @param accept The text for the accept button.
   * @param reject The text for the reject button.
   * @param message The message to display in the dialog.
   * @param parent The parent widget.
   */
  PromptDialog(const QString& title, const QString& accept, const QString& reject,
               const QString& message, QWidget* parent = nullptr);

  /**
   * @brief Sets the type of the status badge.
   *
   * @param type The new type for the status badge.
   */
  void setType(oclero::qlementine::StatusBadge type);

  /**
   * @brief Sets extra information to be displayed in the dialog.
   *
   * @param info The extra information text.
   */
  void setExtraInfo(const QString& info);

private:
  oclero::qlementine::StatusBadgeWidget* mBadge;  /// Pointer to the status badge widget.
};

/**
 * @brief Displays a confirmation prompt dialog.
 *
 * @param message The message to display in the dialog.
 * @param extraInfo Optional extra information to be displayed.
 * @param parent The parent widget.
 * @return true if the user accepted, false otherwise.
 */
bool confirmationPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);

/**
 * @brief Displays a warning prompt dialog.
 *
 * @param message The message to display in the dialog.
 * @param extraInfo Optional extra information to be displayed.
 * @param parent The parent widget.
 * @return true if the user accepted, false otherwise.
 */
bool warningPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);

/**
 * @brief Displays an error prompt dialog.
 *
 * @param message The message to display in the dialog.
 * @param extraInfo Optional extra information to be displayed.
 * @param parent The parent widget.
 * @return true if the user accepted, false otherwise.
 */
bool errorPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);

}  // namespace maki
