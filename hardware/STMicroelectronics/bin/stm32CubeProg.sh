#!/bin/bash
set -o nounset # Treat unset variables as an error
STM32CP_CLI=STM32_Programmer.sh
ADDRESS=0x8000000
ERASE=""
MODE=""
PORT=""
OPTS=""

###############################################################################
## Help function
usage() {
  echo "############################################################"
  echo "##"
  echo "## $(basename "$0") <protocol> <file_path> [OPTIONS]"
  echo "##"
  echo "## protocol:"
  echo "##   0: SWD"
  echo "##   1: Serial"
  echo "##   2: DFU"
  echo "##   Note: prefix it by 1 to erase all sectors."
  echo "##         Ex: 10 erase all sectors using SWD interface."
  echo "## file_path: file path name to be downloaded: (bin, hex)"
  echo "## Options:"
  echo "##   For SWD and DFU: no mandatory options"
  echo "##   For Serial: <com_port>"
  echo "##     com_port: serial identifier (mandatory). Ex: /dev/ttyS0"
  echo "##"
  echo "## Note: all trailing arguments will be passed to the $STM32CP_CLI"
  echo "##   They have to be valid commands for STM32 MCU"
  echo "##   Ex: -g: Run the code at the specified address"
  echo "##       -rst: Reset system"
  echo "##       -s: start automatically (optional)"
  echo "############################################################"
  exit "$1"
}

check_tool() {
  if ! command -v $STM32CP_CLI > /dev/null 2>&1; then
    export PATH="$HOME/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin":$PATH
  fi
  if ! command -v $STM32CP_CLI > /dev/null 2>&1; then
    echo "$STM32CP_CLI not found."
    echo "Please install it or add '<STM32CubeProgrammer path>/bin' to your PATH environment:"
    echo "https://www.st.com/en/development-tools/stm32cubeprog.html"
    echo "Aborting!"
    exit 1
  fi
}

check_tool

if [ $# -lt 2 ]; then
  echo "Not enough arguments!"
  usage 2
fi

# Parse options
PROTOCOL=$1
FILEPATH=$2
# Protocol $1
# 1x: Erase all sectors
if [ "$1" -ge 10 ]; then
  ERASE="yes"
  PROTOCOL=$(($1 - 10))
fi
# Protocol $1
# 0: SWD
# 1: Serial
# 2: DFU
case $PROTOCOL in
  0)
    PORT="SWD"
    MODE="mode=UR"
    shift 2
    ;;
  1)
    if [ $# -lt 3 ]; then
      usage 3
    else
      PORT=$3
      shift 3
    fi
    ;;
  2)
    PORT="USB1"
    shift 2
    ;;
  *)
    echo "Protocol unknown!"
    usage 4
    ;;
esac

if [ $# -gt 0 ]; then
  OPTS="$*"
fi

${STM32CP_CLI} -c port=${PORT} ${MODE} ${ERASE:+"-e all"} -q -d "${FILEPATH}" ${ADDRESS} "${OPTS}"

exit $?
