#!/bin/bash

# Compile the assembly file
echo "Compiling assembly..."
riscv64-unknown-elf-gcc -c -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -g -Wall start.S -o start.o

# Compile the C file
echo "Compiling C code..."
riscv64-unknown-elf-gcc -c -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -mcmodel=medany -g -Wall main.c -o main.o

# Link everything together
echo "Linking..."
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -mcmodel=medany -g -Wall -T linker.ld start.o main.o -o bl33.elf

# Create binary
echo "Creating binary..."
riscv64-unknown-elf-objcopy -O binary bl33.elf bl33.bin

echo "Compilation complete. bl33.bin created."

# Create fip.bin
echo "Creating fip.bin..."
cd fsbl/
./plat/cv180x/fiptool.py -v genfip \
    'build/cv1800b_milkv_duo_sd/fip.bin' \
    --MONITOR_RUNADDR="0x0000000080000000" \
    --CHIP_CONF='build/cv1800b_milkv_duo_sd/chip_conf.bin' \
    --NOR_INFO='FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF' \
    --NAND_INFO='00000000'\
    --BL2='build/cv1800b_milkv_duo_sd/bl2.bin' \
    --DDR_PARAM='test/cv181x/ddr_param.bin' \
    --MONITOR='../opensbi/build/platform/generic/firmware/fw_dynamic.bin' \
    --LOADER_2ND='../bl33.bin' \
    --compress='lzma'
cd ..

echo "fip.bin created at fsbl/build/cv1800b_milkv_duo_sd/fip.bin"
echo "Complete! Copy the fip.bin to your SD card to run."
