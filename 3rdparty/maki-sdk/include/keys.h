#pragma once

#include <QString>

namespace ConfigKeys
{
// Generic
static const QString ID = "id";
static const QString NAME = "name";
static const QString LABEL = "label";
static const QString TYPE = "type";
static const QString TYPES = "types";
static const QString CONTROL = "control";
static const QString DEFAULT = "default";
static const QString SIZE = "size";
static const QString SCALE = "scale";
static const QString NODE_ID = "nodeId";
static const QString POSITION = "position";
static const QString PARENT_ID = "parentId";
static const QString MODIFIABLE = "modifiable";
static const QString OWNER = "owner";
static const QString INFO = "info";
static const QString VALUE = "value";
static const QString LIBRARIES = "libraries";
static const QString IS_VARIABLE = "isVariable";
static const QString EVENT = "event";
static const QString COMPONENT = "component";
static const QString COMPONENT_ID = "componentId";
static const QString ITEMS = "items";
static const QString FLOW = "flow";
static const QString CALL = "call";
static const QString CALL_ID = "callId";

static const QString FIELDS = "fields";
static const QString EVENTS = "events";
static const QString OPTIONS = "options";
static const QString CONNECTORS = "connectors";
static const QString PROPERTIES = "properties";
static const QString CHILDREN = "children";
static const QString FLOWS = "flows";
static const QString BEHAVIOUR = "behaviour";

static const QString ARGUMENTS = "arguments";
static const QString RETURN_TYPE = "returnType";

static const QString X = "x";
static const QString Y = "y";
static const QString WIDTH = "width";
static const QString HEIGHT = "height";
static const QString PIXMAP = "pixmap";
static const QString ICON_PATH = "iconPath";
static const QString DATA = "data";
static const QString OPTION_DATA = "option_data";

static const QString SHIFT = "shift";
static const QString SOURCE = "source";
static const QString DESTINATION = "destination";

// Ids
static const QString CONFIG_ID = "configId";
static const QString CONNECTOR_ID = "connectorId";

static const QString CANVAS = "canvas";
static const QString STRUCTURAL = "structural";
static const QString BEHAVIOURAL = "behavioural";
static const QString PIPELINE = "pipeline";
static const QString TRANSITIONS = "transitions";
static const QString NODES = "nodes";

// =======================================
// Node types
static const QString KODA_NAMESPACE = "Koda::";
static const QString CAPABILITY_CALL_NODE = KODA_NAMESPACE + "Call capability";
static const QString FLOW_CALL_NODE = KODA_NAMESPACE + "Flow call";
static const QString WITHIN_NODE = KODA_NAMESPACE + "Within";
static const QString REPEAT_NODE = KODA_NAMESPACE + "Repeat";
static const QString JOIN_NODE = KODA_NAMESPACE + "Join";
static const QString LINK_IN_NODE = KODA_NAMESPACE + "Link in";
static const QString LINK_OUT_NODE = KODA_NAMESPACE + "Link out";
static const QString CONTINUE_NODE = KODA_NAMESPACE + "Continue";
static const QString START_NODE = KODA_NAMESPACE + "Start";
static const QString SUCCESS_NODE = KODA_NAMESPACE + "Success";
static const QString TERMINATE_NODE = KODA_NAMESPACE + "Terminate";
static const QString FAILURE_NODE = KODA_NAMESPACE + "Failure";
static const QString TASK_NODE = KODA_NAMESPACE + "Task";

}  // namespace ConfigKeys
