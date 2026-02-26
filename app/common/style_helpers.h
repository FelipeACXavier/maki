#pragma once

#include <QIcon>
#include <QList>
#include <QString>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "config.h"
#include "logging.h"

void addClass(QWidget* object, const QString& className);
QPointF snapToGrid(const QPointF& point, const int gridSize);

QString ToLabel(const QString& str);
QString ToLabel(const std::string& str);

QPixmap applyColorToIcon(const QString& path, const QColor& color);
QIcon addIconWithColor(const QString& path, const QColor& color);

QString timeToQT(std::chrono::system_clock::time_point ts);
QString logLevelToQT(logging::LogLevel logLevel);
QString toQT(std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& message);

void addDynamicWidget(QVBoxLayout* layout, QWidget* dynamicWidget, QWidget* parent);

QString getDirPathFor(const QString& path);

void updateProperty(QWidget* widget, const QString& property, bool value);
void applyStyle(QWidget* widget, const QString& style);
void addSectionLabel(QMenu* menu, const QString& text);

QString elideLeft(const QString& text, const QWidget* const component);
QString elideRight(const QString& text, const QWidget* const component);

struct WidgetWithIcon
{
  QWidget* widget;
  QString path;
  int index;
  QColor color;
};

struct TreeWidgetWithIcon
{
  QTreeWidgetItem* widget;
  QString path;
  QColor color;
};

void updateIconTheme(QList<WidgetWithIcon>& icons);
void updateTreeIconTheme(QList<TreeWidgetWithIcon>& icons);

QString htmlEscape(const QString& s);
QString createInformationMessage(const NodeConfig& node);
