#pragma once

#include <QToolButton>
#include <oclero/qlementine.hpp>

class DropDownButton : public QToolButton
{
  Q_OBJECT

public:
  DropDownButton(QWidget* parent = nullptr);

  virtual void setSize(int width, int height);
  virtual void reset();

  QAction* addAction(const QString& text);
  QAction* addAction(const QIcon& icon, const QString& text);

protected:
  int mWidth = 150;
  int mHeight = 40;

  virtual void paintEvent(QPaintEvent* event) override;
  virtual int leadingContentWidth(const oclero::qlementine::Theme& theme) const;
  virtual void paintLeadingContent(QPainter& painter, const QRect& rect, const oclero::qlementine::Theme& theme);
  virtual QString displayedText() const;

private:
  bool mHovered;  /// True when the button is hovered
};
