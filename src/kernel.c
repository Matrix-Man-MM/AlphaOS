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

struct multiboot_t* mb_copy(struct multiboot_t* mb_ptr)
{
	struct multiboot_t* new_header = (struct multiboot_t*)malloc(sizeof(struct multiboot_t));
	memcpy((unsigned char*)new_header, (unsigned char*)mb_ptr, sizeof(struct multiboot_t));
	return new_header;
}

void mb_dump(struct multiboot_t* mb_ptr)
{
	reset_text_color();
	set_text_color(2,0);
	printf("MULTIBOOT Header: 0x%x\r\n", (uintptr_t)mb_ptr);
	printf("MULTIBOOT Information\r\n--------------------------------\r\n");
	set_text_color(7,0);
	printf("Flags: 0x%x\t", mb_ptr->flags);
	printf("Mem Lo: 0x%x\t", mb_ptr->mem_lower);
	printf("Mem Hi: 0x%x\r\n", mb_ptr->mem_upper);
	printf("BootDev: 0x%x\t", mb_ptr->boot_device);
	printf("CmdLine: 0x%x\t", mb_ptr->cmdline);
	printf("Mods: 0x%x\r\n", mb_ptr->mods_count);
	printf("Addr: 0x%x\t", mb_ptr->mods_addr);
	printf("Syms: 0x%x\t", mb_ptr->num);
	printf("Syms: 0x%x\r\n", mb_ptr->size);
	printf("Syms: 0x%x\t", mb_ptr->addr);
	printf("Syms: 0x%x\t", mb_ptr->shndx);
	printf("MMap: 0x%x\r\n", mb_ptr->mmap_length);
	printf("Addr: 0x%x\t", mb_ptr->mmap_addr);
	printf("Drives: 0x%x\t", mb_ptr->drives_length);
	printf("Addr: 0x%x\r\n", mb_ptr->drives_addr);
	printf("Config: 0x%x\t", mb_ptr->config_table);
	printf("Loader: 0x%x\t", mb_ptr->boot_loader_name);
	printf("APM: 0x%x\r\n", mb_ptr->apm_table);
	printf("VBE Control: 0x%x\t", mb_ptr->vbe_control_info);
	printf("VBE Mode Info: 0x%x\t", mb_ptr->vbe_mode_info);
	printf("VBE Mode: 0x%x\r\n", mb_ptr->vbe_mode);
	printf("VBE Seg: 0x%x\t", mb_ptr->vbe_interface_seg);
	printf("VBE Off: 0x%x\t", mb_ptr->vbe_interface_off);
	printf("VBE Len: 0x%x\r\n", mb_ptr->vbe_interface_len);
	set_text_color(2,0);
	printf("--------------------------------\r\nEND MULTIBOOT RAW DATA\r\n");
	set_text_color(3,0);
	printf("Started with %s\r\n", mb_ptr->cmdline);
	printf("Booted from %s\r\n", mb_ptr->boot_loader_name);

	puts("\r\n");
	reset_text_color();

	printf("Lower Memory: %dKB\r\n", mb_ptr->mem_lower);
	printf("Upper Memory: %dKB\r\n", mb_ptr->mem_upper);

	int total_mem_mb = mb_ptr->mem_upper / 1024;
	printf("Total Memory: %dMB\r\n", total_mem_mb);
}

int kernel_main(struct multiboot_t* mb_ptr) {
	mb_ptr = mb_copy(mb_ptr);
	init_vga();
	init_gdt();
	init_idt();
	init_isrs();
	init_irq();
	set_text_color(15,0);
	printf("[%s %s]\r\n", KERNEL_NAME, KERNEL_VERSION);
	init_paging(mb_ptr->mem_upper + 1024);
	mb_dump(mb_ptr);
	init_timer();
	init_keyboard();

	printf("Kernel Booted!\r\n");

	return 0;
}
