#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class SectionWidget : public QWidget
{
  Q_OBJECT

public:
  SectionWidget(QWidget* parent = nullptr);

  void addItem(QWidget* content, const QString& title);
  void setExpanded(bool expanded);
  bool isExpanded() const;

  QWidget* content() const;

private:
  QToolButton* mToggleButton = nullptr;
  QWidget* mContent = nullptr;
};