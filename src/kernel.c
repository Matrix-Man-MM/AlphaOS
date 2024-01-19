#include "sys.h"
#include "multiboot.h"
#include "ext2.h"

unsigned char* memcpy(unsigned char* dst, const unsigned char* src, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = src[i];

	return dst;
}

unsigned char* memset(unsigned char* dst, unsigned char val, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = val;

	return dst;
}

unsigned short* memsetw(unsigned short* dst, unsigned short val, int cnt)
{
	int i = 0;

	for (; i < cnt; i++)
		dst[i] = val;

	return dst;
}

int strlen(const char* str)
{
	int i = 0;

	while (str[i] != (char)0)
		i++;

	return i;
}

unsigned char inb(unsigned short port)
{
	unsigned char ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

void outb(unsigned short port, unsigned char data)
{
	asm volatile("outb %1, %0" :: "dN"(port), "a"(data));
}

struct multiboot_t* mb_copy(struct multiboot_t* mb_ptr)
{
	struct multiboot_t* new_header = (struct multiboot_t*)malloc(sizeof(struct multiboot_t));
	memcpy((unsigned char*)new_header, (unsigned char*)mb_ptr, sizeof(struct multiboot_t));
	return new_header;
}

void mb_dump(struct multiboot_t* mb_ptr)
{
	reset_text_color();
	set_text_color(2,0);
	printf("MULTIBOOT Header: 0x%x\r\n", (uintptr_t)mb_ptr);
	printf("MULTIBOOT Information\r\n--------------------------------\r\n");
	set_text_color(7,0);
	printf("Flags: 0x%x\t", mb_ptr->flags);
	printf("Mem Lo: 0x%x\t", mb_ptr->mem_lower);
	printf("Mem Hi: 0x%x\r\n", mb_ptr->mem_upper);
	printf("BootDev: 0x%x\t", mb_ptr->boot_device);
	printf("CmdLine: 0x%x\t", mb_ptr->cmdline);
	printf("Mods: 0x%x\r\n", mb_ptr->mods_count);
	printf("Addr: 0x%x\t", mb_ptr->mods_addr);
	printf("Syms: 0x%x\t", mb_ptr->num);
	printf("Syms: 0x%x\r\n", mb_ptr->size);
	printf("Syms: 0x%x\t", mb_ptr->addr);
	printf("Syms: 0x%x\t", mb_ptr->shndx);
	printf("MMap: 0x%x\r\n", mb_ptr->mmap_length);
	printf("Addr: 0x%x\t", mb_ptr->mmap_addr);
	printf("Drives: 0x%x\t", mb_ptr->drives_length);
	printf("Addr: 0x%x\r\n", mb_ptr->drives_addr);
	printf("Config: 0x%x\t", mb_ptr->config_table);
	printf("Loader: 0x%x\t", mb_ptr->boot_loader_name);
	printf("APM: 0x%x\r\n", mb_ptr->apm_table);
	printf("VBE Control: 0x%x\t", mb_ptr->vbe_control_info);
	printf("VBE Mode Info: 0x%x\t", mb_ptr->vbe_mode_info);
	printf("VBE Mode: 0x%x\r\n", mb_ptr->vbe_mode);
	printf("VBE Seg: 0x%x\t", mb_ptr->vbe_interface_seg);
	printf("VBE Off: 0x%x\t", mb_ptr->vbe_interface_off);
	printf("VBE Len: 0x%x\r\n", mb_ptr->vbe_interface_len);
	set_text_color(2,0);
	printf("--------------------------------\r\nEND MULTIBOOT RAW DATA\r\n");
	set_text_color(3,0);
	printf("Started with %s\r\n", mb_ptr->cmdline);
	printf("Booted from %s\r\n", mb_ptr->boot_loader_name);

	puts("\r\n");
	reset_text_color();

	printf("Lower Memory: %dKB\r\n", mb_ptr->mem_lower);
	printf("Upper Memory: %dKB\r\n", mb_ptr->mem_upper);

	int total_mem_mb = mb_ptr->mem_upper / 1024;
	printf("Total Memory: %dMB\r\n", total_mem_mb);

	printf("Found %d Modules!\r\n", mb_ptr->mods_count);
	if (mb_ptr->mods_count > 0)
	{
		int i;
		for (i = 0; i < mb_ptr->mods_count; ++i)
		{
			uint32_t mod_start = *((uint32_t*)mb_ptr->mods_addr + 8 * i);
			uint32_t mod_end = *(uint32_t*)(mb_ptr->mods_addr + 8 * i + 4);
			printf("Module %d is at 0x%x:0x%x\r\n", i+1, mod_start, mod_end);
		}
	}
}

int kernel_main(struct multiboot_t* mb_ptr) {
	uint32_t mod_start;
	uint32_t mod_end;

	if (mb_ptr->mods_count > 0)
	{
		mod_start = *((uint32_t*)mb_ptr->mods_addr);
		mod_end = *(uint32_t*)(mb_ptr->mods_addr+4);
		malloc_startat(mod_end);
	}

	mb_ptr = mb_copy(mb_ptr);

	// BOOT (Stage 1)
	init_gdt();
	init_idt();
	init_isrs();
	init_irq();
	init_vga();

	set_text_color(15,0);
	printf("[%s %s]\r\n", KERNEL_NAME, KERNEL_VERSION);
	init_paging(mb_ptr->mem_upper + 1024);
	mb_dump(mb_ptr);
	reset_text_color();

	// BOOT (Stage 2)
	init_timer();
	init_keyboard();

	printf("Kernel Booted!\r\n");

	init_heap();

	printf("Asking for %d\n", sizeof(int) * 50);
	int* ints = amalloc(sizeof(int) * 50);
	printf("LOL\n");
	int j;
	for (j = 0; j < 50; ++j)
		ints[j] = 50;

	printf("ints[23]: %d\n", ints[23]);
	char* chars = amalloc(sizeof(char) * 4);
	for (j = 0; j < 4; ++j)
		chars[j] = 'a';

	printf("chars[2]: %c\n", chars[2]);
	free(chars);
	free(ints);
	char* a_str = amalloc(sizeof(char) * 6);
	a_str[0] = 'H';
	a_str[1] = 'e';
	a_str[2] = 'l';
	a_str[3] = 'l';
	a_str[4] = 'o';
	a_str[5] = '\0';
	printf("a_str: %s\n", a_str);
	free(a_str);
	printf("freed a_str\n");
	a_str = amalloc(sizeof(char) * 6);
	a_str[0] = '1';
	a_str[1] = '2';
	a_str[2] = '3';
	a_str[3] = '\0';
	printf("a_str: %s\n", a_str);
	free(a_str);

	ext2_superblock_t* superblock = (ext2_superblock_t*)(mod_start + 1024);
	printf("Magic: 0x%x\r\n", (int)superblock->magic);
	kernel_assert(superblock->magic == EXT2_SUPER_MAGIC);

	printf("Partition\r\n----------------------");
	printf("Inodes: %d\tBlocks: %d\r\n", superblock->inodes_count, superblock->blocks_count);
	printf("Blocks Reserved For Root: %d\r\n", superblock->r_blocks_count);
	printf("Blocks Free: %d\r\n", superblock->free_blocks_count);
	printf("Free Inodes: %d\r\n", superblock->free_inodes_count);
	printf("Blocks contain %d bytes!\r\n", 1024 << superblock->log_block_size);
	printf("Fragments contain %d bytes!\r\n", 1024 << superblock->log_frag_size);
	printf("Block ID: %d\r\n", superblock->first_data_block);
	printf("Blocks In Group: %d\r\n", superblock->blocks_per_group);
	printf("Fragments In Group: %d\r\n", superblock->frags_per_group);
	printf("Inodes In Group: %d\r\n", superblock->inodes_per_group);
	printf("Last Mount: 0x%x\r\n", superblock->mtime);
	printf("Last Write: 0x%x\r\n", superblock->wtime);
	printf("Mounts Since Verification: %d\r\n", superblock->mnt_count);
	printf("Must Be Verified In %d Mounts!\r\n", superblock->max_mnt_count - superblock->mnt_count);
	printf("Inodes Are %d Bytes!\r\n", (int)superblock->inode_size);

	ext2_bgdescriptor_t* blockgroups = (ext2_bgdescriptor_t*)(mod_start + 1024 + 1024);
	printf("FIRST BLOCK GROUP\r\n---------------------------\r\n");
	printf("Free Blocks: %d\r\n", blockgroups->free_blocks_count);
	printf("Free Inodes: %d\r\n", blockgroups->free_inodes_count);
	printf("Used Dirs: %d\r\n", blockgroups->free_inodes_count);

	ext2_inodetable_t* inodetable = (ext2_inodetable_t*)(mod_start + (1024 << superblock->log_block_size) * blockgroups->inode_table);
	uint32_t i;
	for (i = 0; i < superblock->inodes_per_group; ++i)
	{
		ext2_inodetable_t* inode = (ext2_inodetable_t*)((int)inodetable + (int)superblock->inode_size * i);
		if (inode->block[0] == 0)
			continue;

		printf("INODE %d\r\n----------------------------------\r\n", i);
		printf("Starting Block: %d,%d\r\n", inode->block[0], inode->block[1]);
		printf("Bytes: %d\r\n", inode->size);
		printf("Blocks: %d\r\n", inode->blocks);

		if (inode->mode & EXT2_S_IFDIR)
		{
			printf("Is a directory!\r\n");
			printf("File Listing:\r\n");
			uint32_t dir_offset;
			dir_offset = 0;

			while (dir_offset < inode->size)
			{
				ext2_dir_t* d_ent = (ext2_dir_t*)(mod_start + (1024 << superblock->log_block_size) * inode->block[0] + dir_offset);
				unsigned char* name = amalloc(sizeof(unsigned char) * (d_ent->name_len + 1));
				memcpy(name, &d_ent->name, d_ent->name_len);
				name[d_ent->name_len] = '\0';
				printf("[%d] %s [%d]\r\n", dir_offset, name, d_ent->inode);

				if (name[0] == 'h' && name[1] == 'e' && name[2] == 'l' && name[3] == 'l' && name[4] == 'o')
				{
					printf("File Found!\r\n");
					ext2_inodetable_t* inode_f = (ext2_inodetable_t*)((int)inodetable + (int)superblock->inode_size * (d_ent->inode - 1));
					printf("Going to print %d bytes from block %d...\r\n", inode_f->size, inode_f->block[0]);
					unsigned char* file_ptr = (unsigned char*)(mod_start + (1024 << superblock->log_block_size) * inode_f->block[0]);

					unsigned int file_offset;
					for(file_offset = 0; file_offset < inode_f->size; ++file_offset)
						printf("%c", file_ptr[file_offset]);
				}

				free(name);
				dir_offset += d_ent->rec_len;
				if (d_ent->inode == 0)
					break;
			}

			break;
		}

		printf("\r\n");
	}


	return 0;
}
