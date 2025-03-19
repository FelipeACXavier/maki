#pragma once

#include <QListWidget>
#include <QListWidgetItem>

class BehaviourItem : public QListWidgetItem
{
public:
  BehaviourItem(const QString& title, QListWidget* parent = nullptr);
};
