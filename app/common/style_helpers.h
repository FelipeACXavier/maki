#pragma once

#include <QString>
#include <QWidget>

void addClass(QWidget* object, const QString& className);
QPointF snapToGrid(const QPointF& point, const int gridSize);

QString ToLabel(const QString& str);
QString ToLabel(const std::string& str);
