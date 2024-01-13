#include "sys.h"

struct idt_entry
{
	unsigned short base_low;
	unsigned short sel;
	unsigned char zero;
	unsigned char flags;
	unsigned short base_high;
} __attribute__((packed));

struct idt_pointer
{
	unsigned short limit;
	uintptr_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_pointer idtptr;

extern void load_idt();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
	idt[num].base_low = (base & 0xFFFF);
	idt[num].base_high = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].zero = 0;
	idt[num].flags = flags;
}

void init_idt()
{
	idtptr.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtptr.base = (uintptr_t)&idt;
	memset((unsigned char*)&idt, 0, sizeof(struct idt_entry) * 256);

	load_idt();
}
