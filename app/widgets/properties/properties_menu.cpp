#include "properties_menu.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "app_configs.h"
#include "elements/node.h"
#include "keys.h"
#include "logging.h"
#include "style_helpers.h"

PropertiesMenu::PropertiesMenu(QWidget* parent)
    : QFrame(parent)
    , mCurrentNode("")
{
  // Set widget layout
  QVBoxLayout* layout = new QVBoxLayout();
  setLayout(layout);
}

VoidResult PropertiesMenu::start(std::shared_ptr<SaveInfo> storage)
{
  mStorage = storage;
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeAdded(NodeItem* /* node */)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeRemoved(NodeItem* node)
{
  if (!node)
    return VoidResult();

  // Clear the frame
  if (node->id() != mCurrentNode)
    return VoidResult();

  clear();
  mCurrentNode.clear();

  return VoidResult();
}

VoidResult PropertiesMenu::onNodeModified(NodeItem* node)
{
  return VoidResult();
}

VoidResult PropertiesMenu::onNodeSelected(NodeItem* node, bool selected)
{
  // Clear the frame
  clear();

  if (!node)
    return VoidResult();

  mCurrentNode = node->id();

  RETURN_ON_FAILURE(loadProperties(node));

  static_cast<QVBoxLayout*>(layout())->addStretch();

  return VoidResult();
}

void PropertiesMenu::clear()
{
  if (!layout())
    return;

  // Remove and delete all child widgets
  while (QLayoutItem* item = layout()->takeAt(0))
  {
    if (QWidget* widget = item->widget())
      widget->deleteLater();

    delete item;
  }
}

VoidResult PropertiesMenu::loadProperties(NodeItem* node)
{
  if (node->configurationProperties().size() < 1)
  {
    QLabel* nameLabel = new QLabel("Block has no properties", this);
    nameLabel->setFont(Fonts::Property);

    layout()->setAlignment(Qt::AlignCenter);
    layout()->addWidget(nameLabel);

    return VoidResult();
  }

  for (const auto& property : node->configurationProperties())
  {
    // LOG_DEBUG("Updating properties with %s of type %d", qPrintable(property.id), (int)property.type);

    QString label = ToLabel(property.id);
    QLabel* nameLabel = new QLabel(label);

    nameLabel->setFont(Fonts::Label);
    layout()->addWidget(nameLabel);

    if (property.type == Types::PropertyTypes::STRING)
      LOG_WARN_ON_FAILURE(loadPropertyString(property, node));
    else if (property.type == Types::PropertyTypes::INTEGER)
      LOG_WARN_ON_FAILURE(loadPropertyInt(property, node));
    else if (property.type == Types::PropertyTypes::REAL)
      LOG_WARN_ON_FAILURE(loadPropertyReal(property, node));
    else if (property.type == Types::PropertyTypes::BOOLEAN)
      LOG_WARN_ON_FAILURE(loadPropertyBoolean(property, node));
    else if (property.type == Types::PropertyTypes::SELECT)
      LOG_WARN_ON_FAILURE(loadPropertySelect(property, node));
    else if (property.type == Types::PropertyTypes::COLOR)
      LOG_WARN_ON_FAILURE(loadPropertyColor(property, node));
    else if (property.type == Types::PropertyTypes::EVENT_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyEventSelect(property, node));
    else if (property.type == Types::PropertyTypes::COMPONENT_SELECT)
      LOG_WARN_ON_FAILURE(loadPropertyComponentSelect(property, node));
    else
      LOG_WARNING("Property without a type, how is that possible?");
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyInt(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  QIntValidator* validator = new QIntValidator(INT32_MIN, INT32_MIN, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  widget->setValidator(validator);

  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    bool ok;
    int newValue = widget->text().toInt(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyReal(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  QDoubleValidator* validator = new QDoubleValidator(DBL_MIN, DBL_MAX, 6, widget);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  widget->setValidator(validator);

  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    bool ok;
    qreal newValue = widget->text().toDouble(&ok);
    if (ok)
      node->setProperty(property.id, newValue);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyColor(const PropertiesConfig& property, NodeItem* node)
{
  QWidget* holder = new QWidget(this);
  QHBoxLayout* holderLayout = new QHBoxLayout(holder);
  holder->setLayout(holderLayout);

  QPushButton* widget = new QPushButton(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  QColor selectedColor = QColor::fromString(result.toString());
  QPalette palette;
  palette.setColor(QPalette::Window, selectedColor);

  QLabel* colorPreviewLabel = new QLabel(this);
  colorPreviewLabel->setAutoFillBackground(true);
  colorPreviewLabel->setPalette(palette);

  connect(widget, &QPushButton::pressed, [=]() {
    QColor color = QColorDialog::getColor(selectedColor, this, "Background Color");
    QPalette newPalette;
    newPalette.setColor(QPalette::Window, color);
    colorPreviewLabel->setPalette(newPalette);

    node->config()->body.backgroundColor = color;
    node->setProperty(property.id, color.name());
  });

  widget->setText(result.toString());
  widget->setFont(Fonts::Property);

  holderLayout->addWidget(colorPreviewLabel);
  holderLayout->addWidget(widget);

  layout()->addWidget(holder);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertySelect(const PropertiesConfig& property, NodeItem* node)
{
  QComboBox* widget = new QComboBox(this);

  auto options = property.options;
  for (const auto& option : options)
    widget->addItem(option.id);

  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setCurrentText(result.toString());
  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.id, text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyString(const PropertiesConfig& property, NodeItem* node)
{
  QLineEdit* widget = new QLineEdit(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setText(result.toString());
  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    node->setProperty(property.id, widget->text());
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);
  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node)
{
  QCheckBox* widget = new QCheckBox(this);
  auto result = node->getProperty(property.id);
  if (!result.isValid())
    return VoidResult::Failed("Failed to get default value");

  widget->setChecked(result.toBool());
  connect(widget, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
    node->setProperty(property.id, state);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

// TODO(felaze): The component and the event fields are always dependent on each other for now
VoidResult PropertiesMenu::loadPropertyComponentSelect(const PropertiesConfig& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.id);

  for (const auto& child : mStorage->getPossibleCallers(node->id()))
  {
    auto name = child->properties[ConfigKeys::NAME];
    if (name.isNull() || !name.isValid())
      continue;

    widget->addItem(name.toString(), child->id);
  }

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  if (property.options.empty())
  {
    // Make sure the widget shows the current selected component if it exists
    auto currentValue = node->getProperty(property.id);
    if (currentValue.isValid())
      widget->setCurrentText(currentValue.toString());
    else
      widget->setCurrentText("-");

    connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
      node->setProperty(property.id, text);
    });
  }
  else
  {
    if (property.options.at(0).type == Types::PropertyTypes::EVENT_SELECT)
    {
      // auto option = property.options.at(0);
      // QComboBox* eventWidget = new QComboBox(this);
      // eventWidget->setObjectName(option.id);

      // Set starting values
      auto value = node->getProperty(property.id);
      if (value.isValid())
      {
        QJsonObject object = value.toJsonObject();
        qDebug() << "Starting with " << object;
        widget->setCurrentText(object[ConfigKeys::DATA].toString());

        // auto events = mStorage->getEventsFromNode(widget->currentData().toString());
        // for (const auto& event : events)
        //   eventWidget->addItem(event->name, event->id);

        // eventWidget->setCurrentText(object[ConfigKeys::OPTION_DATA].toString());
      }
      else
      {
        widget->setCurrentText("-");
        // eventWidget->setCurrentText("-");
      }

      // connect(eventWidget, &QComboBox::currentTextChanged, this, [node, property](const QString& text) {
      //   if (text.isEmpty())
      //     return;

      //   auto value = node->getProperty(property.id);
      //   if (!value.isValid())
      //     return;

      //   QJsonObject object = value.toJsonObject();
      //   object[ConfigKeys::OPTION_DATA] = text;

      //   qDebug() << "Setting event " << object;
      //   node->setProperty(property.id, object);
      // });

      // eventWidget->setFont(Fonts::Property);
      // layout()->addWidget(eventWidget);

      connect(widget, &QComboBox::currentTextChanged, this, [this, widget, node, property](const QString& text) {
        if (text.isEmpty())
          return;

        // eventWidget->clear();
        // auto events = mStorage->getEventsFromNode(widget->currentData().toString());
        // for (const auto& event : events)
        //   eventWidget->addItem(event->name, event->id);

        // Set the component
        auto value = node->getProperty(property.id);
        if (!value.isValid())
          return;

        QJsonObject object = value.toJsonObject();
        object[ConfigKeys::DATA] = text;
        // object[ConfigKeys::OPTION_DATA] = events.size() > 0 ? events.at(0)->name : "";

        qDebug() << "Setting component " << object;
        node->setProperty(property.id, object);
      });
    }
    else
    {
      LOG_WARNING("Configuration is not supported");
    }
  }

  return VoidResult();
}

VoidResult PropertiesMenu::loadPropertyEventSelect(const PropertiesConfig& property, NodeItem* node)
{
  if (!mStorage)
    return VoidResult::Failed("No storage assigned to properties menu");

  QComboBox* widget = new QComboBox(this);
  widget->setObjectName(property.id);

  // Make sure the widget shows the current selected component if it exists
  auto currentValue = node->getProperty(property.id);
  if (currentValue.isValid())
    widget->setCurrentText(currentValue.toString());
  else
    widget->setCurrentText("-");

  connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    node->setProperty(property.id, text);
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  return VoidResult();
}

VoidResult PropertiesMenu::onTransitionSelected(TransitionItem* transition)
{
  // Clear the frame
  clear();

  if (!transition)
    return VoidResult();

  mCurrentNode = transition->id();

  QLabel* nameLabel = new QLabel("Condition");

  nameLabel->setFont(Fonts::Label);
  layout()->addWidget(nameLabel);

  QLineEdit* widget = new QLineEdit(this);
  widget->setText(transition->getName());
  connect(widget, &QLineEdit::returnPressed, this, [=]() {
    transition->setName(widget->text());
  });

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  return VoidResult();
}
