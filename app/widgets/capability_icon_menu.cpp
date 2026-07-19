#include "widgets/capability_icon_menu.h"

#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QScrollArea>
#include <QTimer>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <algorithm>
#include <map>
#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "elements/behaviour/component_overlay.h"
#include "keys.h"
#include "save_info.h"
#include "system/config_table.h"
#include "types.h"
#include "widgets/widget_factory.h"

namespace
{
constexpr int kCols = 5;
constexpr int kIconPx = 28;
constexpr int kIconBtnPx = 36;
constexpr int kMinTileW = 72;
constexpr int kMaxTileW = 110;
constexpr int kTileHPadding = 8;

QString capabilityButtonStyle(bool selected)
{
  const char* border = selected ? "palette(highlight)" : "transparent";
  return QStringLiteral(
             "QToolButton {"
             "  border: 2px solid %1;"
             "  border-radius: 8px;"
             "  background: transparent;"
             "  padding: 1px;"
             "}"
             "QToolButton:hover { background: palette(midlight); }")
      .arg(QLatin1String(border));
}

struct Tile
{
  QString name;
  QWidget* cell = nullptr;
};

struct Section
{
  QLabel* header = nullptr;
  QWidget* gridHost = nullptr;
  QGridLayout* grid = nullptr;
  QVector<Tile> tiles;
};

void reflowGrid(QGridLayout* grid, QWidget* gridHost, int columns, const QVector<QWidget*>& visibleCells)
{
  while (QLayoutItem* layoutItem = grid->takeAt(0))
  {
    if (layoutItem->widget())
      layoutItem->widget()->setParent(gridHost);
    delete layoutItem;
  }
  for (int i = 0; i < visibleCells.size(); ++i)
    grid->addWidget(visibleCells[i], i / columns, i % columns);
}

int computeTileWidth(const QFont& font, const QVector<CapabilityIconMenuItem>& items)
{
  const QFontMetrics fm(font);
  int widest = kMinTileW - kTileHPadding;
  for (const auto& item : items)
    widest = qMax(widest, fm.horizontalAdvance(item.name));
  return qBound(kMinTileW, widest + kTileHPadding, kMaxTileW);
}
}  // namespace

QWidget* makeCapabilityIconTile(QWidget* parent,
                                const CapabilityIconMenuItem& item,
                                int tileW,
                                bool selected,
                                const std::function<void(const CapabilityIconMenuItem&)>& onSelected)
{
  auto* cell = new QWidget(parent);
  cell->setFixedWidth(tileW);
  auto* cv = new QVBoxLayout(cell);
  cv->setContentsMargins(0, 0, 0, 0);
  cv->setSpacing(2);

  auto* btn = new QToolButton(cell);
  btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
  btn->setAutoRaise(true);
  btn->setIconSize(QSize(kIconPx, kIconPx));
  btn->setFixedSize(QSize(kIconBtnPx, kIconBtnPx));
  btn->setToolTip(item.name);
  btn->setStyleSheet(capabilityButtonStyle(selected));
  if (!item.iconPath.isEmpty())
    btn->setIcon(QIcon(item.iconPath));
  cv->addWidget(btn, 0, Qt::AlignHCenter);

  auto* lbl = new QLabel(item.name, cell);
  QFont lf = lbl->font();
  lf.setPointSizeF(qMax(7.0, lf.pointSizeF() - 1.0));
  lbl->setFont(lf);
  lbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  lbl->setWordWrap(true);
  lbl->setTextInteractionFlags(Qt::NoTextInteraction);
  lbl->setFixedWidth(tileW - kTileHPadding);
  lbl->setToolTip(item.name);
  cv->addWidget(lbl, 0, Qt::AlignHCenter);

  QObject::connect(btn, &QToolButton::clicked, parent, [item, onSelected]() {
    if (onSelected)
      onSelected(item);
  });

  return cell;
}

CapabilityIconGrid::CapabilityIconGrid(QWidget* parent)
    : QWidget(parent)
    , mEmptyMessage(tr("No capabilities available"))
{
  oclero::qlementine::QlementineStyle::setAutoIconColor(this, oclero::qlementine::AutoIconColor::None);
  mGrid = new QGridLayout(this);
  mGrid->setContentsMargins(0, 0, 0, 0);
  mGrid->setHorizontalSpacing(4);
  mGrid->setVerticalSpacing(4);
}

void CapabilityIconGrid::setColumns(int columns)
{
  mColumns = qMax(1, columns);
  rebuild();
}

void CapabilityIconGrid::setEmptyMessage(const QString& message)
{
  mEmptyMessage = message;
  if (mItems.isEmpty())
    rebuild();
}

void CapabilityIconGrid::setItems(const QVector<CapabilityIconMenuItem>& items)
{
  mItems = items;
  rebuild();
}

void CapabilityIconGrid::setSelectedId(const QString& id)
{
  mSelectedId = id;
}

void CapabilityIconGrid::setSelectedName(const QString& name)
{
  mSelectedName = name;
}

void CapabilityIconGrid::rebuild()
{
  while (QLayoutItem* item = mGrid->takeAt(0))
  {
    if (QWidget* w = item->widget())
      w->deleteLater();
    delete item;
  }

  if (mItems.isEmpty())
  {
    auto* empty = new QLabel(mEmptyMessage, this);
    empty->setEnabled(false);
    mGrid->addWidget(empty, 0, 0);
    return;
  }

  const int tileW = computeTileWidth(font(), mItems);
  const int cols = mColumns;
  for (int index = 0; index < mItems.size(); ++index)
  {
    const CapabilityIconMenuItem& item = mItems[index];
    const bool selected = (item.id == mSelectedId)
                          || (mSelectedId.isEmpty() && !mSelectedName.isEmpty() && item.name == mSelectedName);
    auto* cell = makeCapabilityIconTile(this, item, tileW, selected, [this](const CapabilityIconMenuItem& chosen) {
      emit itemSelected(chosen);
    });
    mGrid->addWidget(cell, index / cols, index % cols);
  }
}

QString CapabilityIconMenu::displayNameFromQualifiedType(const QString& typeOrKey)
{
  const int sep = typeOrKey.indexOf(QLatin1String("::"));
  return sep >= 0 ? typeOrKey.mid(sep + 2) : typeOrKey;
}

QVector<CapabilityIconMenuItem> CapabilityIconMenu::itemsFromPossibleCallers(const SaveInfo& storage,
                                                                             const QString& nodeId,
                                                                             const ConfigurationTable* configTable)
{
  QVector<CapabilityIconMenuItem> items;
  for (const auto& caller : storage.getPossibleCallers(nodeId, Types::PropertyTypes::EVENT_SELECT))
  {
    const QVariant nameVar = caller->getProperty(ConfigKeys::NAME);
    if (!nameVar.isValid() || nameVar.toString().isEmpty())
      continue;

    CapabilityIconMenuItem item;
    item.id = caller->getid();
    item.name = nameVar.toString();
    item.iconPath = behaviour::resolveCapabilityIconPath(caller->getIcon(), caller->getnodeId(), configTable);
    items.push_back(item);
  }
  return items;
}

void CapabilityIconMenu::exec(QWidget* parent,
                              const QVector<CapabilityIconMenuItem>& items,
                              const QPoint& globalAnchor,
                              const std::function<void(const CapabilityIconMenuItem&)>& onSelected,
                              const QString& emptyMessage)
{
  if (!parent)
    return;

  QMenu menu(parent);
  auto* host = new QWidget(&menu);
  oclero::qlementine::QlementineStyle::setAutoIconColor(host, oclero::qlementine::AutoIconColor::None);
  auto* vbox = new QVBoxLayout(host);
  vbox->setContentsMargins(6, 4, 6, 4);
  vbox->setSpacing(4);

  if (items.isEmpty())
  {
    auto* empty = new QLabel(emptyMessage.isEmpty() ? QObject::tr("No capabilities available") : emptyMessage, host);
    empty->setEnabled(false);
    vbox->addWidget(empty);
  }
  else
  {
    auto* search = new maki::SearchWidget(QObject::tr("Filter capabilities"), host);
    vbox->addWidget(search);

    const int tileW = computeTileWidth(host->font(), items);

    // Preserve first-seen section order; empty section key => untitled flat grid.
    std::map<QString, int> sectionOrder;
    QVector<QString> sectionKeys;
    for (const auto& item : items)
    {
      const QString key = item.section;
      if (!sectionOrder.count(key))
      {
        sectionOrder[key] = sectionKeys.size();
        sectionKeys.push_back(key);
      }
    }

    QVector<Section> sections;
    sections.resize(sectionKeys.size());
    for (int s = 0; s < sectionKeys.size(); ++s)
    {
      Section& section = sections[s];
      if (!sectionKeys[s].isEmpty())
      {
        section.header = new QLabel(sectionKeys[s], host);
        QFont hf = section.header->font();
        hf.setBold(true);
        section.header->setFont(hf);
        vbox->addWidget(section.header);
      }

      section.gridHost = new QWidget(host);
      section.grid = new QGridLayout(section.gridHost);
      section.grid->setContentsMargins(0, 0, 0, 0);
      section.grid->setHorizontalSpacing(4);
      section.grid->setVerticalSpacing(4);
      vbox->addWidget(section.gridHost);
    }

    QVector<int> sectionCounts(sectionKeys.size(), 0);
    for (const auto& item : items)
    {
      const int s = sectionOrder[item.section];
      Section& section = sections[s];
      auto* cell = makeCapabilityIconTile(section.gridHost, item, tileW, false, [&menu, onSelected](const CapabilityIconMenuItem& chosen) {
        if (onSelected)
          onSelected(chosen);
        menu.close();
      });
      const int idx = sectionCounts[s]++;
      section.grid->addWidget(cell, idx / kCols, idx % kCols);
      section.tiles.push_back({item.name, cell});
    }

    QObject::connect(search, &maki::SearchWidget::valueChanged, host, [sections](const QString& query) mutable {
      const QString needle = query.trimmed();
      for (Section& section : sections)
      {
        QVector<QWidget*> visible;
        visible.reserve(section.tiles.size());
        for (const Tile& tile : section.tiles)
        {
          const bool matches = needle.isEmpty() || tile.name.contains(needle, Qt::CaseInsensitive);
          tile.cell->setVisible(matches);
          if (matches)
            visible.push_back(tile.cell);
        }
        reflowGrid(section.grid, section.gridHost, kCols, visible);
        const bool hasVisible = !visible.isEmpty();
        if (section.header)
          section.header->setVisible(hasVisible);
        section.gridHost->setVisible(hasVisible);
      }
    });

    QTimer::singleShot(0, search, [search]() { search->widget()->setFocus(); });
  }

  host->adjustSize();

  auto* scroll = new QScrollArea(&menu);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setWidgetResizable(true);
  scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scroll->setWidget(host);

  const int viewportH = parent->height();
  const int maxPopupH = qMax(240, static_cast<int>(viewportH * 0.7));
  const QSize hostHint = host->sizeHint();
  scroll->setMaximumHeight(qMin(hostHint.height() + 2, maxPopupH));
  scroll->setMinimumWidth(qMax(hostHint.width() + 2, 200));

  auto* wa = new QWidgetAction(&menu);
  wa->setDefaultWidget(scroll);
  menu.addAction(wa);

  menu.ensurePolished();
  const QSize menuHint = menu.sizeHint();
  QPoint pos = globalAnchor;
  pos.rx() -= menuHint.width() / 2;
  menu.exec(pos);
}
