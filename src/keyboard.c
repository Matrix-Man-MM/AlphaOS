#include "sys.h"

char us_keyboard[128] =
{
	0,27,
	'1','2','3','4','5','6','7','8','9','0',
	'-','=','\b',
	'\t',
	'q','w','e','r','t','y','u','i','o','p','[',']','\n',
	0, // CTRL
	'a','s','d','f','g','h','j','k','l',';','\'','`',
	0, // LEFT SHIFT
	'\\','z','x','c','v','b','n','m',',','.','/',
	0, // RIGHT SHIFT
	'*',
	0, // ALT
	' ',
	0, // CAPS
	0, // F1
	0,0,0,0,0,0,0,0,
	0, // F10
	0, // NUM LOCK
	0, // SCROLL LOCK
	0, // HOME
	0, // UP
	0, // PAGEUP
	'-',
	0, // LEFT ARROW
	0,
	0, // RIGHT ARROW
	'+',
	0, // END
	0, // DOWN
	0, // PAGEDOWN
	0, // INS
	0, // DEL
	0,0,0,
	0, // F11
	0, // F12
	0, // LITERALLY EVERYTHING ELSE
};

void handle_keyboard(struct regs_t* r)
{
	unsigned char scan_code;
	scan_code = inb(0x60);

	if (scan_code & 0x80)
	{

	}
	else
		putc(us_keyboard[scan_code]);
}

void init_keyboard()
{
	install_irq_handler(1, handle_keyboard);
}

void keyboard_wait()
{
	while(inb(0x64) & 2);
}
