#pragma once

#include <QPoint>
#include <QString>
#include <QVector>
#include <QWidget>
#include <functional>

class ConfigurationTable;
class QGridLayout;
class SaveInfo;

/** One selectable capability tile for CapabilityIconMenu / CapabilityIconGrid. */
struct CapabilityIconMenuItem
{
  QString id;        /// Opaque key returned to the caller on select.
  QString name;      /// Display label and filter text.
  QString iconPath;  /// Optional resolved icon path.
  QString section;   /// Optional section header; empty means flat list.
};

/**
 * Builds one icon+label tile. When @p selected is true, draws a highlight border
 * (used by the Call config form for the current capability).
 */
QWidget* makeCapabilityIconTile(QWidget* parent,
                                const CapabilityIconMenuItem& item,
                                int tileW,
                                bool selected,
                                const std::function<void(const CapabilityIconMenuItem&)>& onSelected);

/**
 * Flat embeddable icon grid (no search/sections).
 * Used by CallConfigPopup for in-place capability assignment.
 */
class CapabilityIconGrid : public QWidget
{
  Q_OBJECT

public:
  explicit CapabilityIconGrid(QWidget* parent = nullptr);

  void setColumns(int columns);
  void setEmptyMessage(const QString& message);
  /** Replaces items and rebuilds; uses the current selected id/name for highlight. */
  void setItems(const QVector<CapabilityIconMenuItem>& items);
  /** Selection highlight applied on the next setItems(). */
  void setSelectedId(const QString& id);
  /** Fallback match when selected id is empty (legacy name-only selection). */
  void setSelectedName(const QString& name);

signals:
  void itemSelected(const CapabilityIconMenuItem& item);

private:
  void rebuild();

  int mColumns = 5;
  QString mEmptyMessage;
  QString mSelectedId;
  QString mSelectedName;
  QVector<CapabilityIconMenuItem> mItems;
  QGridLayout* mGrid = nullptr;
};

/**
 * Searchable icon-grid popup used by the Task empty-slot (library → create).
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

  /** Maps getPossibleCallers (EVENT_SELECT) into icon-grid items for Call pickers. */
  static QVector<CapabilityIconMenuItem> itemsFromPossibleCallers(const SaveInfo& storage,
                                                                  const QString& nodeId,
                                                                  const ConfigurationTable* configTable);
};
