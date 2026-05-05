# MAKI tools

This folder contains some tools that can be used to make either using or developing MAKI easier.

## Generate Doxygen

This is a simple python script that connects to an LLM and generates doxygen documentation for a header file.
 You should first create a virtual environment:
```bash
python3 -m venv env && source env/bin/activate
```

Then install the [ollama](https://pypi.org/project/ollama/) library:
```bash
pip install ollama
```

That is it. You can now run the script:

This will look for header files in the ../../app folder
```bash
python generate_doxygen.py \
  --model qwen2.5-coder:7b-instruct \
  --folder ../../app/ \
  --sleep 2
```
You can also ask it to run for a single header file:
```bash
python generate_doxygen.py \
  --model qwen2.5-coder:7b-instruct \
  --header ../../app/system/canvas.h \
  --sleep 2
```
The full command line options are shown below:
```bash
usage: generate_doxygen.py [-h] [--folder FOLDER] [--model MODEL] [--host HOST] [--dry-run]
                           [--no-backup] [--debug] [--no-stream] [--sleep SLEEP]
                           [--header HEADER] [--source SOURCE] [--patch-mode]
                           [--exclude EXCLUDE]

Add Doxygen comments to C++ headers using a local LLM.

options:
  -h, --help         show this help message and exit
  --folder FOLDER    Root folder to scan recursively.
  --model MODEL      Local model name. Default: mistral
  --host HOST        Ollama host. Default: http://localhost:11434
  --dry-run          Write .llm files instead of modifying headers.
  --no-backup        Do not create .bak files before modifying headers.
  --debug            Enable debug prints
  --no-stream        Do not stream model response
  --sleep SLEEP      Optional delay between requests.
  --header HEADER    Path to a specific header file.
  --source SOURCE    Path to a specific source file.
  --patch-mode       Ask the model for JSON comment insertions instead of rewriting the full header.
  --exclude EXCLUDE  Glob pattern to exclude. E.g. --exclude "*/build/*" --exclude "*_test.*"
```
