#include "dvdfile.h"

FILE *file_fd;
u64 filesize;
u64 pos;

s32 open(const char *path)
{
	close();
	
	file_fd = fopen(path, "r");
	if(!file_fd)
		return -1;
	
	fseek(file_fd, 0, SEEK_END);
	filesize = ftell(file_fd);
	rewind(file_fd);
	
	return 0;
}

s32 read(u8 *ptr, size_t size)
{
	if(file_fd)
	{
		int ret = fread(ptr, 1, size, file_fd);
		if(ret > 0)
			pos += ret;
		return ret;
	}
	
	s32 readsize = size;
	
	if(readsize > (u64) filesize - pos)
		readsize = filesize - pos;
	
	if(readsize < 0)
		return readsize;
	
	return -1;
}

void close()
{
	if(file_fd != NULL)
		fclose(file_fd);
	
	file_fd = NULL;
	filesize = 0;
	pos = 0;
}