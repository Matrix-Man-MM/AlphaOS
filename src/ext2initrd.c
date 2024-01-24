#include "sys.h"
#include "ext2.h"
#include "vfs.h"

ext2_superblock_t* initrd_superblock;
ext2_inodetable_t* initrd_root_node;

vfs_node_t* initrd_root;
vfs_node_t* initrd_dev;

uint32_t initrd_node_from_file(ext2_inodetable_t* inode, ext2_dir_t* direntry, vfs_node_t* fnode);
uint32_t initrd_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t initrd_write(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
void initrd_open(vfs_node_t* node, uint8_t read, uint8_t write);
void initrd_close(vfs_node_t* node);
struct dirent* initrd_readdir(vfs_node_t* node, uint32_t index);
vfs_node_t* initrd_finddir(vfs_node_t* node, char* name);

uint32_t initrd_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	return 0;
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
	return NULL;
}

vfs_node_t* initrd_finddir(vfs_node_t* node, char* name)
{
	return NULL;
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
		fnode->flags &= VFS_FILE;

	if (inode->mode & EXT2_S_IFDIR)
		fnode->flags &= VFS_DIRECTORY;

	if (inode->mode & EXT2_S_IFBLK)
		fnode->flags &= VFS_BLOCKDEVICE;

	if (inode->mode & EXT2_S_IFCHR)
		fnode->flags &= VFS_CHARDEVICE;

	if (inode->mode & EXT2_S_IFIFO)
		fnode->flags &= VFS_PIPE;

	if (inode->mode & EXT2_S_IFLNK)
		fnode->flags &= VFS_SYMLINK;

	fnode->read = initrd_read;
	fnode->write = initrd_write;
	fnode->open = initrd_open;
	fnode->close = initrd_close;
	fnode->readdir = initrd_readdir;
	fnode->finddir = initrd_finddir;
}

