#!/bin/bash

#
# Makes a FunnyOS disk image
# Usage ./make-image.sh [OUTPUT_IMAGE]
# Default OUTPUT_IMAGE is FunnyOS.img
#
# Environmental variables:
# - FOS_LOOP_DEVICE: Loop device to use for mouting the drive [Default: /dev/loop]
#

LOOP=${FOS_LOOP_DEVICE:-/dev/loop0}
OUTPUT=${1:-./FunnyOS.img}

PARTITION=${LOOP}p1
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

$DIR/require-root.sh || exit 1

if ! command -v sfdisk > /dev/null; then
  >&2 echo "ERROR: You must have sfdisk to run this script"
  exit 1
fi

if ! command -v mkfs.fat > /dev/null; then
  >&2 echo "You must have mkfs.fat to run this script"
fi

# Remove old image
rm -f $OUTPUT

# Create an empty image with 256 MB size
dd if=/dev/zero of=${OUTPUT} bs=1 count=0 seek=256M

# Format it
sfdisk $OUTPUT < $DIR/funnyos.sfdisk

# Mount the device
sudo losetup -d $LOOP 2> /dev/null
sudo losetup -P $LOOP $OUTPUT

# Format partition
sudo mkfs.fat -F 32 -R 32 $PARTITION

if command -v fatlabel > /dev/null; then
  sudo fatlabel $PARTITION "FunnyOS"
fi

# Override boot sector
sudo dd conv=notrunc if=./bootloader/bootsector/mbr/mbr.bin of=$LOOP bs=440 count=1

# Override first 3 bytes of Bios Parameter Block in the created partition
sudo dd conv=notrunc if=./bootloader/bootsector/stage1/stage1.bin of=$PARTITION bs=3 count=1

# Put stage1 code to the first sector of the partition
sudo dd conv=notrunc if=./bootloader/bootsector/stage1/stage1.bin of=$PARTITION bs=1 obs=1 skip=90 seek=90 count=422

# Put fat loader code to the third sector of the partition
sudo dd conv=notrunc if=./bootloader/bootsector/fat_loader/fat_loader.bin of=$PARTITION bs=512 obs=512 seek=2 count=16

# Mount the partition
MOUNT=$(mktemp -d /tmp/funnyos-mount-XXXXXXXX)
sudo mount $PARTITION $MOUNT
echo "Mouting at $MOUNT"

# System partition structure
sudo mkdir $MOUNT/boot
sudo mkdir $MOUNT/system
sudo cp ./bootloader/bootloader32/bootloader32.bin $MOUNT/boot/bootload32

if command -v fatattr > /dev/null; then
  echo "Setting FAT file attributes"
  sudo fatattr +rhs $MOUNT/BOOTLD32.BIN
fi

# Unmount
sudo umount $MOUNT
sudo losetup -d $LOOP