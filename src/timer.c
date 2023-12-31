#include "sys.h"

void phase(int hz)
{
	int div = 1193180 / hz;

	outb(0x43, 0x36);
	outb(0x40, div & 0xFF);
	outb(0x40, (div >> 8) & 0xFF);
}

int ticks = 0;
unsigned long ticker = 0;

void handle_timer(struct regs_t* r)
{
	ticks++;

	if (ticks % 18 == 0)
	{
		ticker++;
	}
}

void init_timer()
{
	install_irq_handler(0, handle_timer);
	phase(100);
}

void wait(int wait_ticks)
{
	unsigned long eticks;
	eticks = ticks + wait_ticks;

	while (ticks < eticks);
}
