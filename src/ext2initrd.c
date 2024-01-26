#include "sys.h"
#include "ext2.h"
#include "vfs.h"

ext2_superblock_t* initrd_superblock;
ext2_inodetable_t* initrd_root_node;
ext2_bgdescriptor_t* initrd_root_block;
ext2_inodetable_t* initrd_inode_table;
void* initrd_start;

vfs_node_t* initrd_root;
vfs_node_t* initrd_dev;

uint32_t initrd_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, vfs_node_t* fnode);
uint32_t initrd_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t initrd_write(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
void initrd_open(vfs_node_t* node, uint8_t read, uint8_t write);
void initrd_close(vfs_node_t* node);
struct dirent* initrd_readdir(vfs_node_t* node, uint32_t index);
vfs_node_t* initrd_finddir(vfs_node_t* node, char* name);

ext2_dir_t* ext2_get_direntry(ext2_inodetable_t* inode, uint32_t index);
ext2_inodetable_t* ext2_get_inode(uint32_t inode);
void* ext2_get_block(uint32_t block);

uint32_t initrd_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);
	uint32_t end;

	if (offset + size > inode->size)
		end = inode->size;
	else
		end = offset + size;

	uint32_t size_to_read = end - offset;

	memcpy(buffer, ext2_get_block(inode->block[0]) + offset, size_to_read);
	return size_to_read;
}

uint32_t initrd_write(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	return 0;
}

void initrd_open(vfs_node_t* node, uint8_t read, uint8_t write)
{

}

void initrd_close(vfs_node_t* node)
{

}

struct dirent* initrd_readdir(vfs_node_t* node, uint32_t index)
{
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);
	ext2_dir_t* direntry = ext2_get_direntry(inode, index);
	if (!direntry)
		return NULL;

	struct dirent* dirent = amalloc(sizeof(struct dirent));
	memcpy(&dirent->name, &direntry->name, direntry->name_len);
	dirent->name[direntry->name_len] = '\0';
	dirent->ino = direntry->inode;
	return dirent;
}

vfs_node_t* initrd_finddir(vfs_node_t* node, char* name)
{
	ext2_inodetable_t* inode = ext2_get_inode(node->inode);
	void* block;
	ext2_dir_t* direntry = NULL;
	block = (void*)ext2_get_block(inode->block[0]);
	uint32_t dir_offset;
	dir_offset = 0;

	while (dir_offset < inode->size)
	{
		ext2_dir_t* d_ent = (ext2_dir_t*)((uintptr_t)block + dir_offset);
		if (strlen(name) != d_ent->name_len)
		{
			dir_offset += d_ent->rec_len;
			continue;
		}

		char* dname = amalloc(sizeof(char) * (d_ent->name_len + 1));
		memcpy(dname, &d_ent->name, d_ent->name_len);
		dname[d_ent->name_len] = '\0';

		if (!strcmp(dname, name))
		{
			free(dname);
			direntry = d_ent;
			break;
		}

		free(dname);
		dir_offset += d_ent->rec_len;

		if (!direntry)
			return NULL;

		vfs_node_t* outnode = amalloc(sizeof(vfs_node_t));
		initrd_node_from_file(ext2_get_inode(direntry->inode), direntry, outnode);
		return outnode;
	}
}

uint32_t initrd_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, vfs_node_t* fnode)
{
	if (!fnode)
		return 0;

	fnode->inode = direntry->inode;
	memcpy(&fnode->name, &direntry->name, direntry->name_len);
	fnode->name[direntry->name_len] = '\0';

	fnode->uid = inode->uid;
	fnode->gid = inode->gid;
	fnode->length = inode->size;
	fnode->mask = inode->mode & 0xFFF;

	fnode->flags = 0;

	if (inode->mode & EXT2_S_IFREG)
		fnode->flags |= VFS_FILE;

	if (inode->mode & EXT2_S_IFDIR)
		fnode->flags |= VFS_DIRECTORY;

	if (inode->mode & EXT2_S_IFBLK)
		fnode->flags |= VFS_BLOCKDEVICE;

	if (inode->mode & EXT2_S_IFCHR)
		fnode->flags |= VFS_CHARDEVICE;

	if (inode->mode & EXT2_S_IFIFO)
		fnode->flags |= VFS_PIPE;

	if (inode->mode & EXT2_S_IFLNK)
		fnode->flags |= VFS_SYMLINK;

	fnode->read = initrd_read;
	fnode->write = initrd_write;
	fnode->open = initrd_open;
	fnode->close = initrd_close;
	fnode->readdir = initrd_readdir;
	fnode->finddir = initrd_finddir;
	return 1;
}

uint32_t initrd_node_from_dirent(ext2_inodetable_t* inode, struct dirent* direntry, vfs_node_t* fnode)
{
	if (!fnode)
		return 0;

	fnode->inode = direntry->ino;
	memcpy(&fnode->name, &direntry->name, strlen(direntry->name));
	fnode->name[strlen(direntry->name)] = '\0';

	fnode->uid = inode->uid;
	fnode->gid = inode->gid;
	fnode->length = inode->size;
	fnode->mask = inode->mode & 0xFFF;

	fnode->flags = 0;

	if (inode->mode & EXT2_S_IFREG)
		fnode->flags |= VFS_FILE;

	if (inode->mode & EXT2_S_IFDIR)
		fnode->flags |= VFS_DIRECTORY;

	if (inode->mode & EXT2_S_IFBLK)
		fnode->flags |= VFS_BLOCKDEVICE;

	if (inode->mode & EXT2_S_IFCHR)
		fnode->flags |= VFS_CHARDEVICE;

	if (inode->mode & EXT2_S_IFIFO)
		fnode->flags |= VFS_PIPE;

	if (inode->mode & EXT2_S_IFLNK)
		fnode->flags |= VFS_SYMLINK;

	fnode->read = initrd_read;
	fnode->write = initrd_write;
	fnode->open = initrd_open;
	fnode->close = initrd_close;
	fnode->readdir = initrd_readdir;
	fnode->finddir = initrd_finddir;
	return 1;
}

ext2_inodetable_t* ext2_get_inode(uint32_t inode)
{
	return (ext2_inodetable_t*)((uintptr_t)initrd_inode_table + initrd_superblock->inode_size * (inode - 1));
}

void* ext2_get_block(uint32_t block)
{
	return (void*)((uintptr_t)initrd_start + (1024 << initrd_superblock->log_block_size) * block);
}

ext2_dir_t* ext2_get_direntry(ext2_inodetable_t* inode, uint32_t index)
{
	kernel_assert(inode->mode & EXT2_S_IFDIR);
	void* block;
	block = (void*)ext2_get_block(inode->block[0]);

	uint32_t dir_offset;
	dir_offset = 0;

	uint32_t dir_index;
	dir_index = 0;

	while (dir_offset < inode->size)
	{
		ext2_dir_t* d_ent = (ext2_dir_t*)((uintptr_t)block + dir_offset);
		if (dir_index == index)
			return d_ent;

		dir_offset += d_ent->rec_len;
		dir_index++;
	}

	return NULL;
}

void initrd_mount(uint32_t mem_head, uint32_t mem_top)
{
	initrd_start = (void*)mem_head;
	initrd_superblock = (ext2_superblock_t*)((uintptr_t)initrd_start + 1024);
	kernel_assert(initrd_superblock->magic == EXT2_SUPER_MAGIC);
	initrd_root_block = (ext2_bgdescriptor_t*)((uintptr_t)initrd_start + 1024 + 1024);
	initrd_inode_table = (ext2_inodetable_t*)((uintptr_t)initrd_start + (1024 << initrd_superblock->log_block_size) * initrd_root_block->inode_table);

	ext2_inodetable_t* root_inode = ext2_get_inode(2);
	ext2_dir_t* root_direntry = ext2_get_direntry(root_inode, 0);
	initrd_root = (vfs_node_t*)amalloc(sizeof(vfs_node_t));
	kernel_assert(initrd_node_from_file(root_inode, root_direntry, initrd_root));
	vfs_root = initrd_root;
}
