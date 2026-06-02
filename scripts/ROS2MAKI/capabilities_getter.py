
from asyncio import events
import os
import subprocess
import re
from dataclasses import dataclass, field
from llm_api import ask_llm

from clang.cindex import Config

Config.set_library_file("/usr/lib/llvm-21/lib/libclang.so")
from clang.cindex import Index, CursorKind, AccessSpecifier

@dataclass
class Parameter:
    name: str
    type: str 
    default_value: str | int | float | bool | list

@dataclass
class RosCapability:
    name: str

    parameters: list[Parameter] = field(default_factory=list)
    events: list[str] = field(default_factory=list)


class ParameterPattern:
    regex: str

    def extract(self, content):
        pass

class Nav2ParameterPattern(ParameterPattern):

    regex = r'declare_or_get_parameter\s*\(\s*"([^"]+)"\s*,\s*(.*?)\s*\)'

    def extract(self, content):
        matches = re.findall(self.regex, content)

        parameters = []

        for name, default in matches:

            parameter = Parameter(
                name=name,
                type=infer_type(default),
                default_value=parse_value(default)
            )

            parameters.append(parameter)

        return parameters

class MoveItParameterPattern1(ParameterPattern):

    regex = r'declareOrGetParam<(.+?)>\s*\(.*?"([^"]+)".*?,\s*(.*?)\s*,\s*(.*?)\s*\)'

    def extract(self, content):

        matches = re.findall(self.regex, content)

        parameters = []

        for cpp_type, name, variable, default in matches:

            parameter = Parameter(
                name=name,
                type=cpp_type,
                default_value=parse_value(default)
            )


            parameters.append(parameter)

        return parameters
    
class SlamToolBoxParameterPattern(ParameterPattern):

    regex = r'declare_parameter\s*\(\s*"([^"]+)"\s*,\s*([a-zA-Z0-9_]+)\s*\)'

    def extract(self, content):

        matches = re.findall(self.regex, content)

        parameters = []

        for name, variable in matches:

            parameter = Parameter(
                name=name,
                type="unknown",
                default_value=None
            )


            parameters.append(parameter)

        return parameters

class MoveIt2ParameterPattern2(ParameterPattern):

    regex = (
        r'declare_parameter\s*\('
        r'\s*"([^"]+)"\s*,'
        r'\s*rclcpp::ParameterType::([A-Z_]+)\s*,'
        r'\s*([a-zA-Z0-9_]+)\s*'
        r'\)'
    )

    def extract(self, content):

        matches = re.findall(self.regex, content)

        parameters = []

        for name, ros_type, descriptor in matches:

            parameter = Parameter(
                name=name,
                type=self.convert_ros_type(ros_type),
                default_value=None
            )
        
            parameters.append(parameter)

        return parameters

    def convert_ros_type(self, ros_type):

        mapping = {
            "PARAMETER_STRING": "string",
            "PARAMETER_BOOL": "bool",
            "PARAMETER_INTEGER": "int",
            "PARAMETER_DOUBLE": "double",
            "PARAMETER_STRING_ARRAY": "list[string]",
            "PARAMETER_INTEGER_ARRAY": "list[int]",
            "PARAMETER_DOUBLE_ARRAY": "list[double]",
            "PARAMETER_BOOL_ARRAY": "list[bool]"
        }

        return mapping.get(ros_type, "unknown")

PARAMETER_PATTERNS = [
    Nav2ParameterPattern(),
    SlamToolBoxParameterPattern(),
    MoveIt2ParameterPattern2(),
    MoveItParameterPattern1()
]

def infer_type(value):
    value = value.strip()

    if value in ["true", "false"]:
        return "bool"

    if "std::string" in value:
        return "string"

    try:
        int(value)
        return "int"
    except:
        pass

    try:
        float(value)
        return "double"
    except:
        pass

    return "unknown"

def parse_value(value):
    value = value.strip()

    if value == "true":
        return True

    if value == "false":
        return False

    if "std::string" in value:
        match = re.search(r'"([^"]*)"', value)
        if match:
            return match.group(1)

    try:
        return int(value)
    except:
        pass

    try:
        return float(value)
    except:
        pass

    return "" 

def remove_duplicate_params(all_parameters):
    unique_parameters = {
        param.name: param
        for param in all_parameters
    }
    return list(unique_parameters.values())

def remove_duplicate_methods(methods):
    unique_methods = {
        method['name']: method
        for method in methods
    }
    return list(unique_methods.values())

def get_parameters(package_name, package_path):
    print(f"Scanning ROS package: {package_name}")
    print(f"Package path: {package_path}")

   
    src_path = os.path.join(package_path, "src")

    if not os.path.exists(src_path):
        print(f"No src folder found in {package_path}")
        return []

    all_parameters = []

    for root, dirs, files in os.walk(src_path):
        for file_name in files:
            if not file_name.endswith((".cpp", ".hpp", ".cc", ".cxx", ".h")):
                continue

            file_path = os.path.join(root, file_name)

            print(f"Parsing file: {file_path}")

            try:
                with open(file_path, "r", encoding="utf-8") as f:
                    content = f.read()

                for pattern in PARAMETER_PATTERNS:
                    extracted = pattern.extract(content)
                    all_parameters.extend(extracted)
                

                all_parameters = remove_duplicate_params(all_parameters)

            except Exception as e:
                print(f"Failed to parse {file_path}: {e}")

    return all_parameters

def contains_src_folder(path):
    for root, dirs, files in os.walk(path):
        if "src" in dirs:
            return True
    return False

def access_to_string(access):
    if access == AccessSpecifier.PUBLIC:
        return "public"

    if access == AccessSpecifier.PROTECTED:
        return "protected"

    if access == AccessSpecifier.PRIVATE:
        return "private"

    return "unknown"


def clean_type(type_name):
    """
    Make C++ types cleaner for LLM prompts.
    """

    replacements = [
        ("geometry_msgs::msg::", ""),
        ("std::", ""),
        ("const ", ""),
        (" &", ""),
        ("::SharedPtr", "")
    ]

    for old, new in replacements:
        type_name = type_name.replace(old, new)

    return type_name.strip()

def classify_method(method_name):
    lifecycle_methods = {
        "on_configure",
        "on_activate",
        "on_deactivate",
        "on_cleanup",
        "on_shutdown"
    }

    callback_keywords = [
        "callback",
        "cb",
        "handle",
        "execute"
    ]

    utility_keywords = [
        "get",
        "set",
        "check",
        "publish",
        "create",
        "update"
    ]

    if method_name in lifecycle_methods:
        return "lifecycle"

    if any(k in method_name.lower() for k in callback_keywords):
        return "callback"

    if any(method_name.lower().startswith(k) for k in utility_keywords):
        return "utility"

    return "method"

def is_from_package(node, package_path):
    if not node.location.file:
        return False

    file_path = str(node.location.file)

    return package_path in file_path

def extract_context(package_path):

    include_path = os.path.join(package_path, "include")

    if not os.path.exists(include_path):
        print(f"No include folder found in {package_path}")
        return []

    methods = []

    index = Index.create()

    for root, dirs, files in os.walk(include_path):

        for file_name in files:

            if not file_name.endswith((".hpp")):
                continue

            file_path = os.path.join(root, file_name)

            print(f"Analyzing header: {file_path}")

            try:
                tu = index.parse(file_path,args=["-std=c++17"])
                current_class = None

                for node in tu.cursor.walk_preorder():

                    if node.kind in [
                        CursorKind.CLASS_DECL,
                        CursorKind.STRUCT_DECL
                    ]:

                        if node.spelling:
                            current_class = node.spelling

                    if node.kind == CursorKind.CXX_METHOD:
                        if not is_from_package(node, package_path):
                            continue

                        inputs = []

                        for arg in node.get_arguments():

                            inputs.append({
                                "name": arg.spelling,
                                "type": clean_type(arg.type.spelling)
                            })

                        method_data = {
                            "name": node.spelling,

                            "class": current_class,

                            "visibility": access_to_string(
                                node.access_specifier
                            ),

                            "type": classify_method(
                                node.spelling
                            ),

                            "inputs": inputs,

                            "outputs": [{
                                "type": clean_type(
                                    node.result_type.spelling
                                )
                            }],

                            "is_async": any(
                                k in node.spelling.lower()
                                for k in [
                                    "async",
                                    "callback",
                                    "execute",
                                    "follow"
                                ]
                            ),

                            "is_blocking": any(
                                k in node.spelling.lower()
                                for k in [
                                    "wait",
                                    "spin",
                                    "follow",
                                    "approach"
                                ]
                            ),
                        }

                        methods.append(method_data)

            except Exception as e:
                print(f"Failed parsing {file_path}: {e}")
    methods = remove_duplicate_methods(methods)
    return methods

def print_context(context):
    print("\nExtracted Context:")
    for method in context:
        print(f"Method: {method['name']} (Class: {method['class']}, Visibility: {method['visibility']}, Type: {method['type']})")
        print("  Inputs:")
        for inp in method["inputs"]:
            print(f"    - {inp['name']} (type: {inp['type']})")
        print(f"Method type: {method['type']}")
        print("  Outputs:")
        for out in method["outputs"]:
            print(f"    - (type: {out['type']})")
        print(f"  Is Async: {method['is_async']}")
        print(f"  Is Blocking: {method['is_blocking']}")
        print("\n")


def get_events(package_name, package_path, parameters=None):
    events = []
    include_path = os.path.join(package_path, "include")

    if not os.path.exists(include_path):
        print(f"No include folder found in {package_path}")
        return []
    
    events = extract_context(package_path)
    
    return events

def pkg_to_process(package_name, repo_path):
    packages_to_process = []

    print (f"Searching for packages in repository: {repo_path} with common denominator: {package_name}")

    for root, dirs, files in os.walk(repo_path):
        if "package.xml" in files:
            package_name = os.path.basename(root)
            if contains_src_folder(root):
                packages_to_process.append({
                    "name": package_name,
                    "path": root
                })
    print(f"Found {len(packages_to_process)} packages with src folder:")
    print (f"Packages found in repository: {[pkg['name'] for pkg in packages_to_process]}")
    return packages_to_process

def process(package_name, repo_path):
    ros_capabilities = []
    print(f"Processing ROS package: {package_name} in repository: {repo_path}")

    packages_to_process = pkg_to_process(package_name, repo_path)
    print(f"Found {len(packages_to_process)} packages with src folder:")
    for pkg in packages_to_process:
        #if  pkg["name"]  != "opennav_following" :
        #    continue
        print(f"- {pkg['name']} at {pkg['path']}")
        package_parameters = get_parameters(pkg["name"], pkg["path"])
        events = get_events(pkg["name"], pkg["path"])

        llm_response = ask_llm(pkg['name'], events, package_parameters)

        #ros_capability = RosCapability(
        #    name=pkg["name"],
        #    parameters=package_parameters,
        #    events=events
        #)
        ros_capabilities.extend(llm_response)


    #print("\nExtracted ROS Capabilities:")
    #for capability in ros_capabilities:
    #    print(f"Capability: {capability.name}")
    #    print("Parameters:")
    #    for param in capability.parameters:
    #        print(f"  - {param.name} (type: {param.type}, default: {param.default_value})")
    #    print("Events:")
    #    if capability.events:
    #        for event in capability.events:
    #            print(f"  - {event}")
    #    print("\n")
    return ros_capabilities
