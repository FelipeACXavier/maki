#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QTableView>

#include "base_dialog.h"

class FlowSaveInfo;
class QStandardItem;
class QStandardItemModel;

/**
 * @brief Dialog for managing event information.
 */
class EventDialog : public BaseDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructs an EventDialog with the given title and parent widget.
   *
   * @param title The title of the dialog.
   * @param parent The parent widget.
   */
  EventDialog(const QString& title, QWidget* parent = nullptr);

  /**
   * @brief Sets up the dialog with event information.
   *
   * @param event Shared pointer to the FlowSaveInfo object containing event data.
   */
  void setup(std::shared_ptr<FlowSaveInfo> event);

  /**
   * @brief Retrieves the stored FlowSaveInfo object.
   *
   * @return const std::shared_ptr<FlowSaveInfo>& The stored FlowSaveInfo object.
   */
  std::shared_ptr<FlowSaveInfo> getInfo() const;

  /**
   * @brief Retrieves the name of the event.
   *
   * @return QString The name of the event.
   */
  QString getName() const;

  /**
   * @brief Retrieves the type of the event.
   *
   * @return QString The type of the event.
   */
  QString getType() const;

  /**
   * @brief Retrieves the return type of the event.
   *
   * @return QString The return type of the event.
   */
  QString getReturnType() const;

  /**
   * @brief Retrieves the model containing argument information.
   *
   * @return QStandardItemModel* Pointer to the QStandardItemModel containing arguments.
   */
  QStandardItemModel* getArguments() const;

protected:
  /**
   * @brief Handles key press events.
   *
   * @param event The QKeyEvent object.
   */
  void keyPressEvent(QKeyEvent* event) override;

private:
  std::shared_ptr<FlowSaveInfo> mStorage;  /// Pointer to the stored FlowSaveInfo object.
  int mEnterCount;                         /// Counter for enter key presses.

  /**
   * @brief Populates the node list with available options.
   */
  void populateNodeList();

  /**
   * @brief Creates and configures the name input widget.
   */
  void createNameInput();

  /**
   * @brief Creates and configures the type input widget.
   */
  void createTypeInput();

  /**
   * @brief Creates and configures the return type input widget.
   */
  void createReturnTypeInput();

  /**
   * @brief Creates and configures the argument input widget.
   */
  void createArgumentInput();

  /**
   * @brief Updates the argument table at the specified row and column with the given text.
   *
   * @param row The row index in the argument table.
   * @param column The column index in the argument table.
   * @param text The new text to set.
   */
  void updateArgumentTable(int row, int column, const QString& text);
};
