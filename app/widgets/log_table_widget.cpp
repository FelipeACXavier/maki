#include "log_table_widget.h"

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QMenu>
#include <QPushButton>
#include <QSet>

#include "app_configs.h"
#include "clickable_icon.h"
#include "expanding_widget.h"

class CenterIconDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter* painter,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const override
  {
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    opt.text.clear();
    opt.icon = QIcon();
    opt.features &= ~QStyleOptionViewItem::HasDisplay;
    opt.features &= ~QStyleOptionViewItem::HasDecoration;

    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    const auto icon = qvariant_cast<QIcon>(
        index.data(Qt::DecorationRole));

    if (icon.isNull())
      return;

    const QSize iconSize = opt.decorationSize.isValid() ? opt.decorationSize : QSize(16, 16);

    const QRect iconRect = QStyle::alignedRect(opt.direction, Qt::AlignCenter, iconSize, option.rect);

    icon.paint(painter, iconRect, Qt::AlignCenter);
  }
};

LogTableWidget::LogTableWidget(QWidget* parent)
    : QWidget(parent)
{
  mModel = new LogTableModel(this);

  mProxy = new LogFilterProxyModel(this);
  mProxy->setSourceModel(mModel);

  mHighlightDelegate = new LogHighlightDelegate(this);

  mLevelFilter = new QComboBox(this);
  mLevelFilter->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
  mLevelFilter->setMaximumWidth(100);
  mLevelFilter->addItem("All");
  for (int i = (int)logging::LogLevel::Error; i <= (int)logging::LogLevel::Trace; ++i)
    mLevelFilter->addItem(mModel->toString((logging::LogLevel)i));

  mSourceFilter = new QComboBox(this);
  mSourceFilter->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
  mSourceFilter->setMaximumWidth(100);
  mSourceFilter->setDuplicatesEnabled(false);
  mSourceFilter->addItem("All");
  mSourceFilter->addItem(Config::APPLICATION_NAME);
  mSources.insert("All");
  mSources.insert(Config::APPLICATION_NAME);

  mTable = new QTableView(this);
  mTable->setModel(mProxy);
  mTable->setItemDelegate(mHighlightDelegate);
  mTable->setItemDelegateForColumn(LogTableModel::LevelColumn, new CenterIconDelegate(mTable));

  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mTable->setSelectionMode(QAbstractItemView::SingleSelection);
  mTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  mTable->setSortingEnabled(false);
  mTable->verticalHeader()->setVisible(false);
  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  mTable->setContextMenuPolicy(Qt::CustomContextMenu);

  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::LevelColumn, QHeaderView::Fixed);
  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::TimeColumn, QHeaderView::ResizeToContents);
  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::SourceColumn, QHeaderView::Fixed);
  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::FileColumn, QHeaderView::Fixed);
  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::LineColumn, QHeaderView::Fixed);
  mTable->horizontalHeader()->setSectionResizeMode(LogTableModel::MessageColumn, QHeaderView::Stretch);

  mTable->setColumnWidth(LogTableModel::LevelColumn, 30);
  mTable->setColumnWidth(LogTableModel::SourceColumn, 80);
  mTable->setColumnWidth(LogTableModel::FileColumn, 160);
  mTable->setColumnWidth(LogTableModel::LineColumn, 60);
  mTable->setTextElideMode(Qt::ElideRight);
  mTable->setWordWrap(true);

  // Save the height
  mDefaultRowHeight = mTable->verticalHeader()->defaultSectionSize();

  QPushButton* previousButton = new QPushButton(this);
  previousButton->setIcon(QIcon(":/icons/arrow-up.svg"));
  previousButton->setFixedWidth(30);
  previousButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QPushButton* nextButton = new QPushButton(this);
  nextButton->setIcon(QIcon(":/icons/arrow-down.svg"));
  nextButton->setFixedWidth(30);
  nextButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  mSearchBox = new ExpandingWidget(ExpandingWidget::Direction::Right, this);
  mSearchBox->setButtonIcon(QIcon(":/icons/search.svg"));
  mSearchBox->setButtonTooltip(tr("Search logs"));
  mSearchBox->setExpandedWidth(400);

  mSearchCounterLabel = new QLabel("", mSearchBox);
  mSearchCounterLabel->setFocusPolicy(Qt::NoFocus);
  mSearchCounterLabel->setMinimumWidth(60);

  mSearchField = new QLineEdit(mSearchBox);
  mSearchField->setPlaceholderText(tr("Find"));
  mSearchField->installEventFilter(this);

  mSearchBox->addCollapsableWidget(mSearchField);
  mSearchBox->addCollapsableWidget(previousButton);
  mSearchBox->addCollapsableWidget(nextButton);
  mSearchBox->addCollapsableWidget(mSearchCounterLabel);

  mFilterBox = new ExpandingWidget(ExpandingWidget::Direction::Left, this);
  mFilterBox->setButtonIcon(QIcon(":/icons/filter.svg"));
  mFilterBox->setButtonTooltip(tr("Filter logs"));
  mFilterBox->setExpandedWidth(400);

  mFileFilter = new QLineEdit(this);
  mFileFilter->setPlaceholderText(tr("Filter file"));
  mFileFilter->installEventFilter(this);

  mFilterBox->addCollapsableWidget(mFileFilter);
  mFilterBox->addCollapsableWidget(mLevelFilter);
  mFilterBox->addCollapsableWidget(mSourceFilter);

  auto* filterLayout = new QHBoxLayout();
  filterLayout->addWidget(mSearchBox);
  filterLayout->addStretch();
  filterLayout->addWidget(mFilterBox);

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(5);
  mainLayout->addLayout(filterLayout);
  mainLayout->addWidget(mTable);

  connect(mLevelFilter, &QComboBox::currentTextChanged, mProxy, &LogFilterProxyModel::setLevelFilter);
  connect(mSourceFilter, &QComboBox::currentTextChanged, mProxy, &LogFilterProxyModel::setSourceFilter);
  connect(mFileFilter, &QLineEdit::textChanged, mProxy, &LogFilterProxyModel::setSourceFilter);
  connect(mSearchField, &QLineEdit::textChanged, this, &LogTableWidget::setSearchText);
  connect(previousButton, &QPushButton::pressed, this, &LogTableWidget::previousSearchMatch);
  connect(nextButton, &QPushButton::pressed, this, &LogTableWidget::nextSearchMatch);

  connect(mSearchBox, &ExpandingWidget::areaExpanded, [this](ClickableIcon* button) { onAreaExpanded(button, mSearchField); });
  connect(mSearchBox, &ExpandingWidget::areaCollapsed, [this](ClickableIcon* button) { onAreaCollapsed(button, mSearchField, ":/icons/search.svg"); });
  connect(mFilterBox, &ExpandingWidget::areaExpanded, [this](ClickableIcon* button) { onAreaExpanded(button, mFileFilter); });
  connect(mFilterBox, &ExpandingWidget::areaCollapsed, [this](ClickableIcon* button) { onAreaCollapsed(button, mFileFilter, ":/icons/filter.svg"); });

  connect(mTable, &QTableView::customContextMenuRequested, this, &LogTableWidget::showContextMenu);
  connect(mTable, &QTableView::clicked, this, &LogTableWidget::onClicked);

  connect(mProxy, &QAbstractItemModel::layoutChanged, mHighlightDelegate, &LogHighlightDelegate::clearExpandedRows);
}

void LogTableWidget::onAreaExpanded(ClickableIcon* button, QLineEdit* lineEdit)
{
  if (lineEdit)
  {
    lineEdit->setFocus();
    lineEdit->selectAll();
  }

  if (button)
    button->setIcon(QIcon(":/icons/circle-close.svg"));
}

void LogTableWidget::onAreaCollapsed(ClickableIcon* button, QLineEdit* lineEdit, const QString& iconName)
{
  if (lineEdit)
  {
    lineEdit->clearFocus();
    lineEdit->clear();
  }

  if (button)
    button->setIcon(QIcon(iconName));

  mTable->setFocus();
}

void LogTableWidget::onClicked(const QModelIndex& index)
{
  if (!index.isValid())
    return;

  const int clickedRow = index.row();

  // Click expanded row again: collapse it
  if (mHighlightDelegate->containExpandedRow(clickedRow))
  {
    mHighlightDelegate->removeExpandedRow(clickedRow);

    mTable->resizeRowToContents(clickedRow);
    mTable->setRowHeight(clickedRow, mDefaultRowHeight);
    return;
  }

  // Do nothing if the message already fits
  if (!isMessageElided(clickedRow))
    return;

  // Expand clicked row
  mHighlightDelegate->addExpandedRow(clickedRow);
  mTable->resizeRowToContents(clickedRow);
}

void LogTableWidget::append(logging::LogLevel level, const QString& source, const QString& file, const uint32_t line, const QString& message)
{
  // Remove new lines so the text fits nicely in the table
  auto cleaned = message;
  cleaned.replace("\r\n", " ");
  cleaned.replace("\n", " ");
  cleaned.replace("\r", " ");
  mModel->append(level, source, file, line, cleaned);

  if (!mSources.contains(source))
  {
    mSourceFilter->addItem(source);
    mSources.insert(source);
  }

  const QModelIndex lastSourceIndex = mModel->index(mModel->rowCount() - 1, 0);
  const QModelIndex lastProxyIndex = mProxy->mapFromSource(lastSourceIndex);

  if (lastProxyIndex.isValid())
    mTable->scrollToBottom();
}

void LogTableWidget::append(logging::LogLevel level, const std::string& source, const std::string& file, const uint32_t line, const std::string& message)
{
  append(level, QString::fromStdString(source), QString::fromStdString(file), line, QString::fromStdString(message));
}

void LogTableWidget::clear()
{
  mModel->clear();
}

void LogTableWidget::search()
{
  mSearchBox->expandArea();
}

void LogTableWidget::setSearchText(const QString& text)
{
  mSearchText = text;
  mHighlightDelegate->setSearchText(text);

  rebuildSearchMatches();

  if (!mSearchMatches.isEmpty())
  {
    mCurrentSearchMatch = 0;
    focusCurrentSearchMatch();
  }
  else
  {
    mCurrentSearchMatch = -1;
  }

  updateSearchCounter();
  mTable->viewport()->update();
}

void LogTableWidget::nextSearchMatch()
{
  if (mSearchMatches.isEmpty())
    return;

  mCurrentSearchMatch = (mCurrentSearchMatch + 1) % mSearchMatches.size();

  updateSearchCounter();
  focusCurrentSearchMatch();
}

void LogTableWidget::previousSearchMatch()
{
  if (mSearchMatches.isEmpty())
    return;

  mCurrentSearchMatch = (mCurrentSearchMatch - 1 + mSearchMatches.size()) % mSearchMatches.size();

  updateSearchCounter();
  focusCurrentSearchMatch();
}

void LogTableWidget::rebuildSearchMatches()
{
  mSearchMatches.clear();

  if (mSearchText.isEmpty())
    return;

  for (int row = 0; row < mProxy->rowCount(); ++row)
  {
    for (int col = 0; col < mProxy->columnCount(); ++col)
    {
      const QModelIndex index = mProxy->index(row, col);
      const QString text = index.data(Qt::DisplayRole).toString();

      if (text.contains(mSearchText, Qt::CaseInsensitive))
        mSearchMatches.push_back(index);
    }
  }

  updateSearchCounter();
}

void LogTableWidget::focusCurrentSearchMatch()
{
  if (mCurrentSearchMatch < 0 || mCurrentSearchMatch >= mSearchMatches.size())
    return;

  const QModelIndex index = mSearchMatches[mCurrentSearchMatch];

  mTable->selectionModel()->clearSelection();
  mTable->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);

  mTable->setCurrentIndex(index);
  mTable->scrollTo(index, QAbstractItemView::PositionAtCenter);
}

void LogTableWidget::updateSearchCounter()
{
  if (mSearchMatches.isEmpty())
  {
    mSearchCounterLabel->setText(mSearchText.isEmpty() ? "" : "0/0");
    return;
  }

  mSearchCounterLabel->setText(QString("%1/%2").arg(mCurrentSearchMatch + 1).arg(mSearchMatches.size()));
}

void LogTableWidget::showContextMenu(const QPoint& pos)
{
  QMenu menu(this);

  menu.addAction("Copy selected rows", this, [this]() {
    copySelectedRows();
  });

  menu.addAction("Copy messages", this, [this]() {
    copySelectedColumn(LogTableModel::MessageColumn);
  });

  menu.addAction("Copy sources", this, [this]() {
    copySelectedColumn(LogTableModel::FileColumn);
  });

  menu.addAction("Copy sources and messages", this, [this]() {
    copySelectedColumns({LogTableModel::FileColumn, LogTableModel::MessageColumn});
  });

  menu.exec(mTable->viewport()->mapToGlobal(pos));
}

QVector<int> LogTableWidget::selectedRows() const
{
  QVector<int> rows;

  const auto indexes = mTable->selectionModel()->selectedRows();

  for (const QModelIndex& index : indexes)
    rows.push_back(index.row());

  std::sort(rows.begin(), rows.end());

  return rows;
}

void LogTableWidget::copySelectedRows() const
{
  copySelectedColumns({LogTableModel::TimeColumn,
                       LogTableModel::LevelColumn,
                       LogTableModel::SourceColumn,
                       LogTableModel::FileColumn,
                       LogTableModel::LineColumn,
                       LogTableModel::MessageColumn});
}

void LogTableWidget::copySelectedColumn(int column) const
{
  copySelectedColumns({column});
}

void LogTableWidget::copySelectedColumns(std::initializer_list<int> columns) const
{
  const QVector<int> rows = selectedRows();

  if (rows.isEmpty())
    return;

  QStringList lines;

  for (int row : rows)
  {
    QStringList parts;

    for (int column : columns)
      parts << mProxy->index(row, column).data(Qt::DisplayRole).toString();

    lines << parts.join('\t');
  }

  QApplication::clipboard()->setText(lines.join('\n'));
}

bool LogTableWidget::isMessageElided(int proxyRow) const
{
  const QModelIndex index = mProxy->index(proxyRow, LogTableModel::MessageColumn);

  if (!index.isValid())
    return false;

  const QString text = index.data(Qt::DisplayRole).toString();
  if (text.contains("\n"))
    return true;

  const int columnWidth = mTable->columnWidth(LogTableModel::MessageColumn);

  QStyleOptionViewItem opt;
  opt.initFrom(mTable);
  opt.font = mTable->font();

  const int availableWidth = columnWidth - 12;
  const QString elided = mTable->fontMetrics().elidedText(text, Qt::ElideRight, availableWidth);

  return elided != text;
}

bool LogTableWidget::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == mSearchField && event->type() == QEvent::KeyPress)
  {
    auto* keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
      if (keyEvent->modifiers() & Qt::ShiftModifier)
        previousSearchMatch();
      else
        nextSearchMatch();

      return true;
    }

    if (keyEvent->key() == Qt::Key_Escape)
    {
      mSearchBox->collapseArea();
      return true;
    }
  }

  if (watched == mFileFilter && event->type() == QEvent::KeyPress)
  {
    auto* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Escape)
    {
      mFilterBox->collapseArea();
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}
