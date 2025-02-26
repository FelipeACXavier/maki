#include "element.h"

#include "style_helpers.h"


Element::Element(const QString& text, QWidget* parent)
    : DraggableLabel(text, parent)
{
  addClass(this, QStringLiteral("element"));
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  setFixedSize(100, 50);
}

MenuElement::MenuElement(const QString& text, QWidget* parent)
    : QLabel(text, parent)
{
  addClass(this, QStringLiteral("menu-element"));
}

CanvasElement::CanvasElement(const QString& text, QWidget* parent)
    : Element(text, parent)
{
  addClass(this, QStringLiteral("canvas-element"));
}
