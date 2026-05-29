import requests
import json

OLLAMA_URL = "http://localhost:11434/api/generate"

MODEL = "qwen2.5-coder:7b"


def build_prompt(package_name, methods, parameters):

    return f"""
    You are analyzing a ROS2 package.

    Your task:
    1. Infer the high-level robot capabilities.
    2. Infer important runtime events.
    3. Infer which parameters are relevant to each capability.
    4. Ignore utility/internal/helper methods.
    5. Focus only on robot behavior and externally observable functionality.

    PACKAGE NAME:
    {package_name}

    METHODS:
    {methods}

    PARAMETERS:
    {parameters}

    Return ONLY valid JSON in this format:

    {{
      "type": "structure",
      "nodes": [
        {{
          "type": "{package_name}",
          "body": {{
            "shape": "Rectangle",
            "textColor": "#FFFFFF",
            "backgroundColor": "#DBB146",
            "borderColor": "#FFFFFF",
            "width": 500,
            "height": 500,
            "z-index": -2
          }},

          "parameters": [
            {{
              "name": "parameter_name",
              "type": "parameter_type",
              "default_value": "default_value"
            }}
          ],

          "events": [
            {{
              "id": "event_name",
              "type": "event_type",
              "return_type": "return_type",
              "modifiable": true
            }}
          ]
        }}
      ]
    }}
    """


def ask_llm(package_name, methods, parameters):

    prompt = build_prompt(
        package_name,
        methods,
        parameters
    )

    response = requests.post(
        OLLAMA_URL,
        json={
            "model": MODEL,
            "prompt": prompt,
            "stream": False,
            "format": "json"
        }
    )

    result = response.json()

    print("\n===== LLM RESPONSE =====\n")
    print(result["response"])

    parsed_response = json.loads(result["response"])

    return parsed_response["nodes"]