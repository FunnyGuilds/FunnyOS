#!/bin/bash

#
# Runs qemu using the pre-build FunnyOS image.
# Usage ./run-qemu.sh [FunnyOS.img location]
# The default location is ./FunnyOS.img
#
# Environmental variables:
# - FOS_QEMU: QEMU binary to use [Default: qemu-system-x86_64]
# - FOS_DEBUG_PARAMETERES: Debug parameters for QEMU. [Default: -s -S]
#

OUTPUT=${1:-./FunnyOS.img}
QEMU=${FOS_QEMU:-qemu-system-x86_64}
DEBUG_PARAMETERES=${FOS_DEBUG_PARAMETERES:--s -S}

if ! command -v $QEMU > /dev/null; then
  >&2 echo "ERROR: You must have $QEMU to run this script"
  exit 1
fi

qemu-system-x86_64 $DEBUG_PARAMETERES -hda $OUTPUT