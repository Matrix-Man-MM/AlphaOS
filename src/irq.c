#include "sys.h"

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

void* irq_routines[16] =
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

void install_irq_handler(int irq, void* handler)
{
	irq_routines[irq] = handler;
}

void uninstall_irq_handler(int irq)
{
	irq_routines[irq] = 0;
}

void remap_irq()
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
}

void init_irq()
{
	remap_irq();

	idt_set_gate(32, (unsigned)irq_0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq_1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq_2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq_3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq_4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq_5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq_6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq_7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq_8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq_9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq_10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq_11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq_12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq_13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq_14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq_15, 0x08, 0x8E);
}

void handle_irq(struct regs_t* r)
{
	void (*handler)(struct regs_t* r);
	handler = irq_routines[r->int_no - 32];

	if (handler)
		handler(r);

	if (r->int_no >= 40)
		outb(0xA0, 0x20);

	outb(0x20, 0x20);
}
