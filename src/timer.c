#include "sys.h"

void phase(int hz)
{
	int div = 1193180 / hz;

	outb(0x43, 0x36);
	outb(0x40, div & 0xFF);
	outb(0x40, div >> 8);
}

int ticks = 0;
unsigned long ticker = 0;

void handle_timer(struct regs_t* r)
{
	ticks++;

	if (ticks % 18 == 0)
	{
		ticker++;
		puts("Tick...");
		if (ticker % 4 == 0)
			putc('|');
		else if (ticker % 4 == 1)
			putc('/');
		else if (ticker % 4 == 2)
			putc('-');
		else if (ticker % 4 == 3)
			putc('\\');

		putc('\n');
	}
}

void init_timer()
{
	install_irq_handler(0, handle_timer);
}

void wait(int wait_ticks)
{
	unsigned long eticks;
	eticks = ticks + wait_ticks;

	while (ticks < eticks);
}
