set -euo pipefail

# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1"
  echo ""
  echo "    --unit           | Run unit tests"
  echo "    --integration    | Run integration tests"
  echo "    --filter         | POSITIVE_PATTERNS[-NEGATIVE_PATTERNS]"
  echo "    --show           | Show test window"
  echo "    --no-logs        | Do not print extra logs"
  echo "    --ghelp          | Print gtest help menu"
  echo "    --help           | Print this help menu"
  echo ""
}

EXECUTABLE=""
CURR_DIR=`pwd`
SOURCE_DIR=$CURR_DIR
BUILD_PATH="$SOURCE_DIR/build/linux/debug"
OPTIONS=""
QT_ENV=(env QT_QPA_PLATFORM=offscreen)

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --unit)
      EXECUTABLE="$BUILD_PATH/app/tests/unit_tests/maki_unit_tests"
      shift
      ;;
      --integration)
      EXECUTABLE="$BUILD_PATH/app/tests/integration_tests/maki_integration_tests"
      shift
      ;;
      --list)
      OPTIONS="$OPTIONS --gtest_list_tests"
      shift
      ;;
      --filter)
      OPTIONS="$OPTIONS --gtest_filter=$2"
      shift
      shift
      ;;
      --no-logs)
      OPTIONS="$OPTIONS --no-logs"
      shift
      ;;
      --show)
      QT_ENV=()
      shift
      ;;
      -h|--help)
      printHelp $0
      exit 0
      ;;
      --ghelp)
      OPTIONS="--help"
      shift
      ;;
  esac
done

echo "--------------------------------------"
echo "Running with:"
echo "  EXECUTABLE: ${EXECUTABLE}"
echo "  OPTIONS: ${OPTIONS}"
echo "  QT_ENV: ${QT_ENV}"
echo "--------------------------------------"

# ----------------------------------
# Main
chmod +x $EXECUTABLE
"${QT_ENV[@]}" $EXECUTABLE $OPTIONS
