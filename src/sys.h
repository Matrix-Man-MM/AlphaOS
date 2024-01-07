#ifndef SYS_H
#define SYS_H

/* Types */
#define NULL ((void*)0UL)
typedef unsigned long uintptr_t;
typedef long size_t;
typedef unsigned int uint32_t;

/* Kernel Strings */
#define KERNEL_NAME "AlphaOS"
#define KERNEL_VERSION "0.0.0"

/* KERNEL */
extern unsigned char* memcpy(unsigned char* dst, const unsigned char* src, int cnt);
extern unsigned char* memset(unsigned char* dst, unsigned char val, int cnt);
extern unsigned short* memsetw(unsigned short* dst, unsigned short val, int cnt);
extern int strlen(const char* str);
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);

/* VGA */
extern void clear();
extern void putc(unsigned char c);
extern void puts(char* str);
extern void set_text_color(unsigned char fg, unsigned char bg);
extern void reset_text_color();
extern void init_vga();

/* GDT */
extern void init_gdt();
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

/* IDT */
extern void init_idt();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

/* Registers */
struct regs_t
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

/* ISRs */
extern void init_isrs();
extern void handle_fault(struct regs_t* r);

/* IRQ */
extern void init_irq();
extern void install_irq_handler(int irq, void* handler);
extern void uninstall_irq_handler(int irq);

/* TIMER */
extern void init_timer();
extern void wait();
extern int ticks;

/* KEYBOARD */
extern void init_keyboard();
extern void keyboard_wait();

/* PRINTF */
extern void printf(const char* format, ...);

#endif /* SYS_H */
