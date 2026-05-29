
import os
import subprocess
import json
from parameter_getter import process, Parameter
from dataclasses import dataclass, field


@dataclass
class RosCapability:
    name: str

    parameters: list[Parameter] = field(default_factory=list)

    services: list[str] = field(default_factory=list)
    actions: list[str] = field(default_factory=list)

    publishers: list[str] = field(default_factory=list)
    subscribers: list[str] = field(default_factory=list)

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

    with open("../../app/libraries/ros_capabilities.json", "w") as f:
        json.dump(data, f, indent=1)

            
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
    repo_path = "../repos/nav2"
    ros_capabilities = process(package, repo_path)

    write_json(ros_capabilities)

if __name__ == "__main__":
    main()
