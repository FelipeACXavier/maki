#!/usr/bin/env bash

# To enable exit on error, add the 'e' option
set -uo pipefail

# ----------------------------------
# Flags
LTS=0
TRACE=0
KODA=0
ALL=0
RUN_DEZYNE=0
DEZYNE_COMMAND=0
SHOW_GRAPH=0

KODA_MAX_DISPATCHES=20
KODA_MAX_TRACES=100
DEZYNE_MAX_HOPS=100

# Comparison types
COMPARE=0
COMPARISON=""

# NUXMV stuff
RUN_NUXMV=0
NUXMV_ARGS=()

# Extras
TIME_COMMAND=()
TEST_NAME=""
OUTPUT=""
VERBOSE=""
CLEAN=0

# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1 --koda -t sequence"
  echo "  $1 --koda --trace --max-t 250 -t within"
  echo "  $1 --koda --trace --lts --compare macro --all"
  echo ""
  echo " Operations:"
  echo "        --koda          | Run KODA compiler"
  echo "        --lts           | Generate LTS for a test"
  echo "        --trace         | Generate KODA trace"
  echo "        --dezyne  <cmd> | Run Dezyne command"
  echo "                        | <cmd> can be: verify or simulate"
  echo "        --compare <cmp> | Run the comparison script"
  echo "                        | <cmp> must be one of: macro, xml"
  echo "    -g, --graph         | Show graph of state space"
  echo "        --nuxmv         | Run nuxmv script"
  echo ""
  echo " Operation options:"
  echo "    -a, --all           | Run operation for all tests"
  echo "    -t, --test <name>   | Run operation for specific test"
  echo "    -o, --output <file> | Set output file or folder"
  echo "        --time <secs>   | Time the operation and timeout in <secs>"
  echo ""
  echo " Trace options:"
  echo "        --max-d <n>     | Maximum number of dispatches when building trace"
  echo "        --max-t <n>     | Maximum number of traces to build"
  echo ""
  echo " Comparison options"
  echo "        --hops          | Maximum number of hops when comparing traces"
  echo ""
  echo " Generic:"
  echo "       --clean          | Clean a test folder"
  echo "    -v, --verbose       | Verbose output"
  echo "    -h, --help          | Prints this menu"
  echo ""
}

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --test | -t)
      TEST_NAME="$2"
      shift
      shift
      ;;
      --verbose | -v)
      VERBOSE="-vv"
      shift
      ;;
      --output | -o)
      OUTPUT="$2"
      shift
      shift
      ;;
      --nuxmv)
      RUN_NUXMV=1
      shift
      NUXMV_ARGS=("$@")
      break
      ;;
      --lts)
      LTS=1
      shift
      ;;
      --trace)
      TRACE=1
      shift
      ;;
      --max-d)
      KODA_MAX_DISPATCHES="$2"
      shift
      shift
      ;;
      --max-t)
      KODA_MAX_TRACES="$2"
      shift
      shift
      ;;
      --hops)
      DEZYNE_MAX_HOPS="$2"
      shift
      shift
      ;;
      --koda)
      KODA=1
      shift
      ;;
      --dezyne)
      RUN_DEZYNE=1
      DEZYNE_COMMAND="$2"
      shift
      shift
      ;;
      --graph | -g)
      SHOW_GRAPH=1
      shift
      ;;
      --compare)
      COMPARE=1
      COMPARISON="$2"
      shift
      shift
      ;;
      --all | -a)
      ALL=1
      shift
      ;;
      --clean)
      CLEAN=1
      shift
      ;;
      --time)
      TIME_COMMAND=(
        /usr/bin/time
        -f
        '"Wall: %e s | Memory: %M KB"'
        timeout $2
      )
      shift
      shift
      ;;
      --help|-h)
      printHelp $0
      exit 0
      ;;
      *)
      echo "Unknown option: $key"
      echo ""
      printHelp $0
      exit 0
      ;;
  esac
done

ROOT="$(git rev-parse --show-toplevel)" || exit 1
SOURCE_DIR="$ROOT/examples/CASE"
echo "Running in: $ROOT"
echo "Source folder: $SOURCE_DIR"

function run_lts() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local DEZYNE_FOLDER="$TEST_FOLDER/dezyne"
  local DEZYNE_LIB_FOLDER="$DEZYNE_FOLDER/lib"
  local BASE_NAME="${TEST}_test_task"
  local DEZYNE_FILE="$DEZYNE_FOLDER/$BASE_NAME.dzn"

  # We set a default folder
  if [ -z "$OUTPUT" ]; then
    local OUTPUT_FILE="$TEST_FOLDER/$BASE_NAME.aut"
  else
    local OUTPUT_FILE="$OUTPUT"
  fi

  local COMMAND="${TIME_COMMAND[@]} dzn -v lts -I $DEZYNE_FOLDER -I $DEZYNE_LIB_FOLDER $DEZYNE_FILE > $OUTPUT_FILE"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating LTS"
  echo "  Test: $TEST"
  echo "  Dezyne file: $DEZYNE_FILE"
  echo "  Aut file: $OUTPUT_FILE"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  eval $COMMAND
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_trace() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local TRACER_FOLDER="$TEST_FOLDER/tracer"
  local BASE_NAME="${TEST}_test"
  local TRACER_FILE="$TRACER_FOLDER/$BASE_NAME.reference.json"

  if [ -z "$OUTPUT" ]; then
    local OUTPUT_FILE="$TEST_FOLDER/$BASE_NAME.exploration.json"
  else
    local OUTPUT_FILE="$OUTPUT"
  fi

  local MAX_DISPATCHES="${KODA_MAX_DISPATCHES:-20}"
  local MAX_TRACES="${KODA_MAX_TRACES:-100}"

  local COMMAND="${TIME_COMMAND[@]} \
python -m koda_reference $TRACER_FILE \
--mode explore \
--max-dispatches $MAX_DISPATCHES \
--max-traces $MAX_TRACES \
--output $OUTPUT_FILE"

  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating trace"
  echo "  Test: $TEST"
  echo "  Trace file: $TRACER_FILE"
  echo "  Exploration file: $OUTPUT_FILE"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  local PY="$ROOT/koda-reference-python"
  export PYTHONPATH="$PY/src"

  eval $COMMAND
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_koda() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local KODA_COMPILER="$ROOT/build/linux/debug/plugins/koda_generatorplugin/bin/kodagen"

  local FILES=("$SOURCE_DIR"/test_*_"${TEST}".kd)
  local FILE="${FILES[0]}"
  if [[ ! -f "$FILE" ]]; then
    echo "Koda file '$TEST' not found"
    exit 1
  fi

  local KODA_INPUT="$FILE"
  local COMMAND="${TIME_COMMAND[@]} $KODA_COMPILER -i $KODA_INPUT -o $TEST_FOLDER $VERBOSE -e dezyne -e nuxmv -e tracer"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Running KODA compiler"
  echo "  Test: $TEST"
  echo "  Input file: $KODA_INPUT"
  echo "  Output file: $TEST_FOLDER"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  eval $COMMAND
  if [ $? -ne 0 ]; then
    echo "Compilation failed";
    exit -1
  fi
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_macro_compare() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local BASE_NAME="$(basename $TEST)_test"

  local MAX_HOPS="${DEZYNE_MAX_HOPS:-100}"
  local AUT_FILE="${TEST_FOLDER}/${BASE_NAME}_task.aut"
  local REFERENCE_FILE="${TEST_FOLDER}/${BASE_NAME}.exploration.json"
  local MODEL_FILE="${TEST_FOLDER}/tracer/${BASE_NAME}.reference.json"
  local MOCKS_FILE="${TEST_FOLDER}/tracer/${BASE_NAME}.default-mocks.json"

  if [ -z "$OUTPUT" ]; then
    local OUTPUT_FILE="$TEST_FOLDER/$BASE_NAME.comparison.json"
  else
    local OUTPUT_FILE="$OUTPUT"
  fi

  local COMMAND="${TIME_COMMAND[@]}\
python -m koda_reference.macro_compare \
--aut $AUT_FILE \
--reference $REFERENCE_FILE \
--model $MODEL_FILE \
--mocks $MOCKS_FILE \
--max-hops $MAX_HOPS \
--output $OUTPUT_FILE"

  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating trace"
  echo "  Test: $TEST"
  echo "  Aut file: $AUT_FILE"
  echo "  Reference file: $REFERENCE_FILE"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  local PY="$ROOT/koda-reference-python"
  export PYTHONPATH="$PY/src"

  eval $COMMAND
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_xml_compare() {
  local TEST="$1"
  local TEST_FOLDER="$2"
  local NUXMV_FOLDER="${TEST_FOLDER}/nuxmv"

  local BASE_NAME="$(basename $TEST)_test"

  local XML_FILE="${NUXMV_FOLDER}/${BASE_NAME}_main.trace.xml"
  local SMV_FILE="${NUXMV_FOLDER}/${BASE_NAME}_main.smv"
  local REFERENCE_FILE="${TEST_FOLDER}/${BASE_NAME}.exploration.json"
  local MODEL_FILE="${TEST_FOLDER}/tracer/${BASE_NAME}.reference.json"
  local MOCKS_FILE="${TEST_FOLDER}/tracer/${BASE_NAME}.default-mocks.json"

  if [ -z "$OUTPUT" ]; then
    local OUTPUT_FILE="$TEST_FOLDER/$BASE_NAME.comparison.json"
  else
    local OUTPUT_FILE="$OUTPUT"
  fi

  local COMMAND="${TIME_COMMAND[@]}\
python3 -m koda_reference.nuxmv_xml_compare \
--xml $XML_FILE \
--smv $SMV_FILE \
--mapping $ROOT/koda-reference-python/examples/nuxmv_xml_moon/simple_moon.mapping.json \
--reference $REFERENCE_FILE \
--output $OUTPUT_FILE"

  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating xml comparison"
  echo "  Test: $TEST"
  echo "  XML file: $XML_FILE"
  echo "  SMV file: $SMV_FILE"
  echo "  Reference file: $REFERENCE_FILE"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  local PY="$ROOT/koda-reference-python"
  export PYTHONPATH="$PY/src"

  eval $COMMAND
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_dezyne() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local DEZYNE_FOLDER="$TEST_FOLDER/dezyne"
  local DEZYNE_LIB_FOLDER="$DEZYNE_FOLDER/lib"
  local BASE_NAME="${TEST}_test_task"
  local DEZYNE_FILE="$DEZYNE_FOLDER/$BASE_NAME.dzn"

  local COMMAND="ide -v ${DEZYNE_COMMAND} -I $DEZYNE_FOLDER -I $DEZYNE_LIB_FOLDER $DEZYNE_FILE"
  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating LTS"
  echo "  Test: $TEST"
  echo "  Dezyne command: $DEZYNE_COMMAND"
  echo "  Dezyne file: $DEZYNE_FILE"
  echo "Running:"
  echo "  $COMMAND"
  echo "----------------------------------------"

  eval $COMMAND
  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function show_graph() {
  local TEST="$1"
  local TEST_FOLDER="$2"

  local DOT=$(command -v dot)
  local LTSCONVERT=$(command -v ltsconvert)
  if [ -z "$DOT" ] || [ -z "$LTSCONVERT" ]; then
    echo "dot and ltsconvert are needed to create the graph"
    exit -1
  fi

  local BASE_NAME="$(basename $TEST)_test"
  local AUT_FILE="${TEST_FOLDER}/${BASE_NAME}_task.aut"
  local DOT_FILE="${TEST_FOLDER}/${BASE_NAME}_graph.dot"
  local SVG_FILE="${TEST_FOLDER}/${BASE_NAME}_graph.svg"

  echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
  echo "Generating graph"
  echo "  Test: $TEST"
  echo "  Aut file: $AUT_FILE"
  echo "  Dot file: $DOT_FILE"
  echo "  SVG file: $SVG_FILE"
  echo "----------------------------------------"

  # We set a default folder
  ltsconvert $AUT_FILE $DOT_FILE
  if [ $? -ne 0 ]; then
    echo "Failed to generate dot file";
    exit -1
  fi
  dot -Tsvg $DOT_FILE -o $SVG_FILE
  if [ $? -ne 0 ]; then
    echo "Failed to generate svg file";
    exit -1
  fi

  xdg-open $SVG_FILE

  echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run_nuxmv() {
    local TEST="$1"
    local TEST_FOLDER="$2"

    local SMV_FILE="${TEST_FOLDER}/nuxmv/${TEST}_test_main.smv"

    if [[ ! -f "$SMV_FILE" ]]; then
        echo "Model not found: $SMV_FILE"
        return 1
    fi

    local NUXMV_SCRIPT="$ROOT/scripts/linux/nuxmv.sh"
    local COMMAND="${TIME_COMMAND[@]} $NUXMV_SCRIPT ${NUXMV_ARGS[@]} $SMV_FILE"

    echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
    echo "Runing NUXMV"
    echo "  Script: $NUXMV_SCRIPT"
    echo "  SMV: $SMV_FILE"
    echo "Running:"
    echo "  $COMMAND"
    echo "----------------------------------------"

    eval $COMMAND
    echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
}

function run() {
  if [ $SHOW_GRAPH -eq 1 ]; then
    show_graph "$1" "$2"
    exit 0;
  fi

  if [ $KODA -eq 1 ]; then
    run_koda "$1" "$2"
  fi

  if [ $LTS -eq 1 ]; then
    run_lts "$1" "$2"
  fi

  if [ $TRACE -eq 1 ]; then
    run_trace "$1" "$2"
  fi

  if [ $COMPARE -eq 1 ]; then
    if [[ "$COMPARISON" == "macro" ]]; then
      run_macro_compare "$1" "$2"
    elif [[ "$COMPARISON" == "xml" ]]; then
      run_xml_compare "$1" "$2"
    else
      echo "Unsupported comparison format: $COMPARISON"
      exit -1;
    fi
  fi

  if [[ $RUN_NUXMV -eq 1 ]]; then
    run_nuxmv "$1" "$2"
  fi

  if [ $RUN_DEZYNE -eq 1 ]; then
    run_dezyne "$1" "$2"
  fi

  # Add a blank line before returning
  echo ""
}

for FILE in ${SOURCE_DIR}/test_*.kd ; do
  BASE=$(basename $FILE)
  NUMBERED_NAME="${BASE#*_}";
  NUMBERED_NAME="${NUMBERED_NAME%.kd}";
  NAME="${NUMBERED_NAME#*_}";
  TEST_FOLDER="$(dirname $FILE)/$NUMBERED_NAME"

  if [ $ALL -eq 1 ]; then
    if [[ "$NAME" == *"_moon"* ]]; then
      echo "Skipping: $NAME"
      echo ""
      continue
    fi

    echo "Running test:"
    echo "  Name: $NAME"
    echo "  Folder: $TEST_FOLDER"
    echo ""
    run "$NAME" "$TEST_FOLDER"
    continue
  fi

  if [ -z "$TEST_NAME" ] ; then
    echo "Test not selected"
    printHelp $0
    exit -1
  fi

  if [[ "$TEST_NAME" == "$NAME" ]]; then
    echo "Running test:"
    echo "  Name: $NAME"
    echo "  Folder: $TEST_FOLDER"
    echo ""
    run "$NAME" "$TEST_FOLDER"
  fi
done

exit 0;
