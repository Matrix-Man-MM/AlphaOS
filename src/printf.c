#include "sys.h"

#define args_list char*
#define _arg_stack_size(type) (((sizeof(type)-1)/sizeof(int)+1)*sizeof(int))
#define args_start(ap, fmt) do { ap = (char*)((unsigned int)&fmt + _arg_stack_size(&fmt)); } while (0)
#define args_end(ap)
#define args_next(ap, type) (((type*)(ap+=_arg_stack_size(type)))[-1])

static char buffer[1024] = {-1};
static int pointer = -1;

static void parse_num(unsigned int value, unsigned int base)
{
	unsigned int n = value / base;
	int r = value % base;

	if (r < 0)
	{
		r += base;
		--n;
	}

	if (value >= base)
		parse_num(n, base);

	buffer[pointer++] = (r + '0');
}

static void parse_hex(unsigned int value)
{
	int i = 8;

	while (i-- > 0)
		buffer[pointer++] = "0123456789abcdef"[(value>>(i*4))&0xF];
}

void printf(const char* format, ...)
{
	int i = 0;
	char* s;
	args_list args;
	args_start(args, format);
	pointer = 0;

	for (; format[i]; i++)
	{
		if ((format[i] != '%') && (format[i] != '\\'))
		{
			buffer[pointer++] = format[i];
			continue;
		}
		else if (format[i] == '\\')
		{
			switch (format[i++])
			{
				case 'a':
					buffer[pointer++] = '\a';
					break;
				case 'b':
					buffer[pointer++] = '\b';
					break;
				case 't':
					buffer[pointer++] = '\t';
					break;
				case 'n':
					buffer[pointer++] = '\n';
					break;
				case 'r':
					buffer[pointer++] = '\r';
					break;
				case '\\':
					buffer[pointer++] = '\\';
					break;
			}

			continue;
		}

		switch (format[++i])
		{
			case 's':
				s = (char*)args_next(args, char*);
				while (*s)
					buffer[pointer++] = *s++;

				break;
			case 'c':
				buffer[pointer++] = (char)args_next(args, int);
				break;
			case 'x':
				parse_hex((unsigned long)args_next(args, unsigned long));
				break;
			case 'd':
				parse_num((unsigned long)args_next(args, unsigned long), 10);
				break;
			case '%':
				buffer[pointer++] = '%';
				break;
			default:
				buffer[pointer++] = format[i];
				break;
		}
	}

	buffer[pointer] = '\0';
	args_end(args);
	puts(buffer);
}
