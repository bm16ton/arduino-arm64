#!/bin/bash
# $Id: dfuprog.sh,v 1.12 2013/09/30 14:18:38 claudio Exp $

#set -o verbose

cp /usr/local/bin/iram_dfu_util_spiflash.bin.hdr ./

if [ "z$1" == "z" ]; then
	echo "Missing argument: file to program"
	exit 1
#	FILENAME=BootMonitor_15600CPU.bin
else
	FILENAME="$1"
fi

QUIETMODE=0
if [ "z$2" != "z" ]; then
	if [ "$2" == "-quiet" ]; then
		QUIETMODE=1
		LOGPATH=""
	else
		LOGPATH="$2"
	fi
else
	LOGPATH="."
fi

if ! which dfu-util >/dev/null; then
	echo "dfu-util not found!"
	exit 1
fi

#Check for minimum required version
DFU_UTIL_VERMIN=0.6
DFU_UTIL_VERSION=`dfu-util --version | head -n 1 | cut -d ' ' -f 2 | cut -d '.' -f 1-2`

DFU_UTIL_VER_INT=`echo "scale=1; ${DFU_UTIL_VERSION}*100.0" | bc | cut -d '.' -f 1`
DFU_UTIL_VERMIN_INT=`echo "scale=1; ${DFU_UTIL_VERMIN}*100.0" | bc | cut -d '.' -f 1`

if [ ${DFU_UTIL_VERMIN_INT} -gt ${DFU_UTIL_VER_INT} ]; then
	echo "!  dfu-util version = ${DFU_UTIL_VERSION} (${DFU_UTIL_VER_INT}), Required = ${DFU_UTIL_VERMIN} (${DFU_UTIL_VERMIN_INT})"
	exit 1
fi

#export LD_LIBRARY_PATH="$HOME/lib"
#echo "LDPath=$LD_LIBRARY_PATH"
LPCDFU=`which lpcdfu`
if [ "z${LPCDFU}" == "z" ]; then
	LPCDFU="Release/lpcdfu"
fi
if [ ! -x "${LPCDFU}" ]; then
	echo "lpcdfu not found!"
	exit 1
fi

set -o errexit

echo "Downloading algo..."
dfu-util -d 1fc9:000c -t 2048 -R -D iram_dfu_util_spiflash.bin.hdr
rm -f readback.bin
echo "Programming ${FILENAME}"
if [ "${QUIETMODE}" == "1" ]; then
	${LPCDFU} -d 3 -e -D "${FILENAME}" -U -L "/doesn't exist/lpcdfu_log.txt" -C /dev/null
else
	${LPCDFU} -d 3 -e -D "${FILENAME}" -U -L "${LOGPATH}/lpcdfu_log.txt" -C "${LOGPATH}/command_log.txt"
fi

if [ -f readback.bin ]; then
	if cmp readback.bin "${FILENAME}"; then
		echo "Verify OK"
		rm ./command_log.txt ./iram_dfu_util_spiflash.bin.hdr ./lpcdfu_log.txt ./readback.bin
		exit 0
	else
		echo "file readback.bin and ${FILENAME} differs"
	fi
else
	echo "readback.bin not found!!"
fi
rm command_log.txt iram_dfu_util_spiflash.bin.hdr lpcdfu_log.txt readback.bin
exit 1
