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
	if ((node->flags & VFS_DIRECTORY) && node->readdir != 0)
		return node->finddir(node, name);
	else
		return (vfs_node_t*)NULL;
}

vfs_node_t* aopen(const char* filename, uint32_t flags)
{
	if (!vfs_root)
		KERNEL_HALT("Attempted to aopen() without filesystem!");

	if (!filename[0] == '/')
		KERNEL_HALT("Attempted to aopen() with a non-absolute path!");

	uint32_t path_len = strlen(filename);
	char* path = (char*)amalloc(sizeof(char) * (path_len));
	memcpy(path, filename, path_len);
	char* path_offset = path;
	uint32_t path_depth = 0;

	while (path_offset < path + path_len)
	{
		if (*path_offset == '/')
		{

			*path_offset = '\0';
			path_depth++;
		}

		path_offset++;
	}

	path[path_len] = '\0';
	path_offset = path + 1;
	uint32_t depth;
	vfs_node_t* node_ptr = vfs_root;

	for (depth = 0; depth < path_depth; ++depth)
	{
		node_ptr = finddir_vfs(node_ptr, path_offset);

		if (!node_ptr)
		{
			free((void*)path);
			return NULL;
		}
		else if (depth == path_depth - 1)
			return node_ptr;
	}

	free((void*)path);
	return NULL;
}
