#include "field_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/save_info.h"
#include "logging.h"
#include "style_helpers.h"

FieldDialog::FieldDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
    , mEnterCount(0)
{
  setWindowTitle(title);

  // Create layout and widgets
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);

  // TODO(felaze): Make this dynamic
  resize(500, 400);
}

PropertiesConfig FieldDialog::getInfo() const
{
  return mStorage;
}

void FieldDialog::setup(const PropertiesConfig& config)
{
  mStorage = config;

  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);
  controlLayout->setContentsMargins(10, 0, 10, 0);
  layout()->setAlignment(Qt::AlignCenter);

  controls->setLayout(controlLayout);
  layout()->addWidget(controls);

  createNameInput(controls);
  createTypeInput(controls);
  createArgumentInput(controls);

  layout()->setContentsMargins(10, 5, 10, 5);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  // Create a button box for OK and Cancel buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, controls);
  layout()->addWidget(buttonBox);

  // Disable enter
  auto okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setDefault(false);
  okButton->setAutoDefault(false);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &FieldDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &FieldDialog::reject);
}

void FieldDialog::createNameInput(QWidget* parent)
{
  QLabel* nameLabel = new QLabel(tr("Field name"), parent);
  layout()->addWidget(nameLabel);

  QLineEdit* name = new QLineEdit(parent);
  name->setText(mStorage.id);
  name->setFocusPolicy(Qt::ClickFocus);

  connect(name, &QLineEdit::editingFinished, this, [=]() { mStorage.id = name->text(); });
  layout()->addWidget(name);
}

void FieldDialog::createTypeInput(QWidget* parent)
{
  QLabel* returnTypeLabel = new QLabel(tr("Field type"), parent);
  layout()->addWidget(returnTypeLabel);

  QComboBox* returnType = new QComboBox(parent);
  returnType->setObjectName("TypeField");
  returnType->setFocusPolicy(Qt::ClickFocus);

  for (uint16_t i = (uint16_t)Types::PropertyTypes::UNKNOWN + 1; i < (uint16_t)Types::PropertyTypes::END; ++i)
    returnType->addItem(Types::PropertyTypesToString((Types::PropertyTypes)i));

  if (mStorage.type == Types::PropertyTypes::UNKNOWN)
  {
    returnType->setCurrentIndex(0);
    mStorage.type = Types::StringToPropertyTypes(returnType->currentText());
  }
  else
  {
    returnType->setCurrentText(Types::PropertyTypesToString(mStorage.type));
  }

  layout()->addWidget(returnType);

  if (mStorage.type == Types::PropertyTypes::ENUM)
    addEnumField(parent);

  connect(returnType, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    mStorage.type = Types::StringToPropertyTypes(text);
    if (mStorage.type == Types::PropertyTypes::ENUM)
      addEnumField(parent);
    else
      removeEnumField();
  });
}

void FieldDialog::createArgumentInput(QWidget* parent)
{
  // TODO(felaze): Make this field based on the type
  QLabel* nameLabel = new QLabel(tr("Default value"), parent);
  layout()->addWidget(nameLabel);

  QLineEdit* name = new QLineEdit(parent);
  name->setText(mStorage.defaultValue.toString());
  name->setFocusPolicy(Qt::ClickFocus);

  connect(name, &QLineEdit::editingFinished, this, [=]() {
    LOG_DEBUG("Setting default to %s", qPrintable(name->text()));
    mStorage.defaultValue = name->text();
  });
  layout()->addWidget(name);
}

void FieldDialog::updateArgumentTable(QStandardItem* item)
{
  // if (!item)
  //   return;

  // int row = item->row();
  // if (row >= mStorage->arguments.size())
  // {
  //   LOG_WARNING("Tried to modify argument that does not exist");
  //   return;
  // }

  // int column = item->column();
  // auto text = item->text();
  // if (text.isNull() || text.isEmpty())
  //   return;

  // if (column == 0)
  //   mStorage->arguments[row].id = text;
  // else if (column == 1)
  //   mStorage->arguments[row].type = Types::StringToPropertyTypes(text);
}

void FieldDialog::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
  {
    QWidget* first = focusWidget();
    QWidget* next = first->nextInFocusChain();

    while (next && next != first)
    {
      // TODO: magic number
      if ((next->focusPolicy() & 0x8) && next->isEnabled() && next->isVisible())
      {
        next->setFocus();
        event->accept();
        return;
      }

      next = next->nextInFocusChain();
    }
  }

  QDialog::keyPressEvent(event);
}

void FieldDialog::addEnumField(QWidget* parent)
{
  int index = layout()->count();
  for (int i = 0; i < layout()->count(); ++i)
  {
    QLayoutItem* item = layout()->itemAt(i);
    QWidget* widget = item->widget();
    if (widget && widget->objectName() == "TypeField")
    {
      index = i;
      break;
    }
  }

  QGroupBox* group = new QGroupBox("Settings", parent);
  QVBoxLayout* groupLayout = new QVBoxLayout(group);
  group->setObjectName("EnumGroup");
  group->setTitle("Enum options");

  QLabel* enumNameLabel = new QLabel(tr("Enum name"), group);
  QLineEdit* name = new QLineEdit(parent);
  name->setFocusPolicy(Qt::StrongFocus);

  QLabel* enumTypeLabel = new QLabel(tr("Enum values"), group);

  QListWidget* listWidget = new QListWidget(group);

  QLineEdit* input = new QLineEdit(group);
  input->setFocusPolicy(Qt::StrongFocus);

  PropertiesConfig foundName;
  PropertiesConfig foundValues;
  for (const auto& opt : mStorage.options)
  {
    if (opt.id == "name")
      foundName = opt;
    if (opt.id == "values")
      foundValues = opt;
  }

  if (foundName.id.isEmpty())
  {
    PropertiesConfig enumName;
    enumName.id = "name";
    enumName.type = Types::PropertyTypes::STRING;
    mStorage.options.push_back(enumName);
  }
  else
  {
    name->setText(foundName.defaultValue.toString());
  }

  if (foundValues.options.isEmpty())
  {
    PropertiesConfig enumValues;
    enumValues.id = "values";
    enumValues.type = Types::PropertyTypes::LIST;
    mStorage.options.push_back(enumValues);
  }
  else
  {
    for (const auto& opt : foundValues.options)
      listWidget->addItem(opt.id);
  }

  connect(name, &QLineEdit::editingFinished, this, [=]() {
    for (auto& opt : mStorage.options)
    {
      if (opt.id != "name")
        continue;

      opt.defaultValue = name->text();
      break;
    }
  });

  connect(input, &QLineEdit::returnPressed, this, [=]() {
    if (!input->text().isEmpty())
    {
      for (auto& opt : mStorage.options)
      {
        if (opt.id == "values")
        {
          PropertiesConfig option;
          option.id = input->text();

          opt.options.push_back(option);
          break;
        }
      }

      listWidget->addItem(input->text());
      input->clear();
      input->focusWidget();
    }
  });

  groupLayout->addWidget(enumNameLabel);
  groupLayout->addWidget(name);
  groupLayout->addWidget(enumTypeLabel);
  groupLayout->addWidget(input);
  groupLayout->addWidget(listWidget);

  static_cast<QVBoxLayout*>(layout())->insertWidget(++index, group);
}

void FieldDialog::removeEnumField()
{
  for (int i = 0; i < layout()->count(); ++i)
  {
    QLayoutItem* item = layout()->itemAt(i);
    QWidget* widget = item->widget();
    if (widget && widget->objectName() == "EnumGroup")
    {
      layout()->removeWidget(widget);
      delete widget;
      break;
    }
  }
}
