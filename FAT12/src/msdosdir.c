#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "fat12.h"

int main(int argc, char *argv[])
{
	boot_t boot;
	int fd = open("input/samplefat.bin", O_RDONLY, NULL);

	if (fd < 0)
	{
		perror("Could not open input file");
		return 1;
	}

	if (!read_boot_sector(fd, &boot))
	{
		perror("Could not read boot sector.");
		return 1;
	}

	return 0;
}
