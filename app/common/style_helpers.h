#pragma once

#include <QString>
#include <QWidget>

void addClass(QWidget* object, const QString& className);
QPointF snapToGrid(const QPointF& point, const int gridSize);
