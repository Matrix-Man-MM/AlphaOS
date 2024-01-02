[bits 32]

global _start
_start:
	mov esp, sys_stack
	jmp _stublet

align 4
_boot:
	MB_PAGE_ALIGN equ 1<<0
	MB_MEM_INFO equ 1<<1
	MB_AOUT_KLUDGE equ 1<<16
	MB_HEADER_MAGIC equ 0x1BADB002
	MB_HEADER_FLAGS equ MB_PAGE_ALIGN | MB_MEM_INFO | MB_AOUT_KLUDGE
	MB_CHECKSUM equ -(MB_HEADER_MAGIC + MB_HEADER_FLAGS)
	
	EXTERN code, bss, end

	dd MB_HEADER_MAGIC
	dd MB_HEADER_FLAGS
	dd MB_CHECKSUM

	dd _boot
	dd code
	dd bss
	dd end
	dd _start

_stublet:
	extern kernel_main
	call kernel_main
	jmp $

global flush_gdt
extern gdtptr
flush_gdt:
	lgdt [gdtptr]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:flush_two
flush_two:
	ret

global load_idt
extern idtptr
load_idt:
	lidt [idtptr]
	ret

global isr_0
global isr_1
global isr_2
global isr_3
global isr_4
global isr_5
global isr_6
global isr_7
global isr_8
global isr_9
global isr_10
global isr_11
global isr_12
global isr_13
global isr_14
global isr_15
global isr_16
global isr_17
global isr_18
global isr_19
global isr_20
global isr_21
global isr_22
global isr_23
global isr_24
global isr_25
global isr_26
global isr_27
global isr_28
global isr_29
global isr_30
global isr_31

isr_0:
	cli
	push byte 0
	push byte 0
	jmp isr_stub

isr_1:
	cli
	push byte 0
	push byte 1
	jmp isr_stub

isr_2:
	cli
	push byte 0
	push byte 2
	jmp isr_stub

isr_3:
	cli
	push byte 0
	push byte 3
	jmp isr_stub

isr_4:
	cli
	push byte 0
	push byte 4
	jmp isr_stub

isr_5:
	cli
	push byte 0
	push byte 5
	jmp isr_stub

isr_6:
	cli
	push byte 0
	push byte 6
	jmp isr_stub

isr_7:
	cli
	push byte 0
	push byte 7
	jmp isr_stub

isr_8:
	cli
	push byte 8
	jmp isr_stub

isr_9:
	cli
	push byte 0
	push byte 9
	jmp isr_stub

isr_10:
	cli
	push byte 10
	jmp isr_stub

isr_11:
	cli
	push byte 11
	jmp isr_stub

isr_12:
	cli
	push byte 12
	jmp isr_stub

isr_13:
	cli
	push byte 13
	jmp isr_stub

isr_14:
	cli
	push byte 14
	jmp isr_stub

isr_15:
	cli
	push byte 0
	push byte 15
	jmp isr_stub

isr_16:
	cli
	push byte 0
	push byte 16
	jmp isr_stub

isr_17:
	cli
	push byte 0
	push byte 17
	jmp isr_stub

isr_18:
	cli
	push byte 0
	push byte 18
	jmp isr_stub

isr_19:
	cli
	push byte 0
	push byte 19
	jmp isr_stub

isr_20:
	cli
	push byte 0
	push byte 20
	jmp isr_stub

isr_21:
	cli
	push byte 0
	push byte 21
	jmp isr_stub

isr_22:
	cli
	push byte 0
	push byte 22
	jmp isr_stub

isr_23:
	cli
	push byte 0
	push byte 23
	jmp isr_stub

isr_24:
	cli
	push byte 0
	push byte 24
	jmp isr_stub

isr_25:
	cli
	push byte 0
	push byte 25
	jmp isr_stub

isr_26:
	cli
	push byte 0
	push byte 26
	jmp isr_stub

isr_27:
	cli
	push byte 0
	push byte 27
	jmp isr_stub

isr_28:
	cli
	push byte 0
	push byte 28
	jmp isr_stub

isr_29:
	cli
	push byte 0
	push byte 29
	jmp isr_stub

isr_30:
	cli
	push byte 0
	push byte 30
	jmp isr_stub

isr_31:
	cli
	push byte 0
	push byte 31
	jmp isr_stub

extern handle_fault

isr_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, handle_fault
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret

section .bss
	resb 8192
sys_stack:
