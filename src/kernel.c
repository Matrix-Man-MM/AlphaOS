#include "sys.h"

unsigned char* memcpy(unsigned char* dst, const unsigned char* src, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = src[i];

	return dst;
}

unsigned char* memset(unsigned char* dst, unsigned char val, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = val;

	return dst;
}

unsigned short* memsetw(unsigned short* dst, unsigned short val, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = val;

	return dst;
}

int strlen(const char* str)
{
	int i = 0;

	while (str[i] != (char)0)
		i++;

	return i;
}

unsigned char inb(unsigned short port)
{
	unsigned char ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

void outb(unsigned short port, unsigned char data)
{
	asm volatile("outb %1, %0" :: "dN"(port), "a"(data));
}

int kernel_main() {
	init_vga();
	puts("Welcome to AlphaOS!\r\n");

	init_gdt();
	puts("GDT Initialized!\r\n");

	init_idt();
	puts("IDT Initialized!\r\n");

	init_isrs();
	puts("ISRs Initialized!\r\n");

	init_irq();
	puts("IRQ Initialized!\r\n");

	for (;;);
	return 0;
}
