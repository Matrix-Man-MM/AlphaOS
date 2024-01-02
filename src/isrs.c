#include "sys.h"

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();

void init_isrs()
{
	idt_set_gate(0, (unsigned)isr_0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)isr_1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)isr_2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)isr_3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr_4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr_5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr_6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr_7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)isr_8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr_9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr_10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr_11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr_12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr_13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr_14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr_15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)isr_16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr_17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr_18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr_19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr_20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr_21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr_22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr_23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)isr_24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr_25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr_26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr_27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr_28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr_29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)isr_30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)isr_31, 0x08, 0x8E);
}

unsigned char* except_msgs[] =
{
	(unsigned char*)"DIVISION BY ZERO",
	(unsigned char*)"DEBUG",
	(unsigned char*)"NON-MASKABLE INTERRUPT",
	(unsigned char*)"BREAKPOINT",
	(unsigned char*)"DETECTED OVERFLOW",
	(unsigned char*)"OUT-OF-BOUNDS",
	(unsigned char*)"INVALID OPCODE",
	(unsigned char*)"NO COPROCESSOR",
	(unsigned char*)"DOUBLE FAULT",
	(unsigned char*)"COPROCESSOR SEGMENT OVERRUN",
	(unsigned char*)"BAD TSS",
	(unsigned char*)"SEGMENT NOT PRESENT",
	(unsigned char*)"STACK FAULT",
	(unsigned char*)"GENERAL PROTECTION FAULT",
	(unsigned char*)"PAGE FAULT",
	(unsigned char*)"UNKNOWN INTERRUPT",
	(unsigned char*)"COPROCESSOR FAULT",
	(unsigned char*)"ALIGNMENT CHECK",
	(unsigned char*)"MACHINE CHECK",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED",
	(unsigned char*)"RESERVED"
};

void handle_fault(struct regs_t* r)
{
	if (r->int_no < 32)
	{
		puts((char*)except_msgs[r->int_no]);
		puts(" EXCEPTION! System Halted...\r\n");
		for (;;);
	}
}
