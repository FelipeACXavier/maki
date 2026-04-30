#include "log_table_widget.h"

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QMenu>
#include <QPushButton>
#include <QSet>

#include "expanding_widget.h"

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
  mSourceFilter->addItem("MAKI");

  mFileFilter = new QLineEdit(this);
  mFileFilter->setPlaceholderText(tr("Filter file"));

  mSearchField = new QLineEdit(this);
  mSearchField->setPlaceholderText(tr("Find"));

  mTable = new QTableView(this);
  mTable->setModel(mProxy);
  mTable->setItemDelegate(mHighlightDelegate);
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

  auto searchBox = new ExpandingWidget(ExpandingWidget::Direction::Right, this);
  searchBox->setButtonIcon(QIcon(":/icons/search.svg"));
  searchBox->setButtonTooltip(tr("Search logs"));
  searchBox->setExpandedWidth(400);

  searchBox->addCollapsableWidget(mSearchField);
  searchBox->addCollapsableWidget(previousButton);
  searchBox->addCollapsableWidget(nextButton);

  auto filterBox = new ExpandingWidget(ExpandingWidget::Direction::Left, this);
  filterBox->setButtonIcon(QIcon(":/icons/filter.svg"));
  filterBox->setButtonTooltip(tr("Filter logs"));
  filterBox->setExpandedWidth(400);

  filterBox->addCollapsableWidget(mFileFilter);
  filterBox->addCollapsableWidget(mLevelFilter);
  filterBox->addCollapsableWidget(mSourceFilter);

  auto* filterLayout = new QHBoxLayout();
  filterLayout->addWidget(searchBox);
  filterLayout->addStretch();
  filterLayout->addWidget(filterBox);

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

  connect(searchBox, &ExpandingWidget::areaExpanded, [this](QPushButton* button) { onAreaExpanded(button, mSearchField); });
  connect(searchBox, &ExpandingWidget::areaCollapsed, [this](QPushButton* button) { onAreaCollapsed(button, mSearchField, ":/icons/search.svg"); });
  connect(filterBox, &ExpandingWidget::areaExpanded, [this](QPushButton* button) { onAreaExpanded(button, mFileFilter); });
  connect(filterBox, &ExpandingWidget::areaCollapsed, [this](QPushButton* button) { onAreaCollapsed(button, mFileFilter, ":/icons/filter.svg"); });

  connect(mTable, &QTableView::customContextMenuRequested, this, &LogTableWidget::showContextMenu);
  connect(mTable, &QTableView::clicked, this, &LogTableWidget::onClicked);

  connect(mProxy, &QAbstractItemModel::layoutChanged, mHighlightDelegate, &LogHighlightDelegate::clearExpandedRows);
}

void LogTableWidget::onAreaExpanded(QPushButton* button, QLineEdit* lineEdit)
{
  if (lineEdit)
  {
    lineEdit->setFocus();
    lineEdit->selectAll();
  }

  if (button)
    button->setIcon(QIcon(":/icons/circle-close.svg"));
}

void LogTableWidget::onAreaCollapsed(QPushButton* button, QLineEdit* lineEdit, const QString& iconName)
{
  if (lineEdit)
    lineEdit->clear();

  if (button)
    button->setIcon(QIcon(iconName));
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

void LogTableWidget::append(logging::LogLevel level, const QString& file, const uint32_t line, const QString& message)
{
  mModel->append(level, "MAKI", file, line, message);

  const QModelIndex lastSourceIndex = mModel->index(mModel->rowCount() - 1, 0);
  const QModelIndex lastProxyIndex = mProxy->mapFromSource(lastSourceIndex);

  if (lastProxyIndex.isValid())
    mTable->scrollToBottom();
}

void LogTableWidget::append(logging::LogLevel level, const std::string& file, const uint32_t line, const std::string& message)
{
  append(level, QString::fromStdString(file), line, QString::fromStdString(message));
}

void LogTableWidget::clear()
{
  mModel->clear();
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

  mTable->viewport()->update();
}

void LogTableWidget::nextSearchMatch()
{
  if (mSearchMatches.isEmpty())
    return;

  mCurrentSearchMatch =
      (mCurrentSearchMatch + 1) % mSearchMatches.size();

  focusCurrentSearchMatch();
}

void LogTableWidget::previousSearchMatch()
{
  if (mSearchMatches.isEmpty())
    return;

  mCurrentSearchMatch = (mCurrentSearchMatch - 1 + mSearchMatches.size()) % mSearchMatches.size();

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
