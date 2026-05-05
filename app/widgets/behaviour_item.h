#pragma once

#include <QListWidget>
#include <QListWidgetItem>

/**
 * @brief Represents an item in a list that can have behaviour.
 */
class BehaviourItem : public QListWidgetItem
{
public:
  /**
   * @brief Constructs a new BehaviourItem with the given title and parent widget.
   *
   * @param title The title of the item.
   * @param parent The parent widget, defaults to nullptr.
   */
  BehaviourItem(const QString& title, QListWidget* parent = nullptr);
};
