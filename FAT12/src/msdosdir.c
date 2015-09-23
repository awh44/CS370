#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "../include/fat12.h"

int main(int argc, char *argv[])
{
	int fd = open("input/samplefat.bin", O_RDONLY, NULL);
	if (fd < 0)
	{
		perror("Could not open input file");
		return 1;
	}

	fat12_t fat;
	if (!read_fat12(fd, &fat))
	{
		perror("Could not read file system");
		return 1;
	}

	print_fat12(&fat);
	free_fat12(&fat);
	return 0;
}
