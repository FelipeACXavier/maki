#include "theme_editor.h"

#include <QBoxLayout>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QStandardPaths>
#include <QStyle>
#include <oclero/qlementine/widgets/ColorEditor.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "frame.h"
#include "logging.h"
#include "section.h"

#define ADD_TITLE(TEXT)                                        \
  {                                                            \
    formLayout->addRow(new Label(TEXT, TextRole::H3, &owner)); \
    auto* line = new QFrame(&owner);                           \
    line->setFrameShape(QFrame::HLine);                        \
    line->setFrameShadow(QFrame::Plain);                       \
    formLayout->addRow(line);                                  \
  }

#define ADD_SUBTITLE(TEXT) \
  formLayout->addRow(new Label(TEXT, TextRole::H5, &owner));

#define ADD_DESCRIPTION(TEXT) formLayout->addRow(new Label(TEXT, TextRole::Caption, &owner));

#define ADD_COLOR_EDITOR(NAME, DESCRIPTION)                                                               \
  {                                                                                                       \
    const auto pair = makeColorAndLabel(#NAME, DESCRIPTION, &owner, theme.NAME, [this](const QColor& c) { \
      theme.NAME = c;                                                                                     \
      Q_EMIT owner.themeChanged(theme);                                                                   \
    });                                                                                                   \
    this->NAME##Editor = pair.editor;                                                                     \
    formLayout->addRow(pair.left, pair.right);                                                            \
  }

#define ADD_METADATA_TEXT_EDITOR(NAME, DESCRIPTION)                                           \
  {                                                                                           \
    const auto pair = makeTextAndLabel(#NAME, DESCRIPTION, &owner, [this](const QString& s) { \
      theme.meta.NAME = s;                                                                    \
      Q_EMIT owner.themeChanged(theme);                                                       \
    });                                                                                       \
    this->NAME##Editor = pair.second;                                                         \
    formLayout->addRow(pair.first, pair.second);                                              \
  }

#define UPDATE_COLOR_EDITOR(NAME)           \
  this->NAME##Editor->blockSignals(true);   \
  this->NAME##Editor->setColor(theme.NAME); \
  this->NAME##Editor->blockSignals(false);

#define UPDATE_METADATA_TEXT_EDITOR(NAME)       \
  this->NAME##Editor->blockSignals(true);       \
  this->NAME##Editor->setText(theme.meta.NAME); \
  this->NAME##Editor->blockSignals(false);

namespace oclero::qlementine
{
const QString PREVIOUS_PATH_SETTINGS_KEY{"previousPath"};
const QString DEFAULT_FILE_NAME{"theme.json"};

struct ColorReturn
{
  QWidget* left;
  QWidget* right;
  ColorEditor* editor;
};

ColorReturn makeColorAndLabel(const QString& label, const QString& description,
                              QWidget* parent, const QColor& initialValue, const std::function<void(const QColor&)>& onChanged)
{
  auto* colorEditor = new ColorEditor(initialValue, parent);
  colorEditor->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  QObject::connect(colorEditor, &ColorEditor::colorChanged, parent, [colorEditor, onChanged]() {
    onChanged(colorEditor->color());
  });

  const auto hSpacing = parent->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);

  auto* leftColumn = new QWidget(parent);
  auto* leftColumnLayout = new QVBoxLayout(leftColumn);
  leftColumnLayout->setContentsMargins(hSpacing, 0, 0, 0);

  auto* rightColumn = new QWidget(parent);
  auto* rightLayout = new QHBoxLayout(rightColumn);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  rightLayout->addStretch();
  rightLayout->addWidget(colorEditor);

  const auto vSpacing = leftColumn->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  leftColumnLayout->setSpacing(vSpacing / 4);

  auto* nameLabel = new Label(label, TextRole::Default, leftColumn);
  nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  leftColumnLayout->addWidget(nameLabel);
  if (!description.isEmpty())
    leftColumnLayout->addWidget(new Label(description, TextRole::Caption, leftColumn));

  return {leftColumn, rightColumn, colorEditor};
}

std::pair<QWidget*, LineEdit*> makeTextAndLabel(const QString& label, const QString& description, QWidget* parent,
                                                const std::function<void(const QString&)>& onChanged)
{
  auto* lineEdit = new LineEdit(parent);
  lineEdit->setPlaceholderText(label);
  QObject::connect(lineEdit, &QLineEdit::editingFinished, parent, [lineEdit, onChanged] {
    onChanged(lineEdit->text().trimmed());
  });

  const auto hSpacing = parent->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
  auto* leftColumn = new QWidget(parent);
  auto* leftColumnLayout = new QVBoxLayout(leftColumn);
  leftColumnLayout->setContentsMargins(hSpacing, 0, 0, 0);

  const auto vSpacing = leftColumn->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  leftColumnLayout->setSpacing(vSpacing / 4);

  auto* nameLabel = new Label(label, TextRole::Default, leftColumn);
  nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  leftColumnLayout->addWidget(nameLabel);

  if (!description.isEmpty())
  {
    auto* descriptionLabel = new Label(description, TextRole::Caption, leftColumn);
    leftColumnLayout->addWidget(descriptionLabel);
  }

  return {leftColumn, lineEdit};
}

struct ThemeEditorWidget::Impl
{
  ColorEditor* primaryColorEditor{};
  ColorEditor* primaryColorHoveredEditor{};
  ColorEditor* primaryColorPressedEditor{};
  ColorEditor* primaryColorDisabledEditor{};

  ColorEditor* primaryColorForegroundEditor{};
  ColorEditor* primaryColorForegroundHoveredEditor{};
  ColorEditor* primaryColorForegroundPressedEditor{};
  ColorEditor* primaryColorForegroundDisabledEditor{};

  ColorEditor* primaryAlternativeColorEditor{};
  ColorEditor* primaryAlternativeColorHoveredEditor{};
  ColorEditor* primaryAlternativeColorPressedEditor{};
  ColorEditor* primaryAlternativeColorDisabledEditor{};

  ColorEditor* secondaryColorEditor{};
  ColorEditor* secondaryColorHoveredEditor{};
  ColorEditor* secondaryColorPressedEditor{};
  ColorEditor* secondaryColorDisabledEditor{};

  ColorEditor* secondaryColorForegroundEditor{};
  ColorEditor* secondaryColorForegroundHoveredEditor{};
  ColorEditor* secondaryColorForegroundPressedEditor{};
  ColorEditor* secondaryColorForegroundDisabledEditor{};

  ColorEditor* secondaryAlternativeColorEditor{};
  ColorEditor* secondaryAlternativeColorHoveredEditor{};
  ColorEditor* secondaryAlternativeColorPressedEditor{};
  ColorEditor* secondaryAlternativeColorDisabledEditor{};

  ColorEditor* neutralColorEditor{};
  ColorEditor* neutralColorHoveredEditor{};
  ColorEditor* neutralColorPressedEditor{};
  ColorEditor* neutralColorDisabledEditor{};

  ColorEditor* semiTransparentColor1Editor{};
  ColorEditor* semiTransparentColor2Editor{};
  ColorEditor* semiTransparentColor3Editor{};
  ColorEditor* semiTransparentColor4Editor{};

  ColorEditor* backgroundColorMain1Editor{};
  ColorEditor* backgroundColorMain2Editor{};
  ColorEditor* backgroundColorMain3Editor{};
  ColorEditor* backgroundColorMain4Editor{};

  ColorEditor* borderColorEditor{};
  ColorEditor* borderColorHoveredEditor{};
  ColorEditor* borderColorPressedEditor{};
  ColorEditor* borderColorDisabledEditor{};

  ColorEditor* focusColorEditor{};

  ColorEditor* shadowColor1Editor{};
  ColorEditor* shadowColor2Editor{};
  ColorEditor* shadowColor3Editor{};

  ColorEditor* statusColorErrorEditor{};
  ColorEditor* statusColorErrorHoveredEditor{};
  ColorEditor* statusColorErrorPressedEditor{};
  ColorEditor* statusColorErrorDisabledEditor{};

  ColorEditor* statusColorWarningEditor{};
  ColorEditor* statusColorWarningHoveredEditor{};
  ColorEditor* statusColorWarningPressedEditor{};
  ColorEditor* statusColorWarningDisabledEditor{};

  ColorEditor* statusColorSuccessEditor{};
  ColorEditor* statusColorSuccessHoveredEditor{};
  ColorEditor* statusColorSuccessPressedEditor{};
  ColorEditor* statusColorSuccessDisabledEditor{};

  ColorEditor* statusColorInfoEditor{};
  ColorEditor* statusColorInfoHoveredEditor{};
  ColorEditor* statusColorInfoPressedEditor{};
  ColorEditor* statusColorInfoDisabledEditor{};

  ColorEditor* statusColorForegroundEditor{};
  ColorEditor* statusColorForegroundHoveredEditor{};
  ColorEditor* statusColorForegroundPressedEditor{};
  ColorEditor* statusColorForegroundDisabledEditor{};

  LineEdit* nameEditor{};
  LineEdit* authorEditor{};
  LineEdit* versionEditor{};

  Impl(ThemeEditorWidget& o)
      : owner(o)
  {
  }

  /// Utility to load/save from/to JSON files.
  void setupJSONLoader(QWidget* parent, QFormLayout* formLayout)
  {
    auto* rowLayout = new QHBoxLayout();
    const auto hSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
    rowLayout->setSpacing(hSpacing * 2);

    // 'Load' button.
    {
      auto* loadJsonButton = new QPushButton(QIcon::fromTheme("document-open"), "Load JSON file…", parent);
      loadJsonButton->setToolTip("Load a JSON file from disk.");
      loadJsonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      rowLayout->addWidget(loadJsonButton);

      QObject::connect(loadJsonButton, &QPushButton::pressed, &owner, [this]() {
        // Get previous path.
        // const auto defaultDirPath =
        //     QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DocumentsLocation);
        LOG_DEBUG("Loading theme from: %s", qPrintable(defaultDirPath));
        const auto defaultPath = defaultDirPath + '/' + DEFAULT_FILE_NAME;
        QSettings settings;
        // const auto previousPath = settings.value(PREVIOUS_PATH_SETTINGS_KEY, defaultPath).toString();

        // Get theme from file and set it on the application.
        const auto fileName = getFileName("Load JSON theme");
        // QFileDialog::getOpenFileName(&owner, "Load JSON theme", defaultPath, "JSON Files (*.json)");
        const auto themeOpt = Theme::fromJsonPath(fileName);
        if (!themeOpt.has_value())
        {
          return;
        }

        owner.setTheme(themeOpt.value());

        Q_EMIT owner.themeLoaded(fileName, theme);

        // Save path to QSettings.
        // settings.setValue(PREVIOUS_PATH_SETTINGS_KEY, fileName);
        settings.sync();
      });
    }

    // 'Save' button.
    {
      auto* saveJsonButton = new QPushButton(QIcon::fromTheme("document-save"), "Save JSON file…", parent);
      saveJsonButton->setToolTip("Save the current theme as JSON file to disk.");
      saveJsonButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      rowLayout->addWidget(saveJsonButton);

      QObject::connect(saveJsonButton, &QPushButton::pressed, &owner, [this]() {
        // Get previous path.
        // const auto defaultDirPath =
        // QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DocumentsLocation);
        LOG_DEBUG("Saving theme to: %s", qPrintable(defaultDirPath));
        const auto defaultPath = defaultDirPath + '/' + DEFAULT_FILE_NAME;
        QSettings settings;
        // const auto previousPath = settings.value(PREVIOUS_PATH_SETTINGS_KEY, defaultPath).toString();

        const auto fileName = getFileName("Save JSON theme");
        // QFileDialog::getSaveFileName(&owner, "Save JSON theme", defaultPath, "JSON Files (*.json)");
        if (!fileName.isEmpty())
        {
          const auto jsonDoc = theme.toJson();
          const auto data = QByteArray(jsonDoc.toJson(QJsonDocument::JsonFormat::Indented).replace("    ", "  "));
          QFile file(fileName);
          if (file.open(QIODevice::ReadWrite))
          {
            // Save to disk.
            file.resize(0);
            file.write(data);
            file.close();

            Q_EMIT owner.themeSaved(fileName, theme);

            // Save path to QSettings.
            // settings.setValue(PREVIOUS_PATH_SETTINGS_KEY, fileName);
            settings.sync();
          }
          else
          {
            QMessageBox::critical(&owner, "Writing Error", QString("Can't write to file:\n%1").arg(fileName),
                                  QMessageBox::StandardButton::Ok);
          }
        }
      });
    }

    formLayout->addRow(rowLayout);
  }

  void setupMetadataEditors(QFormLayout* formLayout, int vSpacing)
  {
    // Metadata
    ADD_TITLE("Metadata");
    ADD_METADATA_TEXT_EDITOR(name, "Name of the Qlementine theme");
    ADD_METADATA_TEXT_EDITOR(author, "Author of the Qlementine theme");
    ADD_METADATA_TEXT_EDITOR(version, "Version of the Qlementine theme");
  }

  void setupColorEditors(QFormLayout* formLayout, int vSpacing)
  {
    addSection("Primary Color", formLayout, vSpacing);
    addSection("Secondary Color", formLayout, vSpacing);
    addSection("Neutral Color", formLayout, vSpacing);
    addSection("Semi-transparent Color", formLayout, vSpacing);
    addSection("Background Color", formLayout, vSpacing);
    addSection("Border Color", formLayout, vSpacing);
    addSection("Effect Color", formLayout, vSpacing);
    addSection("Status Color", formLayout, vSpacing);
  }

  void addSection(const QString& title, QFormLayout* parentLayout, int vSpacing)
  {
    const auto* qlementineStyle = oclero::qlementine::appStyle();
    const auto styleTheme = qlementineStyle->theme();
    const auto hSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);

    auto* frame = new StyledFrame(&owner);
    frame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
    frame->setBorderRole(StyledFrame::BorderRole::Mid);
    frame->setRadius(styleTheme.borderRadius);
    frame->setBorderWidth(styleTheme.borderWidth);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* formLayout = new QFormLayout(frame);
    formLayout->setHorizontalSpacing(hSpacing * 8);
    formLayout->setContentsMargins(hSpacing, vSpacing, hSpacing, vSpacing);
    formLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);

    if (title == "Primary Color")
    {
      // ADD_TITLE("Primary Color");
      ADD_SUBTITLE("Background")
      ADD_DESCRIPTION("Used to highlight elements.")
      ADD_COLOR_EDITOR(primaryColor, "");
      ADD_COLOR_EDITOR(primaryColorHovered, "");
      ADD_COLOR_EDITOR(primaryColorPressed, "");
      ADD_COLOR_EDITOR(primaryColorDisabled, "");
      ADD_SUBTITLE("Foreground")
      ADD_DESCRIPTION("Text drawn over highlighted elements.");
      ADD_COLOR_EDITOR(primaryColorForeground, "");
      ADD_COLOR_EDITOR(primaryColorForegroundHovered, "");
      ADD_COLOR_EDITOR(primaryColorForegroundPressed, "");
      ADD_COLOR_EDITOR(primaryColorForegroundDisabled, "");
      ADD_SUBTITLE("Alternative")
      ADD_DESCRIPTION("Used to highlight elements over already highlighted elements.")
      ADD_COLOR_EDITOR(primaryAlternativeColor, "");
      ADD_COLOR_EDITOR(primaryAlternativeColorHovered, "");
      ADD_COLOR_EDITOR(primaryAlternativeColorPressed, "");
      ADD_COLOR_EDITOR(primaryAlternativeColorDisabled, "");
    }
    else if (title == "Secondary Color")
    {
      // ADD_TITLE("Secondary Color");
      ADD_SUBTITLE("Background");
      ADD_DESCRIPTION("A more neutral color, used for text and non-highlighted elements.");
      ADD_COLOR_EDITOR(secondaryColor, "");
      ADD_COLOR_EDITOR(secondaryColorHovered, "");
      ADD_COLOR_EDITOR(secondaryColorPressed, "");
      ADD_COLOR_EDITOR(secondaryColorDisabled, "");
      ADD_SUBTITLE("Foreground")
      ADD_DESCRIPTION("Text drawn over elements in secondary color.")
      ADD_COLOR_EDITOR(secondaryColorForeground, "");
      ADD_COLOR_EDITOR(secondaryColorForegroundHovered, "");
      ADD_COLOR_EDITOR(secondaryColorForegroundPressed, "");
      ADD_COLOR_EDITOR(secondaryColorForegroundDisabled, "");
      ADD_SUBTITLE("Secondary Alternative Color");
      ADD_DESCRIPTION("A lighter version of the secondary color.");
      ADD_COLOR_EDITOR(secondaryAlternativeColor, "");
      ADD_COLOR_EDITOR(secondaryAlternativeColorHovered, "");
      ADD_COLOR_EDITOR(secondaryAlternativeColorPressed, "");
      ADD_COLOR_EDITOR(secondaryAlternativeColorDisabled, "");
    }
    else if (title == "Neutral Color")
    {
      // Neutral color.
      // ADD_TITLE("Neutral Color");
      ADD_DESCRIPTION("Used for hover, pressed, and disabled feedback");
      ADD_COLOR_EDITOR(neutralColor, "");
      ADD_COLOR_EDITOR(neutralColorHovered, "");
      ADD_COLOR_EDITOR(neutralColorPressed, "");
      ADD_COLOR_EDITOR(neutralColorDisabled, "");
    }
    else if (title == "Semi-transparent Color")
    {
      // Semi-transparent color.
      // ADD_TITLE("Semi-transparent Color");
      ADD_DESCRIPTION("Used for semi-transparent elements such as scrollbars.");
      ADD_COLOR_EDITOR(semiTransparentColor1, "");
      ADD_COLOR_EDITOR(semiTransparentColor2, "");
      ADD_COLOR_EDITOR(semiTransparentColor3, "");
      ADD_COLOR_EDITOR(semiTransparentColor4, "");
    }
    else if (title == "Background Color")
    {
      // Background Color.
      // ADD_TITLE("Background Color");
      ADD_DESCRIPTION("Used for containers: windows, GroupBoxes, etc.")
      ADD_COLOR_EDITOR(backgroundColorMain1, "Main background color (window, etc).");
      ADD_COLOR_EDITOR(backgroundColorMain2, "");
      ADD_COLOR_EDITOR(backgroundColorMain3, "");
      ADD_COLOR_EDITOR(backgroundColorMain4, "");
    }
    else if (title == "Border Color")
    {
      // Border Color.
      // ADD_TITLE("Border Color");
      ADD_DESCRIPTION("Used to draw the borders: ComboBox, GroupBox's border, etc.");
      ADD_COLOR_EDITOR(borderColor, "");
      ADD_COLOR_EDITOR(borderColorHovered, "");
      ADD_COLOR_EDITOR(borderColorPressed, "");
      ADD_COLOR_EDITOR(borderColorDisabled, "");
    }
    else if (title == "Effect Color")
    {
      // Focus Color.
      ADD_SUBTITLE("Focus Color");
      ADD_COLOR_EDITOR(focusColor, "Color for the focus border.");

      // Shadow Color.
      ADD_SUBTITLE("Shadow Color");
      ADD_COLOR_EDITOR(shadowColor1, "Shadow 1");
      ADD_COLOR_EDITOR(shadowColor2, "Shadow 2");
      ADD_COLOR_EDITOR(shadowColor3, "Shadow 3");
    }
    else if (title == "Status Color")
    {
      // Status Color.
      // ADD_TITLE("Status Color");
      ADD_DESCRIPTION("Displaying feedback to the user (success, error, etc.)")
      ADD_SUBTITLE("Error");
      ADD_COLOR_EDITOR(statusColorError, "");
      ADD_COLOR_EDITOR(statusColorErrorHovered, "");
      ADD_COLOR_EDITOR(statusColorErrorPressed, "");
      ADD_COLOR_EDITOR(statusColorErrorDisabled, "");
      ADD_SUBTITLE("Warning");
      ADD_COLOR_EDITOR(statusColorWarning, "");
      ADD_COLOR_EDITOR(statusColorWarningHovered, "");
      ADD_COLOR_EDITOR(statusColorWarningPressed, "");
      ADD_COLOR_EDITOR(statusColorWarningDisabled, "");
      ADD_SUBTITLE("Success");
      ADD_COLOR_EDITOR(statusColorSuccess, "");
      ADD_COLOR_EDITOR(statusColorSuccessHovered, "");
      ADD_COLOR_EDITOR(statusColorSuccessPressed, "");
      ADD_COLOR_EDITOR(statusColorSuccessDisabled, "");
      ADD_SUBTITLE("Info");
      ADD_COLOR_EDITOR(statusColorInfo, "");
      ADD_COLOR_EDITOR(statusColorInfoHovered, "");
      ADD_COLOR_EDITOR(statusColorInfoPressed, "");
      ADD_COLOR_EDITOR(statusColorInfoDisabled, "");
      ADD_SUBTITLE("Foreground");
      ADD_DESCRIPTION("Used to draw text over status colors.");
      ADD_COLOR_EDITOR(statusColorForeground, "");
      ADD_COLOR_EDITOR(statusColorForegroundHovered, "");
      ADD_COLOR_EDITOR(statusColorForegroundPressed, "");
      ADD_COLOR_EDITOR(statusColorForegroundDisabled, "");
    }

    auto section = new SectionWidget(&owner);
    section->addItem(frame, title, oclero::qlementine::TextRole::H5);
    section->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    section->setExpanded(false);

    parentLayout->addRow(section);
  }

  void setupUi()
  {
    auto* globalLayout = new QVBoxLayout(&owner);
    // globalLayout->setContentsMargins(0, 0, 0, 0);
    owner.setLayout(globalLayout);

    const auto vSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    const auto hSpacing = owner.style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);

    globalLayout->setContentsMargins(hSpacing, 0, hSpacing, 0);

    QWidget* header = new QWidget(&owner);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    header->setLayout(headerLayout);

    auto* buttonWidget = new QWidget(header);
    auto* buttonLayout = new QFormLayout(buttonWidget);
    buttonLayout->setHorizontalSpacing(hSpacing * 8);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);

    auto* metadataWidget = new QWidget(header);
    auto* metadataLayout = new QFormLayout(metadataWidget);
    metadataLayout->setHorizontalSpacing(hSpacing * 8);
    metadataLayout->setContentsMargins(0, 0, 0, 0);
    metadataLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);

    headerLayout->addWidget(metadataWidget, 0, Qt::AlignVCenter);
    headerLayout->addSpacing(hSpacing);
    headerLayout->addWidget(buttonWidget, 0, Qt::AlignVCenter);

    setupMetadataEditors(metadataLayout, vSpacing);
    setupJSONLoader(&owner, buttonLayout);

    auto* reminder = new oclero::qlementine::Label(tr("Note: Changes are not saved unless you export the theme"), header);
    reminder->setRole(oclero::qlementine::TextRole::Default);
    buttonLayout->addRow(reminder);

    QWidget* container = new QWidget(&owner);
    auto* formLayout = new QFormLayout(container);
    formLayout->setFormAlignment(Qt::AlignTop);
    formLayout->setHorizontalSpacing(hSpacing * 8);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
    container->setLayout(formLayout);

    setupColorEditors(formLayout, vSpacing);

    auto* editorScroll = new QScrollArea(&owner);
    editorScroll->setWidgetResizable(true);
    editorScroll->setFrameShape(QFrame::NoFrame);
    editorScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editorScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    editorScroll->setWidget(container);

    auto* line = new QFrame(&owner);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);

    globalLayout->addWidget(header);
    globalLayout->addWidget(new Label(tr("Theme Editor"), TextRole::H3, &owner));
    globalLayout->addWidget(line);
    globalLayout->addWidget(editorScroll);
  }

  // Update all the widgets.
  void updateUi()
  {
    UPDATE_COLOR_EDITOR(primaryColor);
    UPDATE_COLOR_EDITOR(primaryColorHovered);
    UPDATE_COLOR_EDITOR(primaryColorPressed);
    UPDATE_COLOR_EDITOR(primaryColorDisabled);

    UPDATE_COLOR_EDITOR(primaryColorForeground);
    UPDATE_COLOR_EDITOR(primaryColorForegroundHovered);
    UPDATE_COLOR_EDITOR(primaryColorForegroundPressed);
    UPDATE_COLOR_EDITOR(primaryColorForegroundDisabled);

    UPDATE_COLOR_EDITOR(secondaryColor);
    UPDATE_COLOR_EDITOR(secondaryColorHovered);
    UPDATE_COLOR_EDITOR(secondaryColorPressed);
    UPDATE_COLOR_EDITOR(secondaryColorDisabled);

    UPDATE_COLOR_EDITOR(secondaryColorForeground);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundHovered);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundPressed);
    UPDATE_COLOR_EDITOR(secondaryColorForegroundDisabled);

    UPDATE_COLOR_EDITOR(secondaryAlternativeColor);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorHovered);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorPressed);
    UPDATE_COLOR_EDITOR(secondaryAlternativeColorDisabled);

    UPDATE_COLOR_EDITOR(neutralColor);
    UPDATE_COLOR_EDITOR(neutralColorHovered);
    UPDATE_COLOR_EDITOR(neutralColorPressed);
    UPDATE_COLOR_EDITOR(neutralColorDisabled);

    UPDATE_COLOR_EDITOR(semiTransparentColor1);
    UPDATE_COLOR_EDITOR(semiTransparentColor2);
    UPDATE_COLOR_EDITOR(semiTransparentColor3);
    UPDATE_COLOR_EDITOR(semiTransparentColor4);

    UPDATE_COLOR_EDITOR(backgroundColorMain1);
    UPDATE_COLOR_EDITOR(backgroundColorMain2);
    UPDATE_COLOR_EDITOR(backgroundColorMain3);
    UPDATE_COLOR_EDITOR(backgroundColorMain4);

    UPDATE_COLOR_EDITOR(borderColor);
    UPDATE_COLOR_EDITOR(borderColorHovered);
    UPDATE_COLOR_EDITOR(borderColorPressed);
    UPDATE_COLOR_EDITOR(borderColorDisabled);

    UPDATE_COLOR_EDITOR(focusColor);

    UPDATE_COLOR_EDITOR(shadowColor1);
    UPDATE_COLOR_EDITOR(shadowColor2);
    UPDATE_COLOR_EDITOR(shadowColor3);

    UPDATE_COLOR_EDITOR(statusColorError);
    UPDATE_COLOR_EDITOR(statusColorErrorHovered);
    UPDATE_COLOR_EDITOR(statusColorErrorPressed);
    UPDATE_COLOR_EDITOR(statusColorErrorDisabled);

    UPDATE_COLOR_EDITOR(statusColorWarning);
    UPDATE_COLOR_EDITOR(statusColorWarningHovered);
    UPDATE_COLOR_EDITOR(statusColorWarningPressed);
    UPDATE_COLOR_EDITOR(statusColorWarningDisabled);

    UPDATE_COLOR_EDITOR(statusColorSuccess);
    UPDATE_COLOR_EDITOR(statusColorSuccessHovered);
    UPDATE_COLOR_EDITOR(statusColorSuccessPressed);
    UPDATE_COLOR_EDITOR(statusColorSuccessDisabled);

    UPDATE_COLOR_EDITOR(statusColorInfo);
    UPDATE_COLOR_EDITOR(statusColorInfoHovered);
    UPDATE_COLOR_EDITOR(statusColorInfoPressed);
    UPDATE_COLOR_EDITOR(statusColorInfoDisabled);

    UPDATE_COLOR_EDITOR(statusColorForeground);
    UPDATE_COLOR_EDITOR(statusColorForegroundHovered);
    UPDATE_COLOR_EDITOR(statusColorForegroundPressed);
    UPDATE_COLOR_EDITOR(statusColorForegroundDisabled);

    UPDATE_METADATA_TEXT_EDITOR(name);
    UPDATE_METADATA_TEXT_EDITOR(author);
    UPDATE_METADATA_TEXT_EDITOR(version);
  }

  QString getFileName(const QString& title)
  {
    QFileDialog dialog(&owner, title);

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("JSON Files (*.json)");

    dialog.setDirectory(defaultDirPath);

    // Disable navigation outside
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);

    auto* model = dialog.findChild<QFileSystemModel*>();
    if (model)
    {
      model->setRootPath(defaultDirPath);
      // model->setRootIndex(model->index(defaultDirPath));
    }

    if (dialog.exec() == QDialog::Accepted)
      return dialog.selectedFiles().first();

    return "";
  }

  ThemeEditorWidget& owner;
  Theme theme;
  QString defaultDirPath = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DocumentsLocation);
};

ThemeEditorWidget::ThemeEditorWidget(QWidget* parent)
    : QWidget(parent)
    , _impl(std::make_unique<Impl>(*this))
{
  _impl->setupUi();
}

ThemeEditorWidget::~ThemeEditorWidget() = default;

const Theme& ThemeEditorWidget::theme() const
{
  return _impl->theme;
}

void ThemeEditorWidget::setTheme(const Theme& theme)
{
  if (theme != _impl->theme)
  {
    _impl->theme = theme;
    _impl->updateUi();
    Q_EMIT themeChanged(_impl->theme);
  }
}

void ThemeEditorWidget::setDefaultPath(const QString& savePath)
{
  // Make sure the directory exists
  QDir dir(savePath);
  if (!dir.exists())
    dir.mkpath(".");

  _impl->defaultDirPath = savePath;
  _impl->updateUi();
}

}  // namespace oclero::qlementine
