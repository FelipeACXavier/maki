#pragma once

#include <QString>
#include <QWidget>

#include "logging.h"

void addClass(QWidget* object, const QString& className);
QPointF snapToGrid(const QPointF& point, const int gridSize);

QString ToLabel(const QString& str);
QString ToLabel(const std::string& str);

QString timeToQT(struct timespec ts);
QString logLevelToQT(logging::LogLevel logLevel);
QString toQT(struct timespec ts, logging::LogLevel level, const std::string& message);
