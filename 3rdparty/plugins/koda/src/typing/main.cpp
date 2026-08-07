#include <iostream>

#include "type_model.h"

using namespace koda::types;

int main()
{
  TypeDefinition vector3{
      .id = "builtin.robotics.vector3",
      .name = QualifiedName("robotics::Vector3"),
      .data = RecordTypeDefinition{
          .baseType = std::nullopt,
          .fields = {
              FieldDefinition{
                  .name = "x",
                  .type = TypeReference::primitive(PrimitiveKind::Float64),
              },
              FieldDefinition{
                  .name = "y",
                  .type = TypeReference::primitive(PrimitiveKind::Float64),
              },
              FieldDefinition{
                  .name = "z",
                  .type = TypeReference::primitive(PrimitiveKind::Float64),
              },
          },
      },
      .documentation = "A three-dimensional vector.",
  };

  TypeDefinition pose{
      .id = "project.object-pose",
      .name = QualifiedName("project::ObjectPose"),
      .data = RecordTypeDefinition{
          .baseType = std::nullopt,
          .fields = {
              FieldDefinition{
                  .name = "frameId",
                  .type = TypeReference::primitive(PrimitiveKind::String),
              },
              FieldDefinition{
                  .name = "position",
                  .type = TypeReference::named(
                      vector3.name,
                      vector3.id),
              },
              FieldDefinition{
                  .name = "tags",
                  .type = TypeReference::list(
                      TypeReference::primitive(PrimitiveKind::String)),
                  .required = false,
              },
          },
      },
      .annotations = {
          {"robotics.semantic_type", "Pose"},
          {"ros.suggested_message", "geometry_msgs/msg/PoseStamped"},
      },
  };

  TypeDefinition booleanType{
      .id = "builtin.bool",
      .name = QualifiedName("builtin::Bool"),
      .data = PrimitiveTypeDefinition{
          .primitive = PrimitiveKind::Bool,
      },
  };

  TypeDefinition integerType{
      .id = "builtin.integer",
      .name = QualifiedName("builtin::Integer"),
      .data = PrimitiveTypeDefinition{
          .primitive = PrimitiveKind::Int64,
      },
  };

  TypeDefinition realType{
      .id = "builtin.real",
      .name = QualifiedName("builtin::Real"),
      .data = PrimitiveTypeDefinition{
          .primitive = PrimitiveKind::Float64,
      },
  };

  TypeDefinition stringType{
      .id = "builtin.string",
      .name = QualifiedName("builtin::String"),
      .data = PrimitiveTypeDefinition{
          .primitive = PrimitiveKind::String,
      },
  };

  TypeDefinition vector3Type{
      .id = "robotics.vector3",
      .name = QualifiedName("robotics::Vector3"),
      .data = RecordTypeDefinition{
          .baseType = std::nullopt,
          .fields = {
              FieldDefinition{
                  .name = "x",
                  .type = TypeReference::named(
                      realType.name,
                      realType.id),
              },
              FieldDefinition{
                  .name = "y",
                  .type = TypeReference::named(
                      realType.name,
                      realType.id),
              },
              FieldDefinition{
                  .name = "z",
                  .type = TypeReference::named(
                      realType.name,
                      realType.id),
              },
          },
      },
      .documentation = "A three-dimensional Cartesian vector.",
  };

  TypeDefinition namedPositionsType{
      .id = "robotics.named-positions",
      .name = QualifiedName("robotics::NamedPositions"),
      .data = AliasTypeDefinition{
          .target = TypeReference::map(
              TypeReference::named(stringType.name, stringType.id),
              TypeReference::named(vector3Type.name, vector3Type.id)),
      },
  };

  TypeDefinition waypointType{
      .id = "robotics.waypoint",
      .name = QualifiedName("robotics::Waypoint"),
      .data = AliasTypeDefinition{
          .target = TypeReference::list(TypeReference::named(vector3Type.name, vector3Type.id)),
      },
  };

  TypeDefinition pathType{
      .id = "robotics.path",
      .name = QualifiedName("robotics::Path"),
      .data = AliasTypeDefinition{
          .target = TypeReference::named(waypointType.name, waypointType.id),
      },
  };

  TypeDefinition robotStateType{
      .id = "robotics.robot-state",
      .name = QualifiedName("robotics::RobotState"),
      .data = EnumTypeDefinition{
          .underlyingType = EnumUnderlyingKind::String,
          .values = {
              EnumValueDefinition{
                  .name = "Idle",
                  .value = "\"idle\"",
              },
              EnumValueDefinition{
                  .name = "Driving",
                  .value = "\"driving\"",
              },
              EnumValueDefinition{
                  .name = "Manipulating",
                  .value = "\"manipulating\"",
              },
              EnumValueDefinition{
                  .name = "Error",
                  .value = "\"error\"",
              },
          },
      },
  };

  TypeDefinition distanceType{
      .id = "robotics.distance",
      .name = QualifiedName("robotics::Distance"),
      .data = AliasTypeDefinition{
          .target = TypeReference::named(realType.name, realType.id),
      },
      .documentation = "A distance expressed in metres.",
      .annotations = {
          {"unit", "m"},
      },
  };

  TypeDefinition stampedVector3Type{
      .id = "robotics.stamped-vector3",
      .name = QualifiedName("robotics::StampedVector3"),
      .data = RecordTypeDefinition{
          .baseType = TypeReference::named(
              vector3Type.name,
              vector3Type.id),
          .fields = {
              FieldDefinition{
                  .name = "frameId",
                  .type = TypeReference::named(stringType.name, stringType.id),
              },
              FieldDefinition{
                  .name = "timestamp",
                  .type = TypeReference::primitive(PrimitiveKind::Timestamp),
              },
          },
      },
  };

  TypeDefinition detectedObjectType{
      .id = "robotics.detected-object",
      .name = QualifiedName("robotics::DetectedObject"),
      .data = RecordTypeDefinition{
          .fields = {
              FieldDefinition{
                  .name = "name",
                  .type = TypeReference::named(stringType.name, stringType.id),
              },
              FieldDefinition{
                  .name = "position",
                  .type = TypeReference::named(vector3Type.name, vector3Type.id),
                  .required = false,  // This makes the optional field
              },
          },
      },
  };

  std::cout << "Base ------------------------\n";
  std::cout << pose.name.toString() << '\n';
  std::cout << integerType.name.toString() << '\n';

  std::cout << "Records ------------------------\n";
  for (const FieldDefinition& field : pose.record().fields)
    std::cout << "  " << field.name << ": " << field.type.toString() << '\n';

  const auto diagnostics = pose.validate();

  for (const TypeModelDiagnostic& diagnostic : diagnostics)
  {
    std::cout << diagnostic.code << ": " << diagnostic.message << '\n';
  }

  return diagnostics.empty() ? 0 : 1;
}