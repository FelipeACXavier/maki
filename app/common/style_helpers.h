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

QString timeToQT(struct timespec ts);
QString logLevelToQT(logging::LogLevel logLevel);
QString toQT(struct timespec ts, logging::LogLevel level, const std::string& message);

void addDynamicWidget(QVBoxLayout* layout, QWidget* dynamicWidget, QWidget* parent);
