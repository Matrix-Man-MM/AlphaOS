CC = gcc
CCFLAGS = -Wall -m32 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-PIC

AS = nasm
ASFLAGS = -f elf

LD = ld
LDFLAGS = -m elf_i386 -T link.ld

BOOTFILE = bin/boot.o
KERNELFILE = bin/kernel.o
VGAFILE = bin/vga.o
GDTFILE = bin/gdt.o
IDTFILE = bin/idt.o
ISRSFILE = bin/isrs.o
IRQFILE = bin/irq.o
TIMERFILE = bin/timer.o
OBJ = $(BOOTFILE) $(KERNELFILE) $(VGAFILE) $(GDTFILE) $(IDTFILE) $(ISRSFILE) $(IRQFILE) $(TIMERFILE)
OSFILE = bin/AlphaOS

VM = qemu-system-x86_64
VMFLAGS = -kernel $(OSFILE)

all: build run

build:
	mkdir -p bin
	$(AS) $(ASFLAGS) src/boot.asm -o $(BOOTFILE)
	$(CC) $(CCFLAGS) -c src/kernel.c -o $(KERNELFILE)
	$(CC) $(CCFLAGS) -c src/vga.c -o $(VGAFILE)
	$(CC) $(CCFLAGS) -c src/gdt.c -o $(GDTFILE)
	$(CC) $(CCFLAGS) -c src/idt.c -o $(IDTFILE)
	$(CC) $(CCFLAGS) -c src/isrs.c -o $(ISRSFILE)
	$(CC) $(CCFLAGS) -c src/irq.c -o $(IRQFILE)
	$(CC) $(CCFLAGS) -c src/timer.c -o $(TIMERFILE)
	$(LD) $(LDFLAGS) -o $(OSFILE) $(OBJ)

run:
	$(VM) $(VMFLAGS)

clean:
	rm -rf bin
