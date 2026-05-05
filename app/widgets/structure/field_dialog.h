#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>

#include "property_info.h"

class QStandardItem;
class QStandardItemModel;

/**
 * @brief A dialog for editing field properties.
 */
class FieldDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new FieldDialog with the given title and parent widget.
   *
   * @param title The title of the dialog.
   * @param parent The parent widget.
   */
  FieldDialog(const QString& title, QWidget* parent = nullptr);

  /**
   * @brief Sets up the dialog with the provided property configuration.
   *
   * @param config A shared pointer to the PropertyInfo object containing the configuration.
   */
  void setup(std::shared_ptr<PropertyInfo> config);

  /**
   * @brief Retrieves the current property information.
   *
   * @return A shared pointer to the PropertyInfo object.
   */
  std::shared_ptr<PropertyInfo> getInfo() const;

  /**
   * @brief Retrieves the name of the field.
   *
   * @return The name of the field as a QString.
   */
  QString getName() const;

  /**
   * @brief Retrieves the type of the field.
   *
   * @return The type of the field as a QString.
   */
  QString getType() const;

  /**
   * @brief Retrieves the return type of the field.
   *
   * @return The return type of the field as a QString.
   */
  QString getReturnType() const;

  /**
   * @brief Retrieves the model containing argument information.
   *
   * @return A pointer to the QStandardItemModel containing arguments.
   */
  QStandardItemModel* getArguments() const;

protected:
  /**
   * @brief Handles key press events for the dialog.
   *
   * @param event The QKeyEvent object representing the key press.
   */
  void keyPressEvent(QKeyEvent* event) override;

private:
  std::shared_ptr<PropertyInfo> mStorage; /// Pointer to the storage containing property information.
  int mEnterCount; /// Counter for enter key presses.

  /**
   * @brief Populates the node list with available options.
   */
  void populateNodeList();

  /**
   * @brief Creates the input widget for the field name.
   *
   * @param parent The parent widget for the input widget.
   */
  void createNameInput(QWidget* parent);

  /**
   * @brief Creates the input widget for the field type.
   *
   * @param parent The parent widget for the input widget.
   */
  void createTypeInput(QWidget* parent);

  /**
   * @brief Creates the input widget for the field arguments.
   *
   * @param parent The parent widget for the input widget.
   */
  void createArgumentInput(QWidget* parent);
};
