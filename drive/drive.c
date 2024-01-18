#include <stdio.h>
#include <string.h>

void create_empty_drive()
{
	char data[512];
	FILE* file_ptr;
	file_ptr = fopen("drive.hdd", "w");

	int i;
	for (i = 0; i < 524288; i++)
	{
		memset(data, '\0', sizeof(data));
		fwrite(data, sizeof(data), 1, file_ptr);
	}

	fclose(file_ptr);
}

int main()
{
	create_empty_drive();

	return 0;
}

