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

section .bss
	resb 8192
sys_stack:
