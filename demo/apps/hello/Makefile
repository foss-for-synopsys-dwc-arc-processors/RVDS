### @file   Makefile
### @brief  RISC-V Virtuial Platform - S-mode MPU test application build script

TARGET    := $(notdir $(patsubst %/,%,$(CURDIR)))
OBJECTS   := crt0.o arch.o semihost.o printf.o tmon.o mmon.o smon.o mtwr0.o mtwr3.o stwr0.o stwr3.o mtvec.o stvec.o smpu.o main.o 

INCDIRS  := . ../../tmon/arch ../../tmon ../../slib
SRCDIRS  := ../../tmon ../../slib

SIM_PATH ?= ..
GNU_PATH ?= /opt/riscv-gnu-toolchain
LIB_PATH ?= $(GNU_PATH)/lib/gcc/riscv32-unknown-elf/13.2.0


AS = $(GNU_PATH)/bin/riscv32-unknown-elf-as
CC = $(GNU_PATH)/bin/riscv32-unknown-elf-gcc
LD = $(GNU_PATH)/bin/riscv32-unknown-elf-ld
DB = $(GNU_PATH)/bin/riscv32-unknown-elf-gdb
VP = $(SIM_PATH)/riscv-vp

CC_FLAGS = -c -g -Og -Wall -march=rv32i -misa-spec=2.2 -nostdlib -fno-strict-aliasing -mno-relax
AS_FLAGS = -g -march=rv32i -mabi=ilp32 -misa-spec=2.2
LD_FLAGS = -g -Og -T linker.ld --nostdlib --static --no-warn-rwx-segment -L$(LIB_PATH)


DB_FLAGS = -ex 'set arch riscv:rv32' -ex 'target remote :1234' -ex 'tui enable'
VP_FLAGS = --error-on-zero-traphandler=true

VPATH = src:$(SRCDIRS)

.PHONY: all run trs dbg clean

%.o: %.c
	$(CC) $(CC_FLAGS) $(addprefix -I,$(INCDIRS)) -o $@ $<

%.o: %.S
	$(AS) $(AS_FLAGS) -o $@ $<

dbg: all
	$(VP) $(VP_FLAGS) --debug-mode --input-file $(TARGET).elf &
	$(DB) $(DB_FLAGS) $(TARGET).elf

run: all
	$(VP) $(VP_FLAGS) --input-file $(TARGET).elf

trs: all
	$(VP) $(VP_FLAGS) --trace --input-file $(TARGET).elf

all: $(OBJECTS)
	$(LD) $(LD_FLAGS) $(^F) -lgcc -Map=linker.map -o $(TARGET).elf

clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.map
