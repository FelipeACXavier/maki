#pragma once

#include <QPoint>
#include <QString>
#include <QVector>
#include <functional>

class QWidget;

/** One selectable capability tile for CapabilityIconMenu. */
struct CapabilityIconMenuItem
{
  QString id;        /// Opaque key returned to the caller on select.
  QString name;      /// Display label and filter text.
  QString iconPath;  /// Optional resolved icon path.
  QString section;   /// Optional section header; empty means flat list.
};

/**
 * Searchable icon-grid popup used by Task empty-slot and Call capability pickers.
 * Anchors near @p globalAnchor (typically under a canvas slot).
 */
class CapabilityIconMenu
{
public:
  static void exec(QWidget* parent,
                   const QVector<CapabilityIconMenuItem>& items,
                   const QPoint& globalAnchor,
                   const std::function<void(const CapabilityIconMenuItem&)>& onSelected,
                   const QString& emptyMessage = QString());

  /** Strips a leading "Library::" prefix from a qualified type/key. */
  static QString displayNameFromQualifiedType(const QString& typeOrKey);
};
