#pragma once

#include <QLabel>
#include <QMimeData>
#include <QString>
#include <QWidget>

// TODO(felaze): Now this is related to QLable but I am not sure if this is the best item to
class MovableLabel : public QLabel
{
  Q_OBJECT

public:
  MovableLabel(const QString& text, QWidget* parent = nullptr);

protected:
  void mousePressEvent(QMouseEvent* event) override;
};

class DraggableLabel : public MovableLabel
{
  Q_OBJECT

public:
  DraggableLabel(const QString& text, QWidget* parent = nullptr);

protected:
  void mousePressEvent(QMouseEvent* event) override;
};
