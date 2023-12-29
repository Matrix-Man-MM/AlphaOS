CC = gcc
CCFLAGS = -Wall -m32 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin

AS = nasm
ASFLAGS = -f elf

LD = ld
LDFLAGS = -m elf_i386 -T link.ld

BOOTFILE = bin/boot.o
KERNELFILE = bin/kernel.o
OBJ = $(BOOTFILE) $(KERNELFILE)
OSFILE = bin/AlphaOS

VM = qemu-system-x86_64
VMFLAGS = -kernel $(OSFILE)

all: build run

build:
	mkdir -p bin
	$(AS) $(ASFLAGS) src/boot.asm -o $(BOOTFILE)
	$(CC) $(CCFLAGS) -c src/kernel.c -o $(KERNELFILE)
	$(LD) $(LDFLAGS) -o $(OSFILE) $(OBJ)

run:
	$(VM) $(VMFLAGS)

clean:
	rm -rf bin
