#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QTableView>

#include "property_info.h"
#include "widgets/base_dialog.h"

class FlowSaveInfo;
class QTableWidget;

namespace maki
{
class WidgetGroup;
}

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

public slots:
  void accept() override;

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

  void addArgumentToTable(QTableWidget* table, std::shared_ptr<PropertyInfo> field);
  void removeArgumentFromTable(QTableWidget* table);
};
