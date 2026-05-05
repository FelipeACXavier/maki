#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief A widget that can be collapsed or expanded.
 */
class CollapsibleAreaWidth : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int contentWidth READ contentWidth WRITE setContentWidth)
public:
  /**
   * @brief Constructs a new CollapsibleAreaWidth with the given parent.
   *
   * @param parent The parent widget.
   */
  CollapsibleAreaWidth(QWidget* parent = nullptr);

  /**
   * @brief Returns the current content width.
   *
   * @return The content width.
   */
  int contentWidth() const;

  /**
   * @brief Sets the content width to the given value.
   *
   * @param width The new content width.
   */
  void setContentWidth(int width);

  /**
   * @brief Returns a size hint for this widget.
   *
   * @return A QSize representing the size hint.
   */
  QSize sizeHint() const override;

  /**
   * @brief Returns the minimum size hint for this widget.
   *
   * @return A QSize representing the minimum size hint.
   */
  QSize minimumSizeHint() const override;

private:
  int mContentWidth;  /// The current content width.
};

/**
 * @brief A widget that can be collapsed or expanded vertically.
 */
class CollapsibleAreaHeight : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int contentHeight READ contentHeight WRITE setContentHeight)
public:
  /**
   * @brief Constructs a new CollapsibleAreaHeight with the given parent.
   *
   * @param parent The parent widget.
   */
  CollapsibleAreaHeight(QWidget* parent = nullptr);

  /**
   * @brief Returns the current content height.
   *
   * @return The content height.
   */
  int contentHeight() const;

  /**
   * @brief Sets the content height to the given value.
   *
   * @param height The new content height.
   */
  void setContentHeight(int height);

  /**
   * @brief Returns a size hint for this widget.
   *
   * @return A QSize representing the size hint.
   */
  QSize sizeHint() const override;

  /**
   * @brief Returns the minimum size hint for this widget.
   *
   * @return A QSize representing the minimum size hint.
   */
  QSize minimumSizeHint() const override;

private:
  int mContentHeight;  /// The current content height.
};
