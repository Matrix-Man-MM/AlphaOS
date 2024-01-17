#include "sys.h"
#include "vfs.h"

vfs_node_t* vfs_root = 0;

uint32_t read_vfs(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	if (node->read != 0)
		return node->read(node, offset, size, buffer);
	else
		return 0;
}

uint32_t write_vfs(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
	if (node->write != 0)
		return node->write(node, offset, size, buffer);
	else
		return 0;
}

void open_vfs(vfs_node_t* node, uint8_t read, uint8_t write)
{
	if (node->open != 0)
		node->open(node, read, write);
}

void close_vfs(vfs_node_t* node)
{
	if (node->close != 0)
		node->close(node);
}

struct dirent* readdir_vfs(vfs_node_t* node, uint32_t index)
{
	if ((node->flags & 0x07) == VFS_DIRECTORY && node->readdir != 0)
		return node->readdir(node, index);
	else
		return (struct dirent*)NULL;
}

vfs_node_t* finddir_vfs(vfs_node_t* node, char* name)
{
	if ((node->flags & 0x07) == VFS_DIRECTORY && node->readdir != 0)
		return node->finddir(node, name);
	else
		return (vfs_node_t*)NULL;
}
