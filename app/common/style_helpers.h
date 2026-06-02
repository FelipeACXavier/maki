#pragma once

#include <QIcon>
#include <QList>
#include <QString>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "config.h"
#include "logging.h"

/**
 * @brief Represents a widget with an associated icon and path.
 */
struct WidgetWithIcon
{
  QObject* widget;     /// Pointer to the widget.
  QString path;        /// Path to the icon.
  int index;           /// Index of the widget.
  QColor color;        /// Color for the icon.
  QModelIndex mindex;  /// Model index for the widget.
};

/**
 * @brief Represents a translatable widget with text and optional tooltip.
 */
struct TranslatableWidget
{
  QObject* widget;       /// Pointer to the widget.
  const char* text;      /// Text for the widget.
  int index;             /// Index of the widget.
  bool tooltip = false;  /// Whether the widget has a tooltip.
  QModelIndex mindex;    /// Model index for the widget.
};

/**
 * @brief Represents a tree widget with an associated icon and path.
 */
struct TreeWidgetWithIcon
{
  QTreeWidgetItem* widget;  /// Pointer to the tree widget.
  QString path;             /// Path to the icon.
  QColor color;             /// Color for the icon.
};

/**
 * @brief Adds a class name to a widget.
 *
 * @param object The widget to add the class name to.
 * @param className The class name to add.
 */
void addClass(QWidget* object, const QString& className);

/**
 * @brief Snaps a point to the nearest grid line.
 *
 * @param point The point to snap.
 * @param gridSize The size of the grid.
 * @return QPointF The snapped point.
 */
QPointF snapToGrid(const QPointF& point, const int gridSize);

/**
 * @brief Converts a string to a label format.
 *
 * @param str The string to convert.
 * @return QString The converted label.
 */
QString ToLabel(const QString& str);
QString ToLabel(const std::string& str);

/**
 * @brief Applies a color to an icon.
 *
 * @param path The path to the icon.
 * @param color The color to apply.
 * @return QPixmap The colored icon.
 */
QPixmap applyColorToIcon(const QString& path, const QColor& color);

/**
 * @brief Adds a colored icon to a QIcon.
 *
 * @param path The path to the icon.
 * @param color The color to apply.
 * @return QIcon The icon with the applied color.
 */
QIcon addIconWithColor(const QString& path, const QColor& color);

/**
 * @brief Returns a QIcon from the system theme.
 *
 * If the icon is not available, the counterpart from the icons/fallback is used
 *
 * @param path The path to the icon.
 * @return QIcon The icon with the applied color.
 */
QIcon iconFromTheme(const QString& name, bool fallback = false);

/**
 * @brief Converts a time point to a string in QT format.
 *
 * @param ts The time point to convert.
 * @return QString The converted string.
 */
QString timeToQT(std::chrono::system_clock::time_point ts);

/**
 * @brief Converts a log level to a string in QT format.
 *
 * @param logLevel The log level to convert.
 * @return QString The converted string.
 */
QString logLevelToQT(logging::LogLevel logLevel);

/**
 * @brief Converts a time point and log level to a string in QT format.
 *
 * @param ts The time point to convert.
 * @param level The log level to convert.
 * @param message The message to convert.
 * @return QString The converted string.
 */
QString toQT(std::chrono::system_clock::time_point ts, logging::LogLevel level, const std::string& message);

/**
 * @brief Adds a dynamic widget to a layout.
 *
 * @param layout The layout to add the widget to.
 * @param dynamicWidget The dynamic widget to add.
 * @param parent The parent widget of the dynamic widget.
 */
void addDynamicWidget(QVBoxLayout* layout, QWidget* dynamicWidget, QWidget* parent);

/**
 * @brief Updates a property on a widget.
 *
 * @param widget The widget to update.
 * @param property The property to update.
 * @param value The new value for the property.
 */
void updateProperty(QWidget* widget, const QString& property, bool value);

/**
 * @brief Applies a style to a widget.
 *
 * @param widget The widget to apply the style to.
 * @param style The style to apply.
 */
void applyStyle(QWidget* widget, const QString& style);

/**
 * @brief Adds a section label to a menu.
 *
 * @param menu The menu to add the label to.
 * @param text The text for the label.
 */
void addSectionLabel(QMenu* menu, const QString& text);

/**
 * @brief Elides text on the left side of a widget.
 *
 * @param text The text to elide.
 * @param component The widget to elide the text for.
 * @return QString The elided text.
 */
QString elideLeft(const QString& text, const QWidget* const component);

/**
 * @brief Elides text on the right side of a widget.
 *
 * @param text The text to elide.
 * @param component The widget to elide the text for.
 * @return QString The elided text.
 */
QString elideRight(const QString& text, const QWidget* const component);

/**
 * @brief Clears all widgets from a layout.
 *
 * @param layout The layout to clear.
 */
void clearLayout(QLayout* layout);

/**
 * @brief Updates the icon theme for a list of widget icons.
 *
 * @param icons The list of widget icons to update.
 */
void updateIconTheme(QList<WidgetWithIcon>& icons);

/**
 * @brief Updates the icon theme for a list of tree widget icons.
 *
 * @param icons The list of tree widget icons to update.
 */
void updateTreeIconTheme(QList<TreeWidgetWithIcon>& icons);

/**
 * @brief Escapes HTML characters in a string.
 *
 * @param s The string to escape.
 * @return QString The escaped string.
 */
QString htmlEscape(const QString& s);

/**
 * @brief Creates an information message for a node configuration.
 *
 * @param node The node configuration to create the message for.
 * @return QString The created message.
 */
QString createInformationMessage(const NodeConfig& node);

/**
 * @brief Creates a default message for the application.
 *
 * @return QString The default message.
 */
QString createDefaultMessage();
