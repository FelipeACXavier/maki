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
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>
#include <algorithm>
#include <map>
#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "widgets/widget_factory.h"

namespace
{
constexpr int kCols = 5;
constexpr int kIconPx = 28;
constexpr int kIconBtnPx = 36;
constexpr int kMinTileW = 72;
constexpr int kMaxTileW = 110;
constexpr int kTileHPadding = 8;

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

QWidget* makeTileCell(QWidget* parent,
                      int tileW,
                      const CapabilityIconMenuItem& item,
                      QMenu* menu,
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

  QObject::connect(btn, &QToolButton::clicked, parent, [menu, item, onSelected]() {
    if (onSelected)
      onSelected(item);
    if (menu)
      menu->close();
  });

  return cell;
}

void reflowGrid(QGridLayout* grid, QWidget* gridHost, const QVector<QWidget*>& visibleCells)
{
  while (QLayoutItem* layoutItem = grid->takeAt(0))
  {
    if (layoutItem->widget())
      layoutItem->widget()->setParent(gridHost);
    delete layoutItem;
  }
  for (int i = 0; i < visibleCells.size(); ++i)
    grid->addWidget(visibleCells[i], i / kCols, i % kCols);
}
}  // namespace

QString CapabilityIconMenu::displayNameFromQualifiedType(const QString& typeOrKey)
{
  const int sep = typeOrKey.indexOf(QLatin1String("::"));
  return sep >= 0 ? typeOrKey.mid(sep + 2) : typeOrKey;
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

    const QFontMetrics fm(host->font());
    int widest = kMinTileW - kTileHPadding;
    for (const auto& item : items)
      widest = qMax(widest, fm.horizontalAdvance(item.name));
    const int tileW = qBound(kMinTileW, widest + kTileHPadding, kMaxTileW);

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
      auto* cell = makeTileCell(section.gridHost, tileW, item, &menu, onSelected);
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
        reflowGrid(section.grid, section.gridHost, visible);
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
