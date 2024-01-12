#include "sys.h"

unsigned short* textmemptr;
int attribute = 0x0F;
int cursor_x = 0;
int cursor_y = 0;

void scroll()
{
	unsigned blank, tmp;
	blank = 0x20 | (attribute << 8);

	if (cursor_y >= 25)
	{
		tmp = (cursor_y - 25) + 1;
		
		memcpy(textmemptr, textmemptr + tmp * 80, (25 - tmp) * 80 * 2);
		memsetw(textmemptr + (25 - tmp) * 80, blank, 80);

		cursor_y = 25 - 1;
	}
}

void move_cursor()
{
	unsigned tmp;
	tmp = cursor_y * 80 + cursor_x;

	outb(0x3D4, 14);
	outb(0x3D5, tmp >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, tmp);
}

void clear()
{
	unsigned blank;
	int i;
	blank = 0x20 | (attribute << 8);

	for (i = 0; i < 25; i++)
		memsetw(textmemptr + i * 80, blank, 80);

	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

void putc(unsigned char c)
{
	unsigned short* loc;
	unsigned att = attribute << 8;

	if (c == 0x08)
	{
		if (cursor_x != 0)
			cursor_x--;
	}
	else if (c == 0x09)
		cursor_x = (cursor_x + 8) & ~(8-1);
	else if (c == '\r')
		cursor_x = 0;
	else if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	else if (c >= ' ')
	{
		loc = textmemptr + (cursor_y * 80 + cursor_x);
		*loc = c | att;
		cursor_x++;
	}

	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y++;
	}

	scroll();
	move_cursor();
}

void puts(char* str)
{
	int i;
	int len = strlen(str);

	for (i = 0; i < len; i++)
		putc(str[i]);
}

void set_text_color(unsigned char fg, unsigned char bg)
{
	attribute = (bg >> 4) | (fg & 0x0F);
}

void reset_text_color()
{
	set_text_color(15,0);
}

void init_vga()
{
	textmemptr = (unsigned short*)0xB8000;

	cursor_y = 10;
	move_cursor();
}
