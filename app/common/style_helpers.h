#pragma once

#include <QIcon>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "logging.h"

void addClass(QWidget* object, const QString& className);
QPointF snapToGrid(const QPointF& point, const int gridSize);

QString ToLabel(const QString& str);
QString ToLabel(const std::string& str);

QIcon addIconWithColor(const QString& path, const QColor& color);

QString timeToQT(std::chrono::system_clock::time_point ts);
QString logLevelToQT(logging::LogLevel logLevel);
QString toQT(std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& message);

void addDynamicWidget(QVBoxLayout* layout, QWidget* dynamicWidget, QWidget* parent);

QString getDirPathFor(const QString& path);