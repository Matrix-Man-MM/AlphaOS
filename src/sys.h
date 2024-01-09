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

/* KERNEL PANIC */
#define KERNEL_HALT(msg) kernel_halt(msg, __FILE__, __LINE__)
#define KERNEL_ASSERT(statement) ((statement) ? (void)0 : kernel_assert_failed(__FILE__, __LINE__, #statement))
void kernel_halt(char* error_msg, const char* file, int line);
void kernel_assert_failed(const char* file, uint32_t line, const char* desc);

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

typedef void (*irq_handler_t)(struct regs_t*);

/* ISRs */
extern void init_isrs();
extern void handle_fault(struct regs_t* r);
extern void install_isrs_handler(int isrs, irq_handler_t);
extern void uninstall_isrs_handler(int isrs);

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

/* MEMORY */
extern uintptr_t placement_ptr;
extern uintptr_t malloc_real(size_t size, int align, uintptr_t* phys);
extern uintptr_t malloc(size_t size);
extern uintptr_t vmalloc(size_t size);
extern uintptr_t malloc_p(size_t size, uintptr_t* phys);
extern uintptr_t vmalloc_p(size_t size, uintptr_t* phys);

typedef struct page
{
	uint32_t present : 1;
	uint32_t rw : 1;
	uint32_t user : 1;
	uint32_t accessed : 1;
	uint32_t dirty : 1;
	uint32_t unused : 7;
	uint32_t frame : 20;
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_dir
{
	page_table_t* tables[1024];
	uintptr_t physical_tables[1024];
	uintptr_t phsyical_addr;
} page_dir_t;

extern page_dir_t* kernel_dir;
extern page_dir_t* current_dir;

extern void init_paging(uint32_t memsize);
extern void switch_page_dir(page_dir_t* new_page_dir);
extern page_t* get_page(uintptr_t addr, int make, page_dir_t* dir);
extern void page_fault(struct regs_t* r);

#endif /* SYS_H */
