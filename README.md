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
./build.sh milkv-duo-sd
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

#### 4. Create the FIP Image(or use ./sd.sh, but modify the script to your mount point and location of your compiled fip.bin)
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


Currently, there is an error when attempting to read pmpcfg1, pmpcfg3, and pmpcfg4.
That being said, only pmpcfg0 and pmpcfg2 are readible (other have not tested yet)
```
sbi_trap_error: hart0: illegal instruction handler failed (error -2)
sbi_trap_error: hart0: mcause=0x0000000000000002 mtval=0x000000003a1027f3
sbi_trap_error: hart0: mepc=0x00000000802001f8 mstatus=0x8000000a01807800
sbi_trap_error: hart0: ra=0x0000000080200456 sp=0x00000000803fff90
sbi_trap_error: hart0: gp=0x0000000000000000 tp=0x0000000080020000
sbi_trap_error: hart0: s0=0x00000000803fffc0 s1=0x0000000080080000
sbi_trap_error: hart0: a0=0x0000000000000001 a1=0x0000000080080000
sbi_trap_error: hart0: a2=0x0000000000000007 a3=0x0000000000000019
sbi_trap_error: hart0: a4=0xfffffffffffffd2c a5=0x00000000802001f8
sbi_trap_error: hart0: a6=0x0000000000000001 a7=0x0000000000000005
sbi_trap_error: hart0: s2=0x0000000000000000 s3=0x8000000a01806000
sbi_trap_error: hart0: s4=0x0000000080200020 s5=0x0000000000000000
sbi_trap_error: hart0: s6=0x8000000a01806000 s7=0x000000008001b020
sbi_trap_error: hart0: s8=0x000000000000007f s9=0x000000008001b698
sbi_trap_error: hart0: s10=0x0000000000000000 s11=0x0000000000000000
sbi_trap_error: hart0: t0=0x0000000004140000 t1=0x0000000000000020
sbi_trap_error: hart0: t2=0x0000000000001000 t3=0x0000000000000010
sbi_trap_error: hart0: t4=0x0000000000000008 t5=0x000000008001fec0
sbi_trap_error: hart0: t6=0x0000000000000001

```