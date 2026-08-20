#pragma once

#include "../base_dialog.h"
#include "mission_parameter.h"
#include "typing/type_reference.h"

namespace maki
{

struct WidgetAlignment;
class WidgetScrollGroup;
class StringWidget;
class TypeSelector;
class InputWidget;

class TypeValueDialog : public BaseDialog
{
  Q_OBJECT

public:
  TypeValueDialog(const QString& title, QWidget* parent = nullptr);

  void setParameter(const MissionParameter& parameter);
  MissionParameter getParameter() const;

public slots:
  void accept() override;

private:
  MissionParameter mParameter;
  maki::WidgetScrollGroup* mValueGroup;
  maki::StringWidget* mNameEditor;
  TypeSelector* mTypeEditor;
  QDialogButtonBox* mButtonBox;

  void buildUi();
  void buildValueGroup(const koda::types::TypeReference& ref, const Value& value);
  void updateFocus(maki::InputWidget* editor);
};

}  // namespace maki