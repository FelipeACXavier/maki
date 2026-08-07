#include "logging.h"
#include "result.h"
#include "type_definition.h"
#include "type_registry.h"

using namespace koda::types;

int main()
{
  TypeRegistry registry;
  registry.registerBuiltinTypes();

  const auto realType = registry.findByName(RealType);
  if (!realType)
  {
    LOG_ERROR("No {} registered", RealType);
    return -1;
  }

  const auto stringType = registry.findByName(StringType);
  if (!stringType)
  {
    LOG_ERROR("No {} registered", StringType);
    return -1;
  }

  TypeDefinition booleanType = TypeDefinition::createPrimitive("builtin::Bool", PrimitiveKind::Bool);
  TypeDefinition shortType = TypeDefinition::createPrimitive("builtin::Short", PrimitiveKind::Int8);

  TypeDefinition vector3Type = TypeDefinition::createRecord("robotics::Vector3", {
                                                                                     {"x", realType->name},
                                                                                     {"y", realType->name},
                                                                                     {"z", realType->name},
                                                                                 });

  TypeDefinition namedPositionsType = TypeDefinition::createMap("robotics::NamedPositions", stringType->name, vector3Type.name);
  TypeDefinition waypointType = TypeDefinition::createList("robotics::Waypoint", vector3Type.name);
  TypeDefinition pathType = TypeDefinition::createAlias("robotics::Path", waypointType.name);

  TypeDefinition robotStateType = TypeDefinition::createEnum("robotics::RobotState", EnumUnderlyingKind::String,
                                                             {
                                                                 {"Idle", "idle"},
                                                                 {"Driving", "driving"},
                                                                 {"Manipulating", "manipulating"},
                                                                 {"Error", "error"},
                                                             });

  TypeDefinition distanceType = TypeDefinition::createAlias("robotics::Distance", realType->name);
  TypeDefinition stampedVector3Type = TypeDefinition::createRecord("robotics::StampedVector3",
                                                                   {
                                                                       {"frameId", stringType->name},
                                                                       {"timestamp", TimestampType},
                                                                   },
                                                                   vector3Type.name.toString());

  LOG_ERROR_ON_FAILURE(registry.add(booleanType));  // This should log, boolean is already defined

  LOG_ERROR_ON_FAILURE(registry.add(shortType));
  LOG_ERROR_ON_FAILURE(registry.add(vector3Type));
  LOG_ERROR_ON_FAILURE(registry.add(namedPositionsType));

  LOG_ERROR_ON_FAILURE(registry.add(waypointType));
  LOG_ERROR_ON_FAILURE(registry.add(pathType));

  LOG_ERROR_ON_FAILURE(registry.add(robotStateType));
  LOG_ERROR_ON_FAILURE(registry.add(distanceType));
  LOG_ERROR_ON_FAILURE(registry.add(stampedVector3Type));
  registry.print();

  const auto fields = registry.fieldsOf(vector3Type);
  for (const auto& f : fields)
    LOG_DEBUG("vector3Type field: {}", f.name);

  const auto sfields = registry.fieldsOf(stampedVector3Type);
  for (const auto& f : sfields)
    LOG_DEBUG("stampedVector3Type field: {}", f.name);

  return 0;
}