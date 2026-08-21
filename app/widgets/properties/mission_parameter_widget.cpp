#include "mission_parameter_widget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../dialogs/type_value_dialog.h"
#include "app_configs.h"
#include "type_selector.h"
#include "typing/helpers.h"

namespace maki
{

MissionParameterWidget::MissionParameterWidget(QWidget* parent)
    : QWidget(parent)
    , mStorage(nullptr)
{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);

  // ------------------------------------------------------
  // Table
  mTable = new QTableWidget(this);
  mTable->setColumnCount(4);
  mTable->setHorizontalHeaderLabels({"Name", "Type", "Value", ""});

  mTable->verticalHeader()->hide();
  mTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  mTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  mTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
  mTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // ------------------------------------------------------
  // Separator
  auto* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);

  // ------------------------------------------------------
  // Buttons
  auto* browserButtons = new QHBoxLayout();
  auto* addButton = new QPushButton(this);
  addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  addButton->setIcon(iconFromTheme("plus"));

  auto* removeButton = new QPushButton(this);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));

  connect(addButton, &QPushButton::clicked, this, &MissionParameterWidget::addParameter);
  connect(removeButton, &QPushButton::clicked, this, &MissionParameterWidget::removeParameter);

  browserButtons->addWidget(addButton);
  browserButtons->addWidget(removeButton);
  browserButtons->addStretch();

  layout->addWidget(mTable);
  layout->addStretch();
  layout->addWidget(line);
  layout->addLayout(browserButtons);
}

void MissionParameterWidget::setParameters(QVector<MissionParameter> parameters)
{
  if (mStorage)
    mStorage->setParameters(parameters);

  rebuildTable();
}

void MissionParameterWidget::setStorage(std::shared_ptr<SaveInfo> storage)
{
  mStorage = storage;
  rebuildTable();
}

void MissionParameterWidget::rebuildTable()
{
  if (!mStorage)
  {
    LOG_WARNING("Something went wrong, no database available");
    return;
  }

  mTable->setRowCount(0);

  for (int i = 0; i < mStorage->missionParameters().size(); ++i)
  {
    const auto& parameter = mStorage->getParameter(i);
    LOG_DEBUG("Adding parameter to table: {}", parameter.value.toReadable());
    const int row = mTable->rowCount();
    mTable->insertRow(row);

    mTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(parameter.name)));
    mTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(parameter.type.toString())));
    mTable->setItem(row, 2, new QTableWidgetItem(parameter.value.toReadable()));

    auto* container = new QWidget(mTable);
    auto* edit = new QPushButton(container);
    edit->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
    edit->setIcon(iconFromTheme("document-edit"));
    connect(edit, &QPushButton::clicked, this, [this, i]() { editParameter(i); });

    auto containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
    containerLayout->addWidget(edit);

    mTable->setCellWidget(row, 3, container);
  }
}

void MissionParameterWidget::addParameter()
{
  // We only set the id once
  MissionParameter parameter;
  parameter.id = koda::types::makeUuid();

  TypeValueDialog dialog(tr("Add mission parameter"), parameter, this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  auto newParam = dialog.getParameter();
  if (newParam.type.isValid())
  {
    LOG_DEBUG("Adding parameter: {}", newParam.value.toReadable());
    mStorage->addParameter(newParam);
  }

  rebuildTable();
  emit parametersChanged();
}

void MissionParameterWidget::editParameter(int index)
{
  if (!mStorage)
  {
    LOG_WARNING("Something went wrong when editing parameter, no database available");
    return;
  }

  if (index >= mStorage->missionParameters().size())
    return;

  const auto& parameter = mStorage->getParameter(index);

  TypeValueDialog dialog(tr("Edit ") + QString::fromStdString(parameter.name), parameter, this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  auto newParam = dialog.getParameter();
  if (newParam.type.isValid())
    mStorage->setParameter(index, newParam);
  else
    mStorage->removeParameter(parameter);

  rebuildTable();
  emit parametersChanged();
}

void MissionParameterWidget::removeParameter()
{
  if (!mStorage)
  {
    LOG_WARNING("Something went wrong when removing parameter, no database available");
    return;
  }

  const auto selectedRows = mTable->selectionModel()->selectedRows();
  if (selectedRows.empty())
    return;

  std::vector<int> rows;
  rows.reserve(selectedRows.size());

  for (const auto& index : selectedRows)
    rows.push_back(index.row());

  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows)
  {
    if (row < 0 || row >= mStorage->missionParameters().size())
      continue;

    mStorage->removeParameter(*(mStorage->missionParameters().begin() + row));
    mTable->removeRow(row);
  }

  emit parametersChanged();
}

}  // namespace maki