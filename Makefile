CC = gcc
CCFLAGS = -Wall -m32 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-PIC

AS = nasm
ASFLAGS = -f elf

LD = ld
LDFLAGS = -m elf_i386 -T link.ld

BOOTFILE = bin/boot.o
KERNELFILE = bin/kernel.o
VGAFILE = bin/vga.o
OBJ = $(BOOTFILE) $(KERNELFILE) $(VGAFILE)
OSFILE = bin/AlphaOS

VM = qemu-system-x86_64
VMFLAGS = -kernel $(OSFILE)

all: build run

build:
	mkdir -p bin
	$(AS) $(ASFLAGS) src/boot.asm -o $(BOOTFILE)
	$(CC) $(CCFLAGS) -c src/kernel.c -o $(KERNELFILE)
	$(CC) $(CCFLAGS) -c src/vga.c -o $(VGAFILE)
	$(LD) $(LDFLAGS) -o $(OSFILE) $(OBJ)

run:
	$(VM) $(VMFLAGS)

clean:
	rm -rf bin
