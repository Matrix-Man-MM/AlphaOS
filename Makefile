CC = gcc
CCFLAGS = -Wall -Wextra -m32 -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-PIC -fno-stack-protector

AS = nasm
ASFLAGS = -f elf

LD = ld
LDFLAGS = -m elf_i386 -T link.ld

CFILES = $(wildcard src/*.c)
ASMFILES = $(wildcard src/*.asm)
OBJS = $(patsubst src/%.asm,bin/%.o,$(ASMFILES)) $(patsubst src/%.c,bin/%.o,$(CFILES))

OSFILE = bin/AlphaOS

VM = qemu-system-x86_64
VMFLAGS = -kernel $(OSFILE) -m 2048M

all: build run

build: dir $(OBJS)
	$(LD) $(LDFLAGS) -o $(OSFILE) $(OBJS)

bin/%.o: src/%.c
	$(CC) $(CCFLAGS) -c $< -o $@

bin/%.o: src/%.asm
	$(AS) $(ASFLAGS) $< -o $@

run:
	$(VM) $(VMFLAGS)

dir:
	mkdir -p bin

clean:
	rm -rf bin
