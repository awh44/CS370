#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "../include/fat12.h"

//Need a macro (and not a function) to make use of sizeof on the array
#define READ_INTO_ARRAY(fd, array)\
	bool_read(fd, array, sizeof(array))

#define PRINT_ARRAY(array)\
	printf("%.*s\n", (int) sizeof(array), (char *) array)

uint8_t bool_read(int fd, void *array, size_t n)
{
	return read(fd, array, n) < 0 ? 0 : 1;
}

uint8_t read_uint8(int fd, uint8_t *integer)
{
	return bool_read(fd, integer, 1);
}

uint8_t read_uint16_little_endian(int fd, uint16_t *integer)
{
	if (!bool_read(fd, integer, sizeof *integer))
	{
		return 0;
	}
	//Linux on x86-64 is already little endian, so don't actually have to convert

	return 1;
}

uint8_t read_twelve_bits_twice(int fd, uint16_t *integers)
{
	uint8_t tmp[3];
	if (!READ_INTO_ARRAY(fd, tmp))
	{
		return 0;
	}

	integers[0] = (((uint16_t) tmp[1] & 0x0f) << 8) | tmp[0];
	integers[1] = ((uint16_t ) tmp[2] << 4) | ((tmp[1] & 0xf0) >> 4);
	
}

uint8_t read_boot_sector(int fd, boot_t *boot)
{
	//Can't just read directly from the file into boot because the struct
	//is padded. The padding could be removed using _attribute_((packed)),
	//but doing so could/would cause a performance degradation
	uint8_t result = READ_INTO_ARRAY(fd, boot->jmp_boot) &&
		READ_INTO_ARRAY(fd, boot->oem_name) &&
		read_uint16_little_endian(fd, &boot->bytes_per_sector) &&
		read_uint8(fd, &boot->sectors_per_cluster) &&
		read_uint16_little_endian(fd, &boot->reserved_sectors) &&
		read_uint8(fd, &boot->fat_copies) &&
		read_uint16_little_endian(fd, &boot->root_dir_entries) &&
		read_uint16_little_endian(fd, &boot->total_sectors) &&
		read_uint8(fd, &boot->media_type) &&
		read_uint16_little_endian(fd, &boot->sectors_per_fat) &&
		read_uint16_little_endian(fd, &boot->sectors_per_track) &&
		read_uint16_little_endian(fd, &boot->num_heads) &&
		read_uint16_little_endian(fd, &boot->num_hidden) &&
		READ_INTO_ARRAY(fd, boot->bootstrap) &&
		READ_INTO_ARRAY(fd, boot->signature);
	
	if (result)
	{
		size_t boot_sector_remaining = boot->bytes_per_sector -
			BOOT_MEMBER_SIZE;
		return lseek(fd, boot_sector_remaining, SEEK_CUR) >= 0;
	}
	
	return 0;
}
