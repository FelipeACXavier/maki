#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

class NodeItem;

/**
 * @brief Dialog for configuring behaviour.
 */
class BehaviourDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new BehaviourDialog with the given title and parent widget.
   *
   * @param title The title of the dialog.
   * @param parent The parent widget.
   */
  BehaviourDialog(const QString& title, QWidget* parent = nullptr);

  /**
   * @brief Sets up the dialog with the given list of nodes.
   *
   * @param nodes List of NodeItem objects to be displayed in the dialog.
   */
  void setup(const QList<NodeItem*>& nodes);

  /**
   * @brief Gets the name from the dialog.
   *
   * @return The name as a QString.
   */
  QString getName() const;

public slots:
  /**
   * @brief Handles node selection changes.
   *
   * @param node The selected NodeItem object.
   * @param selected True if the node is selected, false otherwise.
   */
  void nodeSelected(NodeItem* node, bool selected);

private:
  QLineEdit* mName;  /// Line edit for displaying and editing the name.
  QListWidget* mListWidget;  /// List widget for displaying nodes.

  /**
   * @brief Populates the list widget with nodes.
   */
  void populateNodeList();
};
