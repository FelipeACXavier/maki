#!/usr/bin/env bash
set -euo pipefail

FILES=()

USE_BMC=false
USE_MSAT=false
USE_COI=false
LIST_PROPS=false
PRINT_XML=false
USE_FULL_MODEL=false

PROPERTY=""
VERBOSITY=""
BOUND=""

function usage() {
  echo "Usage: $0 [options] <model>"
  echo
  echo "Options:"
  echo "  -o <file>   Output file"
  echo "  -b          Enable regular SAT-based BMC"
  echo "  -m          Enable MathSAT-based BMC"
  echo "  -c          Enable cone-of-influence reduction"
  echo "  -t          Test in a separate file"
  echo "  -n <num>    Property index"
  echo "  -l          List available properties"
  echo "  -v <num>    Set nuXmv verbosity"
  echo "  -k <num>    BMC bound"
  echo "  -x          Print XML format"
  echo "  -h          Show this help"
  echo
  echo "Examples:"
  echo "  $0 sequence"
  echo "  $0 -b -n 6 -k 10 sequence"
  echo "  $0 -m -n 6 -k 10 sequence"
  echo "  $0 -m -n 6 -k 10 -v 2 sequence"
}

while getopts ":bmclxX:n:v:k:h" opt; do
  case "$opt" in
    x)
      PRINT_XML=true
      ;;
    X)
      PRINT_XML=true
      USE_FULL_MODEL=true
      ;;
    b)
      USE_BMC=true
      ;;
    m)
      USE_MSAT=true
      ;;
    c)
      USE_COI=true
      ;;
    l)
      LIST_PROPS=true
      ;;
    n)
      PROPERTY="$OPTARG"
      ;;
    v)
      VERBOSITY="$OPTARG"
      ;;
    k)
      BOUND="$OPTARG"
      ;;
    h)
      usage
      exit 0
      ;;
    :)
      echo "Error: -$OPTARG requires an argument."
      usage
      exit 1
      ;;
    \?)
      echo "Error: unknown option -$OPTARG"
      usage
      exit 1
      ;;
  esac
done

shift $((OPTIND - 1))
if [[ $# -ne 1 ]]; then
  usage
  exit 1
fi

if [[ "$USE_BMC" == true && "$USE_MSAT" == true ]]; then
  echo "Error: -b and -m are mutually exclusive."
  exit 1
fi

if [[ -n "$BOUND" && "$USE_BMC" == false && "$USE_MSAT" == false && "$PRINT_XML" == false ]]; then
  echo "Error: -k requires either -b, -m, or -x."
  exit 1
fi

ROOT="$(git rev-parse --show-toplevel)" || exit 1

MODEL="${1%.smv}"
if [[ "$MODEL" = /* ]]; then
    MODEL_FILE="${MODEL}.smv"
else
    MODEL_FILE="${ROOT}/${MODEL}.smv"
fi

if [[ ! -f "$MODEL_FILE" ]]; then
  echo "Error: model file '$MODEL_FILE' does not exist."
  exit 1
fi

NUXMV_ARGS=()
# ------------------------------------------------------------------------------------
# List properties
if [[ "$LIST_PROPS" == true ]]; then
  NUXMV_ARGS+=("-lp")

# ------------------------------------------------------------------------------------
# Print the current SMV in XML format
elif [[ "$PRINT_XML" == true ]]; then
  CMD_FILE="${MODEL_FILE%.smv}.xml.cmd"
  XML_FILE="${MODEL_FILE%.smv}.trace.xml"
  XMI_FILE="${MODEL_FILE%.smv}.trace.xmi"

  # Check show_plugins for the correct XML plugin index.
  XML_PLUGIN="${NUXMV_XML_PLUGIN:-4}"
  SIM_STEPS="${BOUND:-40}"

  {
    echo "read_model -i $MODEL_FILE"

    if [[ -n "$VERBOSITY" ]]; then
      echo "set verbose_level $VERBOSITY"
    fi

    echo "go"

    # Generate an execution trace.
    if [[ "$USE_FULL_MODEL" == true ]]; then
      echo "write_xmi_model -F bdd -o $XMI_FILE"
    else
      # echo "show_plugins"
      echo "pick_state -r"
      # Trigger the task once.
      echo 'simulate -r -k 1 -t "command = CMD_TRIGGER"'

      # Afterward, allow capability events but no new task commands.
      if (( SIM_STEPS > 1 )); then
        echo "simulate -r -k $((SIM_STEPS - 1)) -t \"command = CMD_NONE\""
      fi

      # Export the execution to XML.
      echo "show_traces -p $XML_PLUGIN -o $XML_FILE"
    fi

    echo "quit"
  } > "$CMD_FILE"

  echo "Generating $CMD_FILE..."
  if [[ "$USE_FULL_MODEL" == true ]]; then
    echo "Exporting XMI model to $XMI_FILE"
  else
    echo "Exporting XML trace to $XML_FILE"
  fi
  echo ""

  systemd-run --user --scope --quiet \
    -p MemoryMax=6G \
    -p MemorySwapMax=1G \
    nuXmv -source "$CMD_FILE" \
    | egrep -v "^\*{3}" | egrep -v "^$"

  exit 0
# ------------------------------------------------------------------------------------
# MathSAT
elif [[ "$USE_MSAT" == true ]]; then
  CMD_FILE="${MODEL_FILE%.smv}.msat.cmd"

  echo "Generating $CMD_FILE..."

  {
    echo "read_model -i $MODEL_FILE"
    if [[ -n "$VERBOSITY" ]]; then
      echo "set verbose_level $VERBOSITY"
    fi
    if [[ "$USE_COI" == true ]]; then
      echo "set cone_of_influence"
    fi
    echo "go_msat"

    # CMD="msat_check_ltlspec_bmc"
    CMD="msat_check_ltlspec_sbmc_inc"
    if [[ -n "$PROPERTY" ]]; then
      CMD+=" -n $PROPERTY"
    fi
    if [[ -n "$BOUND" ]]; then
      CMD+=" -k $BOUND"
    fi

    echo "$CMD"
    echo "quit"
  } > "$CMD_FILE"

  echo
  echo "Running:"
  echo "nuXmv -source $CMD_FILE"
  echo

  systemd-run --user --scope --quiet \
    -p MemoryMax=6G \
    -p MemorySwapMax=1G \
    nuXmv -source "$CMD_FILE" \
    | egrep -v "^\*{3}" | egrep -v "^$"

  exit 0
# ------------------------------------------------------------------------------------
# Standard nuXmv / SAT-BMC
else
  if [[ "$USE_COI" == true ]]; then
    NUXMV_ARGS+=("-coi")
  fi

  if [[ "$USE_BMC" == true ]]; then
    NUXMV_ARGS+=("-bmc")
  fi

  if [[ -n "$PROPERTY" ]]; then
    NUXMV_ARGS+=("-n" "$PROPERTY")
  fi

  if [[ -n "$VERBOSITY" ]]; then
    NUXMV_ARGS+=("-v" "$VERBOSITY")
  fi

  if [[ -n "$BOUND" ]]; then
    NUXMV_ARGS+=("-bmc_length" "$BOUND")
  fi
fi


echo
echo "Running:"
printf 'nuXmv'
printf ' %q' "${NUXMV_ARGS[@]}" "$MODEL_FILE"
echo
echo

if [[ "$LIST_PROPS" == true ]]; then
  printf "%-5s %s\n" "ID" "NAME"
  systemd-run --user --scope --quiet \
    -p MemoryMax=6G \
    -p MemorySwapMax=1G \
    nuXmv "${NUXMV_ARGS[@]}" "$MODEL_FILE" \
    | egrep -v "^\*{3}" | egrep -v "^$" \
    | awk '/^[[:space:]]*[0-9]+[[:space:]]*:/ {id=$1; sub(/:$/, "", id)} /\[LTL/ {name=$NF; sub(/\]$/, "", name); printf "%-5s %s\n", id, name}'
else
  systemd-run --user --scope --quiet \
    -p MemoryMax=6G \
    -p MemorySwapMax=1G \
    nuXmv "${NUXMV_ARGS[@]}" "$MODEL_FILE" \
    | egrep -v "^\*{3}" | egrep -v "^$"
fi
