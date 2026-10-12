
#include "capability_call_selector.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include "app_configs.h"
#include "document/node_info.h"
#include "document/save_info.h"
#include "elements/node.h"
#include "keys.h"
#include "system/config_table.h"
#include "system/library_container.h"
#include "system/library_scene.h"
#include "widgets/frame.h"
#include "widgets/section.h"
#include "widgets/widget_factory.h"

// ============================================================================
// CapabilityCallSelector
// ============================================================================

CapabilityCallSelector* CapabilityCallSelector::create(NodeItem* node, QWidget* parent)
{
  return new CapabilityCallSelector(node, parent);
}

CapabilityCallSelector::CapabilityCallSelector(NodeItem* node, QWidget* parent)
    : maki::ControlWidget(parent)
    , mNode(node)
{
  auto* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  vlayout->setSpacing(0);

  setMinimumWidth(400);
  setMinimumHeight(400);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  // All steps live in the same stacked widget.
  mStack = new QStackedWidget(this);
  vlayout->addWidget(mStack);

  // --------------------------------------------------------------------------
  // Page 0: Capability selection
  // --------------------------------------------------------------------------
  {
    auto* capabilityPage = new QWidget(mStack);
    auto* capabilityLayout = new QVBoxLayout(capabilityPage);
    capabilityLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
    capabilityLayout->setSpacing(theme.spacing);

    auto* search = new maki::SearchWidget(tr("Filter capabilities"), capabilityPage);
    connect(search, &maki::SearchWidget::valueChanged, this, [this](const QString& query) {
      // Search only the libraries on the capability page.
      for (const auto& section : mStack->widget(0)->findChildren<SectionWidget*>())
        if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
          section->setExpanded(library->filterNodes(query));
    });

    capabilityLayout->addWidget(search);

    mContent = new StyledFrame();
    mContent->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    mContent->setBorderRole(StyledFrame::BorderRole::Mid);
    mContent->setRadius(theme.borderRadius);
    mContent->setBorderWidth(theme.borderWidth);

    auto* contentLayout = new QVBoxLayout(mContent);
    contentLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);

    auto* scrollArea = new QScrollArea(capabilityPage);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(mContent);

    capabilityLayout->addWidget(scrollArea);

    mStack->addWidget(capabilityPage);
  }
  // --------------------------------------------------------------------------
  // Page 1: Event selection
  // --------------------------------------------------------------------------
  {
    auto* eventPage = new QWidget(mStack);
    auto* eventLayout = new QVBoxLayout(eventPage);

    eventLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
    eventLayout->setSpacing(theme.spacing);

    auto* group = new maki::WidgetGroup(tr("Select event"), oclero::qlementine::TextRole::H3, eventPage);
    eventLayout->addWidget(group);

    mCapabilityLabel = new oclero::qlementine::Label(eventPage);
    mCapabilityLabel->setRole(oclero::qlementine::TextRole::H4);
    mCapabilityLabel->setWordWrap(true);
    group->addWidget(mCapabilityLabel);

    mEventCombo = new maki::SelectorWidget(tr("Event"), maki::WidgetAlignment::Vertical(), eventPage);
    mEventCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    group->addWidget(mEventCombo);

    auto* description = new oclero::qlementine::Label(tr("Description"), eventPage);
    description->setRole(oclero::qlementine::TextRole::Default);
    group->addWidget(description);

    mEventInfo = new QTextBrowser(eventPage);
    mEventInfo->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    mEventInfo->setFont(theme.fontRegular);
    mEventInfo->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    mEventInfo->setReadOnly(true);
    mEventInfo->setContextMenuPolicy(Qt::NoContextMenu);
    mEventInfo->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    group->addWidget(mEventInfo);

    eventLayout->addStretch();

    auto* buttons = new QHBoxLayout();
    auto* backButton = new QPushButton(tr("Back"), eventPage);
    backButton->setIcon(QIcon(":/icons/reset.svg"));

    mConfirmButton = new QPushButton(tr("Confirm"), eventPage);
    mConfirmButton->setIcon(QIcon(":/icons/accept.svg"));
    mConfirmButton->setEnabled(false);

    buttons->addWidget(backButton);
    buttons->addStretch();
    buttons->addWidget(mConfirmButton);

    eventLayout->addLayout(buttons);

    connect(backButton, &QPushButton::clicked, this, &CapabilityCallSelector::showCapabilityPage);
    connect(mConfirmButton, &QPushButton::clicked, this, &CapabilityCallSelector::confirmSelection);
    connect(mEventCombo, &maki::SelectorWidget::dataChanged, this, [this](const QString& text, const QVariant& data) {
      if (auto typeConfig = mSelectedCapability ? ConfigurationTable::instance().get(mSelectedCapability->getnodeId()) : nullptr)
        for (const auto& event : typeConfig->events)
          if (event.name == text)
            mEventInfo->setHtml(createInformationMessage(event));

      mConfirmButton->setEnabled(!text.isEmpty());
    });

    mStack->addWidget(eventPage);
  }
  showCapabilityPage();
}

void CapabilityCallSelector::start()
{
  showCapabilityPage();
  if (auto* search = mStack->widget(0)->findChild<maki::SearchWidget*>())
    search->widget()->setFocus();
}

void CapabilityCallSelector::buildLibraries(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  for (const auto& node : nodes)
  {
    if (!node)
      continue;

    LOG_DEBUG("Adding {} ({}) to library selection", node->getnodeId(), node->getid());
    const auto nodeConfig = ConfigurationTable::instance().get(node->getnodeId());
    if (!nodeConfig)
      continue;

    if (nodeConfig->libraryType != Types::LibraryTypes::STRUCTURAL)
      continue;

    if (node->getnodeId() == ConfigKeys::TASK_NODE)
      continue;

    const auto libId = libraryFromId(node->getnodeId());
    LibraryContainer* library = getLibrary(libId);
    library->addNode(node->getnodeId(), nodeConfig, node->getid());
  }

  qobject_cast<QVBoxLayout*>(mContent->layout())->addStretch();
}

void CapabilityCallSelector::selectCapability(const QString& instanceId, const QString& capabilityType)
{
  const auto cap = ConfigurationTable::instance().get(capabilityType);
  if (!cap)
  {
    LOG_DEBUG("Could not find configuration for type: {}", capabilityType);
    return;
  }

  const auto info = SaveInfo::instance().getNodeWithId(instanceId);
  if (!info)
  {
    LOG_DEBUG("Could not find node with id: {}", instanceId);
    return;
  }

  mEventCombo->widget()->clear();
  for (const auto& event : SaveInfo::instance().getEventsOfTypeFromNode(instanceId, {Types::CallType::TRIGGER, Types::CallType::ABORT}))
    if (!event->getid().isEmpty())
      mEventCombo->addItem(event->getname(), event->getid());

  const auto enoughEvents = mEventCombo->widget()->count() > 0;
  // An empty event list should not allow confirmation.
  mConfirmButton->setEnabled(enoughEvents);
  mStack->setCurrentIndex(1);

  mSelectedCapability = info;
  mCapabilityLabel->setText(nameFromNodeId(capabilityType));

  if (enoughEvents)
    mEventCombo->setFocus();
}

void CapabilityCallSelector::confirmSelection()
{
  if (!mNode)
  {
    LOG_DEBUG("No node available");
    return;
  }

  if (!mSelectedCapability)
  {
    LOG_DEBUG("No capability selected");
    return;
  }

  const QString eventName = mEventCombo->getValue();
  const QString eventId = mEventCombo->getData().toString();
  if (eventName.isEmpty() || eventId.isEmpty())
  {
    LOG_DEBUG("Selected event has empty identifier");
    return;
  }

  const auto* current = mNode->getProperty(ConfigKeys::CAPABILITY);
  auto valueRecord = maki::parameterRecord(current);
  setRecordValue(valueRecord, ConfigKeys::COMPONENT, *maki::asValue(mSelectedCapability->getProperty(ConfigKeys::NAME)->getvalue()));
  setRecordValue(valueRecord, ConfigKeys::ARGUMENTS, maki::Value::createList({}));
  setRecordValue(valueRecord, ConfigKeys::EVENT, maki::Value::createString(eventName));
  setRecordValue(valueRecord, ConfigKeys::COMPONENT_ID, maki::Value::createString(mSelectedCapability->getid()));
  setRecordValue(valueRecord, ConfigKeys::COMPONENT_TYPE, maki::Value::createString(mSelectedCapability->getnodeId()));
  setRecordValue(valueRecord, ConfigKeys::CALL_ID, maki::Value::createString(eventId));
  mNode->setProperty(ConfigKeys::CAPABILITY, maki::Value::createRecord(valueRecord));

  emit dismissed();
}

void CapabilityCallSelector::showCapabilityPage()
{
  if (mEventInfo)
    mEventInfo->clear();

  mSelectedCapability.reset();

  if (mStack)
    mStack->setCurrentIndex(0);
}

QString CapabilityCallSelector::libraryFromId(const QString& id) const
{
  return id.left(id.indexOf("::"));
}

LibraryContainer* CapabilityCallSelector::getLibrary(const QString& id)
{
  for (const auto& lib : mContent->findChildren<LibraryContainer*>())
    if (lib->property(ConfigKeys::ID.toUtf8()).toString() == id)
      return lib;

  auto* library = LibraryContainer::create(id, mContent);
  library->setProperty(ConfigKeys::ID.toUtf8(), id);
  library->setColumnCount(4);
  library->setDraggable(false);

  if (auto* scene = qobject_cast<LibraryScene*>(library->scene()))
    connect(scene, &LibraryScene::libraryNodeSelected, this,
            [this](const QString& nodeType, const QString& instanceId) { selectCapability(instanceId, nodeType); });

  return library;
}

void CapabilityCallSelector::wheelEvent(QWheelEvent* event)
{
  QWidget::wheelEvent(event);
  event->accept();
}
