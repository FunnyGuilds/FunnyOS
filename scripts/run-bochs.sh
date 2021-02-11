#!/bin/bash

#
# Runs bochs using the pre-build FunnyOS image.
# Usage ./run-bochs.sh [FunnyOS.img location]
# The default location is ./FunnyOS.img
#
# Environmental variables:
# - FOS_BOCHS_GUI_DEBUG: Should the GUI debugger be used. [Default: 0]
# - FOS_BOCHS_GUI_DEBUG_LIBRARY: GUI library to use if FOS_BOCHS_GUI_DEBUG is enabled . [Default: x]
#

OUTPUT=${1:-./FunnyOS.img}
DEBUG=${FOS_BOCHS_GUI_DEBUG:-0}
BOCHS_DISPLAY_LIBRARY=${FOS_BOCHS_GUI_DEBUG_LIBRARY:-x}
SCRIPT=./funnyos.bochsrc.tmp

if ! command -v bochs > /dev/null; then
  >&2 echo "ERROR: You must have bochs to run this script"
  exit 1
fi

if [[ ${DEBUG} -ne 0 ]]; then
  echo "display_library: $BOCHS_DISPLAY_LIBRARY, options=\"gui_debug\"" >> ${SCRIPT}
  echo "magic_break: enabled=1" >> ${SCRIPT}
fi

cat <<EOT >> ${SCRIPT}
clock: sync=realtime
memory: guest=1024, host=1024
ata0-master: type=disk, path=${OUTPUT}, mode=flat, cylinders=1024, heads=16, spt=32, translation=none
boot: disk
EOT

bochs -f ${SCRIPT} -q
rm ${SCRIPT}