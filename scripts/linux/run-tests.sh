set -euo pipefail

# ----------------------------------
# Helpers and arg parsing
function printHelp()
{
  echo "Usage:"
  echo "  $1"
  echo ""
  echo "    --unit           | Run unit tests"
  echo ""
}

UNIT_TESTS=0
INTEGRATION_TESTS=0
CURR_DIR=`pwd`
SOURCE_DIR=$CURR_DIR
BUILD_PATH="$SOURCE_DIR/build/linux/debug"
OPTIONS=""

while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
      --unit)
      UNIT_TESTS=1
      shift
      ;;
      --integration)
      INTEGRATION_TESTS=1
      shift
      ;;
      --list)
      OPTIONS="$OPTIONS --gtest_list_tests"
      shift
      ;;
      --no-logs)
      OPTIONS="$OPTIONS --no-logs"
      shift
      ;;
  esac
done

echo "--------------------------------------"
echo "Running with:"
echo "  UNIT_TESTS: ${UNIT_TESTS}"
echo "--------------------------------------"

# ----------------------------------
# Main
if [ $UNIT_TESTS -eq 1 ]; then
  $BUILD_PATH/app/tests/unit_tests/maki_unit_tests $OPTIONS
fi
if [ $INTEGRATION_TESTS -eq 1 ]; then
  $BUILD_PATH/app/tests/integration_tests/maki_integration_tests $OPTIONS
fi
