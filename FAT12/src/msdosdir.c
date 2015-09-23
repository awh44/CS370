#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "../include/fat12.h"

#define USER_ERROR 1
#define SYSTEM_ERROR 2

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please include the input file as a command line argument.\n");
		return USER_ERROR;
	}

	int fd = open(argv[1], O_RDONLY, NULL);
	if (fd < 0)
	{
		perror("Could not open input file");
		return SYSTEM_ERROR;
	}

	fat12_t fat;
	if (!read_fat12(fd, &fat))
	{
		perror("Could not read file system");
		return SYSTEM_ERROR;
	}

	print_fat12(&fat);
	free_fat12(&fat);
	return 0;
}
