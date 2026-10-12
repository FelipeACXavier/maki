#include "capability_call_selector.h"

#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "document/node_info.h"
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
CapabilityCallSelector* CapabilityCallSelector::create(QWidget* parent)
{
  auto* menu = new CapabilityCallSelector(parent);
  return menu;
}

CapabilityCallSelector::CapabilityCallSelector(QWidget* parent)
    : maki::ControlWidget(parent)
{
  QVBoxLayout* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  vlayout->setSpacing(0);
  setMinimumWidth(400);
  setMinimumHeight(400);

  auto search = new maki::SearchWidget(tr("Filter capabilities"), this);
  connect(search, &maki::SearchWidget::valueChanged, this, [this](const QString& query) {
    for (const auto& section : findChildren<SectionWidget*>())
      if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
        section->setExpanded(query.isEmpty() || library->filterNodes(query));
  });

  vlayout->addWidget(search);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  const auto theme = qlementineStyle->theme();

  auto* container = new QWidget(this);
  auto* layout = new QVBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(theme.spacing);

  mContent = new StyledFrame();
  mContent->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  mContent->setBorderRole(StyledFrame::BorderRole::Mid);
  mContent->setRadius(theme.borderRadius);
  mContent->setBorderWidth(theme.borderWidth);

  QVBoxLayout* contentLayout = new QVBoxLayout(mContent);
  contentLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);

  auto scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidget(mContent);

  layout->addWidget(scrollArea);
  vlayout->addWidget(container);
}

void CapabilityCallSelector::start()
{
  if (auto* search = findChild<maki::SearchWidget*>())
    search->widget()->setFocus();
}

void CapabilityCallSelector::buildLibraries(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  for (const auto& node : nodes)
  {
    const auto nodeConfig = ConfigurationTable::instance().get(node->getnodeId());
    if (!nodeConfig)
      continue;

    if (nodeConfig->libraryType != Types::LibraryTypes::STRUCTURAL)
      continue;

    if (node->getnodeId() == ConfigKeys::TASK_NODE)
      continue;

    auto libId = libraryFromId(node->getnodeId());
    LibraryContainer* library = getLibrary(libId);
    library->addNode(node->getnodeId(), nodeConfig);
  }

  qobject_cast<QVBoxLayout*>(mContent->layout())->addStretch();
}

QString CapabilityCallSelector::libraryFromId(const QString& id) const
{
  return id.left(id.indexOf("::"));
}

LibraryContainer* CapabilityCallSelector::getLibrary(const QString& id)
{
  for (const auto& lib : findChildren<LibraryContainer*>())
    if (lib->property(ConfigKeys::ID.toUtf8()).toString() == id)
      return lib;

  auto library = LibraryContainer::create(id, mContent);
  library->setProperty(ConfigKeys::ID.toUtf8(), id);
  library->setColumnCount(4);
  library->setDraggable(false);
  if (auto* scene = qobject_cast<LibraryScene*>(library->scene()))
    connect(scene, &LibraryScene::libraryNodeSelected, [this](const QString& nodeType) {
      emit accepted({nodeType});
      emit dismissed();
    });

  return library;
}

void CapabilityCallSelector::wheelEvent(QWheelEvent* event)
{
  QWidget::wheelEvent(event);
  event->accept();
}
