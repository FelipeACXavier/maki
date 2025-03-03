#include "main_window.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleValidator>
#include <QDrag>
#include <QIntValidator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QMimeData>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <cfloat>

#include "app_configs.h"
#include "canvas.h"
#include "library_container.h"
#include "logging.h"
#include "node.h"
#include "ui_editor.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
  mUI->setupUi(this);
  connect(mUI->actionGenerate, &QAction::triggered, this,
          &MainWindow::onActionGenerate);
}

MainWindow::~MainWindow()
{
}

VoidResult MainWindow::start()
{
  LOG_INFO("Starting the main window");

  auto configRead = JSON::fromFile(":/assets/config.json");
  if (!configRead.IsSuccess())
    return VoidResult::Failed(configRead);

  mConfig = configRead.Value();
  mConfigTable = std::make_unique<ConfigurationTable>();

  Canvas* canvas = new Canvas(mConfigTable, mUI->graphicsView);
  connect(canvas, &Canvas::nodeSelected, this, &MainWindow::onNodeSelected);
  mUI->graphicsView->setScene(canvas);

  mGenerator = std::make_shared<Generator>();

  RETURN_ON_FAILURE(loadElements());

  LOG_INFO("Main window started");

  return VoidResult();
}

VoidResult MainWindow::loadElements()
{
  LOG_INFO("Loading the elements");

  if (!mConfig.contains("libraries"))
    return VoidResult::Failed("Not a single library was specified");

  auto libraries = mConfig["libraries"];
  if (!libraries.isArray())
    return VoidResult::Failed(
        "Libraries must be in a list in the format \"libraries\": []");

  mUI->splitter->widget(0)->setMinimumWidth(200);
  mUI->splitter->widget(0)->setMaximumWidth(400);
  mUI->splitter->widget(0)->setFixedWidth(200);

  mUI->splitter->widget(2)->setMinimumWidth(200);
  mUI->splitter->widget(2)->setMaximumWidth(600);
  mUI->splitter->widget(2)->setFixedWidth(400);

  for (const auto& library : libraries.toArray())
  {
    QString fileName = QStringLiteral(":/libraries/%1.json").arg(library.toString());

    auto libRead = JSON::fromFile(fileName);
    if (!libRead.IsSuccess())
      return VoidResult::Failed(
          QStringLiteral("Failed to open configuration: %1")
              .arg(fileName)
              .toStdString());

    auto libConfig = libRead.Value();

    RETURN_ON_FAILURE(loadElementLibrary(libConfig));
  }

  return VoidResult();
}

// TODO(felaze): Get this from the configuration file and convert to function
VoidResult MainWindow::loadElementLibrary(const JSON& config)
{
  if (!config.contains("name"))
    return VoidResult::Failed("Libraries must have a name");

  if (!config.contains("type"))
    return VoidResult::Failed("Libraries must have a type");

  QString name = config["name"].toString();
  QString type = config["type"].toString();

  LOG_DEBUG("Loading library: %s", qPrintable(name));

  // Every library is added to a new item in the toolbox.
  // We load those dynamically on startup.
  LibraryContainer* sidebarview = LibraryContainer::create(name, type == "behaviour" ? mUI->behaviourToolbox : mUI->structureToolbox);

  auto nodes = config["nodes"];
  if (!nodes.isArray())
    return VoidResult::Failed(
        "nodes must be in a list in the format \"nodes\": []");

  // Every library has a bunch of elements, here we add them.
  for (const auto& value : nodes.toArray())
  {
    if (!value.isObject())
      return VoidResult::Failed("Invalid node format");

    QJsonObject node = value.toObject();
    if (!node.contains("name"))
      return VoidResult::Failed("Nodes must contain a name");

    // Parse config and make sure it is valid before continuing
    auto config = std::make_shared<NodeConfig>(node);
    if (!config->isValid())
      return config->result();

    auto id = QStringLiteral("%1::%2").arg(name, config->name);
    sidebarview->addNode(id, config);

    RETURN_ON_FAILURE(mConfigTable->add(id, config));
  }

  return VoidResult();
}

void MainWindow::onActionGenerate()
{
  if (!mGenerator)
    LOG_WARNING("No generator available");

  auto canvas = static_cast<Canvas*>(mUI->graphicsView->scene());
  auto ret = mGenerator->generate(canvas);
  if (!ret.IsSuccess())
    LOG_ERROR("Generation failed: %s", ret.ErrorMessage().c_str());
  else
    LOG_INFO("Generation complete");
}

void MainWindow::onNodeSelected(NodeItem* node)
{
  if (!node)
  {
    LOG_WARNING("A node was selected but no node was provided");
    return;
  }

  // LOG_DEBUG("Info message: %s", qPrintable(node->help().message));
  mUI->infoText->setText(node->help().message);
  mUI->infoText->setWordWrapMode(QTextOption::WrapMode::WordWrap);

  // Clear the frame
  QVBoxLayout* layout =
      qobject_cast<QVBoxLayout*>(mUI->propertiesFrame->layout());
  if (layout)
  {
    // Remove and delete all child widgets
    while (QLayoutItem* item = layout->takeAt(0))
    {
      if (QWidget* widget = item->widget())
        widget->deleteLater();

      delete item;
    }
  }
  else
  {
    // Ensure there is a layout
    layout = new QVBoxLayout(mUI->propertiesFrame);
    mUI->propertiesFrame->setLayout(layout);
  }

  // Add new widgets
  for (const auto& property : node->properties())
  {
    LOG_DEBUG("Updating properties with %s of type %d", qPrintable(property.id), (int)property.type);

    QLabel* nameLabel = new QLabel(property.id);
    layout->addWidget(nameLabel);

    if (property.type == Types::PropertyTypes::STRING)
    {
      QLineEdit* widget = new QLineEdit(mUI->propertiesFrame);
      auto result = node->getProperty(property.id);
      if (!result.IsSuccess())
      {
        LOG_WARNING("Failed to get default value: %s", result.ErrorMessage().c_str());
        continue;
      }

      widget->setText(result.Value().toString());
      connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
        node->setProperty(property.id, text);
      });

      layout->addWidget(widget);
    }
    else if (property.type == Types::PropertyTypes::INTEGER)
    {
      QLineEdit* widget = new QLineEdit(mUI->propertiesFrame);
      QIntValidator* validator = new QIntValidator(INT32_MIN, INT32_MIN, widget);

      auto result = node->getProperty(property.id);
      if (!result.IsSuccess())
      {
        LOG_WARNING("Failed to get default value: %s", result.ErrorMessage().c_str());
        continue;
      }

      widget->setText(result.Value().toString());
      widget->setValidator(validator);

      connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
        bool ok;
        int newValue = text.toInt(&ok);
        if (ok)
          node->setProperty(property.id, newValue);
      });

      layout->addWidget(widget);
    }
    else if (property.type == Types::PropertyTypes::REAL)
    {
      QLineEdit* widget = new QLineEdit(mUI->propertiesFrame);
      QDoubleValidator* validator = new QDoubleValidator(DBL_MIN, DBL_MAX, 6, widget);

      auto result = node->getProperty(property.id);
      if (!result.IsSuccess())
      {
        LOG_WARNING("Failed to get default value: %s", result.ErrorMessage().c_str());
        continue;
      }

      widget->setText(result.Value().toString());
      widget->setValidator(validator);

      connect(widget, &QLineEdit::textChanged, this, [=](const QString& text) {
        bool ok;
        qreal newValue = text.toDouble(&ok);
        if (ok)
          node->setProperty(property.id, newValue);
      });

      layout->addWidget(widget);
    }
    else if (property.type == Types::PropertyTypes::BOOLEAN)
    {
      QCheckBox* widget = new QCheckBox(mUI->propertiesFrame);
      auto result = node->getProperty(property.id);
      if (!result.IsSuccess())
      {
        LOG_WARNING("Failed to get default value: %s", result.ErrorMessage().c_str());
        continue;
      }

      widget->setChecked(result.Value().toBool());
      connect(widget, &QCheckBox::checkStateChanged, this, [=](Qt::CheckState state) {
        node->setProperty(property.id, state);
      });

      layout->addWidget(widget);
    }
    else if (property.type == Types::PropertyTypes::SELECT)
    {
      QComboBox* widget = new QComboBox(mUI->propertiesFrame);

      auto options = property.options;
      for (const auto& option : options)
        widget->addItem(option.id);

      auto result = node->getProperty(property.id);
      if (!result.IsSuccess())
      {
        LOG_WARNING("Failed to get default value: %s", result.ErrorMessage().c_str());
        continue;
      }

      LOG_INFO("Found default: %s", qPrintable(result.Value().toString()));
      widget->setCurrentText(result.Value().toString());
      connect(widget, &QComboBox::currentTextChanged, this, [=](const QString& text) {
        node->setProperty(property.id, text);
      });

      layout->addWidget(widget);
    }
    else
    {
      LOG_WARNING("Property without a type, how is that possible?");
      continue;
    }
  }

  layout->addStretch();
}
