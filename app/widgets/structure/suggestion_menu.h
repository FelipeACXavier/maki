#pragma once

#include <QString>
#include <QWidget>
#include <vector>

#include "../frame.h"

class QEnterEvent;
class QEvent;
class QMouseEvent;
class QPropertyAnimation;
class QTimer;
class QVBoxLayout;
class NodeItem;
class CanvasView;
class QCheckBox;
class QLabel;

namespace maki
{
class WidgetGroup;
}

class SuggestionRow : public QWidget
{
  Q_OBJECT

public:
  SuggestionRow(const QString& svgPath, const QString& labelText, QWidget* parent = nullptr);

  const QString& text() const;

  bool isSelected() const;
  void setSelected(bool selected);

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

private:
  void setHovered(bool hovered);

private:
  QString mText;

  QCheckBox* mCheckBox = nullptr;
  QLabel* mLabel = nullptr;

  bool mHovered = false;
};

class SuggestionMenu : public StyledFrame
{
  Q_OBJECT

public:
  SuggestionMenu(QWidget* parent = nullptr);

  void setSuggestions(const QStringList& consumers, const QStringList& producers);

  void showMenu(NodeItem* node, CanvasView* view);
  void hideMenu();

  void updatePosition(const NodeItem* node, CanvasView* view);

signals:
  void accepted(const QStringList& suggestions);
  void dismissed();

protected:
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;

private:
  maki::WidgetGroup* mConsumersGroup = nullptr;
  maki::WidgetGroup* mProducersGroup = nullptr;

  std::vector<SuggestionRow*> mConsumerRows;
  std::vector<SuggestionRow*> mProducerRows;

  QTimer* mFadeTimer = nullptr;
  QPropertyAnimation* mFadeAnimation = nullptr;

  bool mMouseInside = false;

  void clearSuggestions();

  QStringList selectedSuggestions() const;

  void startFadeOut();
  void cancelFadeOut();
};