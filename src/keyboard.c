#include "sys.h"

struct key_states
{
	uint32_t shift : 1;
	uint32_t alt : 1;
	uint32_t ctrl : 1;
} key_states;

typedef void (*keyboard_handler_t)(int scan_code);

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

char us_keyboard_12[128] =
{
	0,27,
	'!','@','#','$','%','^','&','*','(',')',
	'_','+','\b',
	'\t',
	'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
	0, // CTRL
	'A','S','D','F','G','H','J','K','L',':','"','~',
	0, // LEFT SHIFT
	'|','Z','X','C','V','B','N','M','<','>','?',
	0, // RIGHT SHIFT
	'*',
	0, // ALT
	' ',
	0, // CAPS LOCK
	0, // F1
	0,0,0,0,0,0,0,0,
	0, // F10
	0, // NUM LOCK
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
	0, // PAGE DOWN
	0, // INSERT
	0, // DELETE
	0,0,0,
	0, // F11
	0, // F12
	0, // LITERALLY EVERYTHING ELSE
};

void norm_key(int scan_code)
{
	if (scan_code & 0x80)
		return;

	if (!us_keyboard[scan_code])
		return;

	if (key_states.shift)
		putc(us_keyboard_12[scan_code]);
	else if (key_states.ctrl)
	{
		putc('^');
		putc(us_keyboard_12[scan_code]);
	}
	else
		putc(us_keyboard[scan_code]);
}

void shift_key(int scan_code)
{
	key_states.shift ^= 1;
}

void alt_key(int scan_code)
{
	key_states.alt ^= 1;
}

void ctrl_key(int scan_code)
{
	key_states.ctrl ^= 1;
}

void func_key(int scan_code)
{

}

keyboard_handler_t key_method[] =
{
	NULL,NULL,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,
	norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,
	norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,
	norm_key,norm_key,norm_key,norm_key,norm_key,ctrl_key,norm_key,norm_key,
	norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,
	norm_key,norm_key,shift_key,norm_key,norm_key,norm_key,norm_key,norm_key,
	norm_key,norm_key,norm_key,norm_key,norm_key,norm_key,shift_key,norm_key,
	alt_key,norm_key,NULL,func_key,func_key,func_key,func_key,func_key,
	func_key,func_key,func_key,func_key,func_key,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,func_key,
	func_key,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};

void handle_keyboard(struct regs_t* r)
{
	unsigned char scan_code;
	scan_code = inb(0x60);

	keyboard_handler_t handler;
	handler = key_method[(int)scan_code & 0x7F];

	if (handler)
		handler(scan_code);
}

void init_keyboard()
{
	install_irq_handler(1, handle_keyboard);
}

void keyboard_wait()
{
	while(inb(0x64) & 2);
}
