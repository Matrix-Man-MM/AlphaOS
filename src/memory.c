#include "sys.h"

typedef struct
{
	uint32_t magic;
	char is_hole;
	uint32_t size;
} header_t;

typedef struct
{
	uint32_t magic;
	header_t* header;
} footer_t;

extern uintptr_t end;
uintptr_t placement_ptr = (uintptr_t)&end;

page_dir_t* kernel_dir;
page_dir_t* current_dir;

uintptr_t malloc_real(size_t size, int align, uintptr_t* phys)
{
	if (align && (placement_ptr & 0xFFFFF000))
	{
		placement_ptr &= 0xFFFFF000;
		placement_ptr += 0x1000;
	}

	if (phys)
		*phys = placement_ptr;

	uintptr_t addr = placement_ptr;
	placement_ptr += size;
	return addr;
}

uintptr_t malloc(size_t size)
{
	return malloc_real(size, 0, NULL);
}

uintptr_t vmalloc(size_t size)
{
	return malloc_real(size, 1, NULL);
}

uintptr_t malloc_p(size_t size, uintptr_t* phys)
{
	return malloc_real(size, 0, phys);
}

uintptr_t vmalloc_p(size_t size, uintptr_t* phys)
{
	return malloc_real(size, 1, phys);
}

uint32_t* frames;
uint32_t nframes;

#define INDEX_FROM_BIT(b) (b / 0x20)
#define OFFSET_FROM_BIT(b) (b % 0x20)

static void set_frame(uintptr_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t index = INDEX_FROM_BIT(frame);
	uint32_t offset = OFFSET_FROM_BIT(frame);
	frames[index] |= (0x1 << offset);
}

static void clear_frame(uintptr_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t index = INDEX_FROM_BIT(frame);
	uint32_t offset = OFFSET_FROM_BIT(frame);
	frames[index] &= ~(0x1 << offset);
}

static uint32_t first_frame()
{
	uint32_t i;
	uint32_t j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
	{
		if (frames[i] != 0xFFFFFFFF)
		{
			for (j = 0; j < 32; j++)
			{
				uint32_t test_frame = 0x1 << j;
				if (!(frames[i] & test_frame))
					return i * 0x20 + j;
			}
		}
	}

	return -1;
}

void alloc_frame(page_t* page, int is_kernel, int is_writeable)
{
	if (page->frame)
		return;
	else
	{
		uint32_t index = first_frame();

		if (index == (uint32_t)-1)
			KERNEL_HALT("Failed To Allocate Frame -> OUT OF FRAMES!");

		set_frame(index * 0x1000);
		page->present = 1;
		page->rw = (is_writeable) ? 1 : 0;
		page->user = (is_kernel) ? 0 : 1;
		page->frame = index;
	}
}

void free_frame(page_t* page)
{
	uint32_t frame;

	if (!(frame = page->frame))
		return;
	else
	{
		clear_frame(frame);
		page->frame = 0x0;
	}
}

void init_paging(uint32_t memsize)
{
	nframes = memsize / 4;
	frames = (uint32_t*)malloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	kernel_dir = (page_dir_t*)vmalloc(sizeof(page_dir_t));
	memset(kernel_dir, 0, sizeof(page_dir_t));
	current_dir = kernel_dir;

	uint32_t i = 0;
	while (i < placement_ptr)
	{
		alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
		i += 0x1000;
	}

	install_isrs_handler(14, page_fault);
	switch_page_dir(kernel_dir);
}

void switch_page_dir(page_dir_t* dir)
{
	current_dir = dir;
	asm volatile("mov %0, %%cr3" :: "r"(&dir->physical_tables));
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_t* get_page(uintptr_t addr, int make, page_dir_t* dir)
{
	addr /= 0x1000;

	uint32_t table_index = addr / 1024;
	if (dir->tables[table_index])
		return &dir->tables[table_index]->pages[addr % 1024];
	else if (make)
	{
		uint32_t tmp;
		dir->tables[table_index] = (page_table_t*)vmalloc_p(sizeof(page_table_t), (uintptr_t*)(&tmp));
		memset(dir->tables[table_index], 0, 0x1000);
		dir->physical_tables[table_index] = tmp | 0x7;
		return &dir->tables[table_index]->pages[addr % 1024];
	}
	else
		return 0;
}

void page_fault(struct regs_t* r)
{
	uint32_t fault_addr;
	asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

	int present = !(r->err_code & 0x1);
	int rw = r->err_code & 0x2;
	int user = r->err_code & 0x4;
	int reserved = r->err_code & 0x8;
	int id = r->err_code & 0x10;

	printf("PAGE FAULT! p:%d,rw:%d,user:%d,res:%d,id:%d) at 0x%x\r\n", present, rw, user, reserved, fault_addr, id);
	KERNEL_HALT("Page Fault");
}
