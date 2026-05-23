
import os
import subprocess
import re
from dataclasses import dataclass, field
import json

@dataclass
class Parameter:
    name: str
    type: str 
    default_value: str | int | float | bool | list


@dataclass
class RosCapability:
    name: str

    parameters: list[Parameter] = field(default_factory=list)

    services: list[str] = field(default_factory=list)
    actions: list[str] = field(default_factory=list)

    publishers: list[str] = field(default_factory=list)
    subscribers: list[str] = field(default_factory=list)


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

def to_json(ros_cap):
    return {
        "type": ros_cap.name,
        "body": {
            "shape": "rounded rectangle",
            "textColor": "#FFFFFF",
            "backgroundColor": "#219ebc",
            "borderColor": "#FFFFFF",
            "borderRadius": 5,
            "width": 100,
            "height": 50,
            "scale": 0.5
        },
        "parameters": [
            {
                "name": param.name,
                "type": param.type,
                "default_value": param.default_value
            }
            for param in ros_cap.parameters
        ]
    }

def write_json(ros_capabilities):

    data = {
        "name": "ROS packages",
        "author": "Clara Rodrigues",
        "libraries": [
            {
                "type": "internal behaviour",
                "nodes": [to_json(cap) for cap in ros_capabilities]
            }
        ]
    }

    with open("../app/libraries/ros_capabilities.json", "w") as f:
        json.dump(data, f, indent=1)

def scan_package(package_name, package_path):
    print(f"Scanning ROS package: {package_name}")
    print(f"Package path: {package_path}")

    ros_capability = RosCapability(name=package_name)

    pattern = r'declare_or_get_parameter\s*\(\s*"([^"]+)"\s*,\s*(.*?)\s*\)'

    parameter_file = os.path.join(package_path, "src", "parameter_handler.cpp")
    if not os.path.exists(parameter_file):
        print(f"Parameter file not found: {parameter_file}")
        return
    

    with open(parameter_file, "r") as f:
        content = f.read()

    matches = re.findall(pattern, content)

    for name, default in matches:
        param_type = infer_type(default)
        default_value = parse_value(default)
        parameter = Parameter(name=name, type=param_type, default_value=default_value)
        ros_capability.parameters.append(parameter)

    return ros_capability


def contains_src_folder(path):
    for root, dirs, files in os.walk(path):
        if "src" in dirs:
            return True
    return False

def process(package_name, repo_path):

    packages_to_process = []
    ros_capabilities = []

    for root, dirs, files in os.walk(repo_path):
        if "package.xml" in files:
            package_name = os.path.basename(root)
            if contains_src_folder(root):
                packages_to_process.append({
                    "name": package_name,
                    "path": root
                })
    print(f"Found {len(packages_to_process)} packages with src folder:")
    for pkg in packages_to_process:
        print(f"- {pkg['name']} at {pkg['path']}")
        #scan_package(pkg["name"], pkg["path"])
    
    ros_capability = scan_package("opennav_following", "repos/nav2/nav2_following/opennav_following")
    ros_capabilities.append(ros_capability) 
    write_json(ros_capabilities)

    print("\nExtracted ROS Capabilities:")
    for capability in ros_capabilities:
        print(f"Capability: {capability.name}")
        print("Parameters:")
        for param in capability.parameters:
            print(f"  - {param.name} (type: {param.type}, default: {param.default_value})")
        print("Services:")
        for service in capability.services:
            print(f"  - {service}")
        print("Actions:")
        for action in capability.actions:
            print(f"  - {action}")
        print("Publishers:")
        for pub in capability.publishers:
            print(f"  - {pub}")
        print("Subscribers:")
        for sub in capability.subscribers:
            print(f"  - {sub}")
        print("\n")

            

def main():

    #git_link = input("Enter the git link of the ROS2 package: ")
#
    #package = input(
    #    "Enter the common denominator of the package "
    #    "(e.g., navigation2 -> nav2): "
    #)
#
    #base_dir = "repos"
    #os.makedirs(base_dir, exist_ok=True)
    #repo_path = os.path.join(base_dir, package)
#
    #print(f"Cloning repository into: {repo_path}")
#
    #subprocess.run(
    #    ["git", "clone", git_link, repo_path],
    #    check=True
    #)
#
    package = "nav2"
    repo_path = "repos/nav2"
    process(package, repo_path)

if __name__ == "__main__":
    main()
