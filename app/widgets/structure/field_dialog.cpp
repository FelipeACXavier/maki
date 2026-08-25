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
#include "logging.h"
#include "save_info.h"
#include "style_helpers.h"
#include "widgets/widget_factory.h"

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

std::shared_ptr<PropertyInfo> FieldDialog::getInfo() const
{
  return mStorage;
}

void FieldDialog::setup(std::shared_ptr<PropertyInfo> config)
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

  qobject_cast<QVBoxLayout*>(layout())->addStretch();

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
  auto name = new maki::StringWidget(tr("Field name"), mStorage->getid(), {maki::WidgetAlignment::Type::VERTICAL}, this);
  connect(name, &maki::StringWidget::valueChanged, this, [this](const QString& text) { mStorage->setId(text); });
  layout()->addWidget(name);
}

void FieldDialog::createTypeInput(QWidget* parent)
{
  auto typeBox = new maki::TypeSelectionWidget(Types::PropertyTypesToString(Types::PropertyTypes::REAL), Types::minus(Types::PropertyTypes::VOID, 1), this);

  auto type = new maki::SelectorWidget(tr("Return type"), typeBox, maki::WidgetAlignment::Vertical(), this);
  type->setObjectName("TypeField");

  connect(type, &maki::SelectorWidget::valueChanged, this, [this](const QString& text) {
    auto fieldType = Types::StringToPropertyTypes(text);
    mStorage->setType(fieldType);
  });

  if (mStorage->gettype() == Types::PropertyTypes::UNKNOWN)
  {
    mStorage->setType(Types::StringToPropertyTypes(type->getValue()));
  }
  else
  {
    type->setValue(Types::PropertyTypesToString(mStorage->gettype()));
  }

  layout()->addWidget(type);
}

void FieldDialog::createArgumentInput(QWidget* parent)
{
  // TODO(felaze): Make this field based on the type
  QLabel* nameLabel = new QLabel(tr("Default value"), parent);
  layout()->addWidget(nameLabel);

  QLineEdit* name = new QLineEdit(parent);
  name->setText(mStorage->getdefaultValue().toString());
  name->setFocusPolicy(Qt::ClickFocus);

  connect(name, &QLineEdit::editingFinished, this, [this, name]() {
    LOG_DEBUG("Setting default to {}", name->text());
    mStorage->setDefaultValue(name->text());
  });
  layout()->addWidget(name);
}

// void FieldDialog::updateArgumentTable(QStandardItem* item)
// {
//   if (!item)
//     return;

//   int row = item->row();
//   if (row >= mStorage->arguments.size())
//   {
//     LOG_WARNING("Tried to modify argument that does not exist");
//     return;
//   }

//   int column = item->column();
//   auto text = item->text();
//   if (text.isNull() || text.isEmpty())
//     return;

//   if (column == 0)
//     mStorage->arguments[row].id = text;
//   else if (column == 1)
//     mStorage->arguments[row].type = Types::StringToPropertyTypes(text);
// }

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

// void FieldDialog::addEnumField(QWidget* parent)
// {
//   int index = layout()->count();
//   for (int i = 0; i < layout()->count(); ++i)
//   {
//     QLayoutItem* item = layout()->itemAt(i);
//     QWidget* widget = item->widget();
//     if (widget && widget->objectName() == "TypeField")
//     {
//       index = i;
//       break;
//     }
//   }

//   QGroupBox* group = new QGroupBox("Settings", parent);
//   QVBoxLayout* groupLayout = new QVBoxLayout(group);
//   group->setObjectName("EnumGroup");
//   group->setTitle("Enum options");

//   QLabel* enumNameLabel = new QLabel(tr("Enum name"), group);
//   QLineEdit* name = new QLineEdit(parent);
//   name->setFocusPolicy(Qt::StrongFocus);

//   QLabel* enumTypeLabel = new QLabel(tr("Enum values"), group);

//   QListWidget* listWidget = new QListWidget(group);

//   QLineEdit* input = new QLineEdit(group);
//   input->setFocusPolicy(Qt::StrongFocus);

//   PropertyInfo foundName;
//   PropertyInfo foundValues;
//   for (const auto& opt : mStorage->getoptions())
//   {
//     if (opt->getid() == "name")
//       foundName = *std::dynamic_pointer_cast<PropertyInfo>(opt);
//     if (opt->getid() == "values")
//       foundValues = *std::dynamic_pointer_cast<PropertyInfo>(opt);
//   }

//   if (foundName.getid().isEmpty())
//   {
//     PropertyInfo enumName;
//     enumName.setId("name");
//     enumName.setType(Types::PropertyTypes::STRING);
//     mStorage->addOption(std::make_shared<PropertyInfo>(enumName));
//   }
//   else
//   {
//     name->setText(foundName.getdefaultValue().toString());
//   }

//   if (foundValues.getoptions().isEmpty())
//   {
//     PropertyInfo enumValues;
//     enumValues.setId("values");
//     enumValues.setType(Types::PropertyTypes::LIST);
//     mStorage->addOption(std::make_shared<PropertyInfo>(enumValues));
//   }
//   else
//   {
//     for (const auto& opt : foundValues.getoptions())
//       listWidget->addItem(opt->getid());
//   }

//   connect(name, &QLineEdit::editingFinished, this, [=]() {
//     for (auto& opt : mStorage->getoptions())
//     {
//       if (opt->getid() != "name")
//         continue;

//       std::dynamic_pointer_cast<PropertyInfo>(opt)->setDefaultValue(name->text());
//       break;
//     }
//   });

//   connect(input, &QLineEdit::returnPressed, this, [=]() {
//     if (!input->text().isEmpty())
//     {
//       for (auto& opt : mStorage->getoptions())
//       {
//         if (opt->getid() == "values")
//         {
//           PropertyInfo option;
//           option.setId(input->text());

//           std::dynamic_pointer_cast<PropertyInfo>(opt)->addOption(std::make_shared<PropertyInfo>(option));
//           break;
//         }
//       }

//       listWidget->addItem(input->text());
//       input->clear();
//       input->focusWidget();
//     }
//   });

//   groupLayout->addWidget(enumNameLabel);
//   groupLayout->addWidget(name);
//   groupLayout->addWidget(enumTypeLabel);
//   groupLayout->addWidget(input);
//   groupLayout->addWidget(listWidget);

//   static_cast<QVBoxLayout*>(layout())->insertWidget(++index, group);
// }

// void FieldDialog::removeEnumField()
// {
//   for (int i = 0; i < layout()->count(); ++i)
//   {
//     QLayoutItem* item = layout()->itemAt(i);
//     QWidget* widget = item->widget();
//     if (widget && widget->objectName() == "EnumGroup")
//     {
//       layout()->removeWidget(widget);
//       delete widget;
//       break;
//     }
//   }
// }
