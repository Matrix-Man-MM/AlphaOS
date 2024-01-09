#include "sys.h"
#include "multiboot.h"

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

int kernel_main(struct multiboot_t* mb_ptr) {
	init_vga();
	init_gdt();
	init_idt();
	init_isrs();
	init_irq();
	init_paging(mb_ptr->mem_upper);
	init_timer();
	init_keyboard();

	set_text_color(15,0);
	printf("[%s %s]\r\n", KERNEL_NAME, KERNEL_VERSION);
	set_text_color(2,0);

	printf("MULTIBOOT Information\r\n--------------------------------\r\n");
	set_text_color(7,0);
	printf("Flags: %x\t", mb_ptr->flags);
	printf("Mem Lo: %x\t", mb_ptr->mem_lower);
	printf("Mem Hi: %x\r\n", mb_ptr->mem_upper);
	printf("BootDev: %x\t", mb_ptr->boot_device);
	printf("CmdLine: %x\t", mb_ptr->cmdline);
	printf("Mods: %x\r\n", mb_ptr->mods_count);
	printf("Addr: %x\t", mb_ptr->mods_addr);
	printf("Syms: %x\t", mb_ptr->num);
	printf("Syms: %x\r\n", mb_ptr->size);
	printf("Syms: %x\t", mb_ptr->addr);
	printf("Syms: %x\t", mb_ptr->shndx);
	printf("MMap: %x\r\n", mb_ptr->mmap_length);
	printf("Addr: %x\t", mb_ptr->mmap_addr);
	printf("Drives: %x\t", mb_ptr->drives_length);
	printf("Addr: %x\r\n", mb_ptr->drives_addr);
	printf("Config: %x\t", mb_ptr->config_table);
	printf("Loader: %x\t", mb_ptr->boot_loader_name);
	printf("APM: %x\r\n", mb_ptr->apm_table);
	printf("VBE Control: %x\t", mb_ptr->vbe_control_info);
	printf("VBE Mode Info: %x\t", mb_ptr->vbe_mode_info);
	printf("VBE Mode: %x\r\n", mb_ptr->vbe_mode);
	printf("VBE Seg: %x\t", mb_ptr->vbe_interface_seg);
	printf("VBE Off: %x\t", mb_ptr->vbe_interface_off);
	printf("VBE Len: %x\r\n", mb_ptr->vbe_interface_len);
	set_text_color(2,0);
	printf("--------------------------------\r\nEND MULTIBOOT RAW DATA\r\n");
	set_text_color(3,0);
	printf("Started with %s\r\n", (char*)mb_ptr->cmdline);
	printf("Booted from %s\r\n", (char*)mb_ptr->boot_loader_name);

	reset_text_color();
	printf("Testing VGA Text Colors...\r\n");
	for (int i = 0; i < 16; i++)
	{
		set_text_color(i,i);
		putc('A');
	}
	puts("\r\n");
	reset_text_color();

	printf("Lower Memory: %dKB\r\n", mb_ptr->mem_lower);
	printf("Upper Memory: %dKB\r\n", mb_ptr->mem_upper);

	int total_mem_mb = mb_ptr->mem_upper / 1024;
	printf("Total Memory: %dMB\r\n", total_mem_mb);

	printf("Kernel Booted!\r\n");

	return 0;
}
