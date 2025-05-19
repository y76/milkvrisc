## Milk-V Duo Custom Boot with M-mode Access
This repository contains code for running custom RISC-V programs in M-mode on the Milk-V Duo board, allowing direct access to PMP (Physical Memory Protection) and other privileged features.
Setup
Prerequisites

Milk-V Duo board
SD card
Serial console connection (baud rate: 115200)
duo-buildroot-sdk cloned and set up

```bash
unzip the milkv-duo-sd-v1.1.4.img.zip and flash the image onto an SD card
```
### Building and Running
#### 1. Initial FSBL Setup
If you haven't built the SDK before, run these commands to set up the environment and build the FSBL:
```bash
export MILKV_BOARD=milkv-duo
source milkv/boardconfig-milkv-duo.sh
source build/milkvsetup.sh
defconfig cv1800b_milkv_duo_sd
build_fsbl
```

#### 2. Modifying OpenSBI to Stay in M-mode
Whenever you want to change OpenSBI's behavior:

Edit the OpenSBI code:
```bash
opensbi/lib/sbi/sbi_hart.c
```
Rebuild OpenSBI:
```bash
cd opensbi
make PLATFORM=generic CROSS_COMPILE=riscv64-unknown-elf- FW_PAYLOAD=y FW_PAYLOAD_PATH=../bl33.bin
cd ..
```

Make sure output looks something like this -> 

```bash
make[1]: Leaving directory '/home/pavel/apps/milkvrisc/fsbl'
cp /home/pavel/apps/milkvrisc/fsbl/build/cv1800b_milkv_duo_sd/fip.bin /home/pavel/apps/milkvrisc/install/soc_cv1800b_milkv_duo_sd/
cp /home/pavel/apps/milkvrisc/fsbl/build/cv1800b_milkv_duo_sd/fip.bin /home/pavel/apps/milkvrisc/install/soc_cv1800b_milkv_duo_sd/fip_spl.bin
```

#### 3. Compile Your Test Program (or use ./compile.sh)
```bash
Compile the assembly file
riscv64-unknown-elf-gcc -c -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -g -Wall start.S -o start.o

# Compile the C file
riscv64-unknown-elf-gcc -c -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -mcmodel=medany -g -Wall main.c -o main.o

# Link everything together
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -march=rv64gc -mabi=lp64f -mcmodel=medany -g -Wall -T linker.ld start.o main.o -o bl33.elf

# Create binary
riscv64-unknown-elf-objcopy -O binary bl33.elf bl33.bin
```

#### 4. Create the FIP Image(or use ./sd.sh, but modify the script to your mount point)
```bash
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
```

#### 5. Copy to SD Card

#### 6. Monitor Serial
```bash
picocom -b 115200 /dev/ttyACM0
```
![Screenshot_20250515_161838](https://github.com/user-attachments/assets/94cc7921-e377-447a-a9f1-290a20d29837)
![Pasted image 20250513202806](https://github.com/user-attachments/assets/c56f8e16-0f94-4aa6-87b2-6a22dccd89ec)
