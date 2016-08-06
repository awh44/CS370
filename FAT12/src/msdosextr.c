#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "fat12.h"

#define USER_ERROR 1
#define SYSTEM_ERROR 2;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Please include the input file and output directories as command line arguments.\n");
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
		fprintf(stderr, "Could not read file system.");
		free_fat12(&fat);
		return SYSTEM_ERROR;
	}

	extract_files(fd, &fat, argv[2]);
	free_fat12(&fat);
	return 0;
}
