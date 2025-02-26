#pragma once

#include <QGraphicsItem>

#include "movable.h"

class Element : public DraggableLabel
{
  Q_OBJECT

public:
  Element(const QString& text, QWidget* parent = nullptr);
};

class MenuElement : public QLabel
{
  Q_OBJECT

public:
  MenuElement(const QString& text, QWidget* parent = nullptr);
};

class CanvasElement : public Element
{
  Q_OBJECT

public:
  CanvasElement(const QString& text, QWidget* parent = nullptr);
};
