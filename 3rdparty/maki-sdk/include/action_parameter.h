#pragma once

#include "idocument.h"
#include "mission_parameter.h"

namespace maki
{
class ActionParameter : public IParameter
{
public:
  ActionParameter(const QString& id, const koda::types::TypeReference& type, const Value& value);

  QString getid() const override;
  QString getname() const override;
  const IValue* getvalue() const override;
  koda::types::TypeReference gettype() const override;
  Types::ControlTypes getcontrol() const override;

private:
  const QString mId;
  const koda::types::TypeReference mType;
  const Value mValue;
};
}  // namespace maki