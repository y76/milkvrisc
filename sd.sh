#!/bin/bash

# Source and destination paths
SRC_PATH="./fsbl/build/cv1800b_milkv_duo_sd/fip.bin"
DEST_PATH="/media/pavel/boot/"

# Check if source file exists
if [ ! -f "$SRC_PATH" ]; then
    echo "Error: Source file $SRC_PATH does not exist!"
    exit 1
fi

# Check if destination directory is mounted
if [ ! -d "$DEST_PATH" ]; then
    echo "Error: Destination directory $DEST_PATH does not exist or is not mounted!"
    exit 1
fi

# Copy the file
echo "Copying fip.bin to SD card..."
cp "$SRC_PATH" "$DEST_PATH"

# Check if copy was successful
if [ $? -eq 0 ]; then
    echo "Success: fip.bin copied to $DEST_PATH"

    # Sync to ensure writes are completed
    sync
    echo "SD card synced. Safe to remove."
else
    echo "Error: Failed to copy fip.bin to $DEST_PATH"
    exit 1
fi
