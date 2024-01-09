#include "sys.h"

void kernel_halt(char* error_msg, const char* file, int line)
{
	asm volatile("cli");
	set_text_color(14, 4);
	
	printf("PANIC! %s\r\n", error_msg);
	printf("File: %s\r\n", file);
	printf("Line: %d\r\n", line);

	for(;;);
}

void kernel_assert_failed(const char* file, uint32_t line, const char* desc)
{
	asm volatile("cli");
	set_text_color(14, 4);

	printf("ASSERTION FAILED! %s\r\n", desc);
	printf("File: %s\r\n", file);
	printf("Line: %d\r\n", line);

	for (;;);
}
