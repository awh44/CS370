#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/fat12.h"

//Need a macro (and not a function) to make use of sizeof on the array
#define READ_INTO_ARRAY(fd, array)\
	bool_read(fd, array, sizeof(array))

#define PRINT_ARRAY(array)\
	printf("%.*s", (int) sizeof(array), (char *) array)

#define PRINT_ARRAY_NL(array)\
	PRINT_ARRAY(array); printf("\n")

#define FILE_DELETED 0xe5
#define ENTRY_FREE 0

#define READ_ONLY     1
#define HIDDEN        2
#define SYSTEM_FILE   4
#define VOLUME_LABEL  8
#define SUBDIRECTORY 16
#define ARCHIVE      32

//Reading helper functions
uint8_t bool_read(int fd, void *array, size_t n);
uint8_t bool_seek(int fd, size_t n);
uint8_t read_uint8(int fd, uint8_t *integer);
uint8_t read_uint16_little_endian(int fd, uint16_t *integer);
uint8_t read_twelve_bits_twice(int fd, uint16_t *integers);

//Boot sector functions
uint8_t read_boot_sector(int fd, boot_t *boot);
uint8_t skip_remaining_boot_sector(int fd, boot_t *boot);

//Directory entry functions
uint8_t read_root_directory(int fd, fat12_t *fat);

//General fat12 functions
uint8_t read_fat12(int fd, fat12_t *fat);
uint8_t skip_reserved_sectors(int fd, boot_t *boot);
uint8_t read_file_allocation_tables(int fd, fat12_t *fat);

uint8_t bool_read(int fd, void *array, size_t n)
{
	return read(fd, array, n) < 0 ? 0 : 1;
}

uint8_t bool_seek(int fd, size_t n)
{
	return lseek(fd, n, SEEK_CUR) >= 0;
}

uint8_t read_uint8(int fd, uint8_t *integer)
{
	return bool_read(fd, integer, 1);
}

uint8_t read_uint16_little_endian(int fd, uint16_t *integer)
{
	//Linux on x86-64 is already little endian, so don't actually have to convert
	return bool_read(fd, integer, sizeof *integer);
}

uint8_t read_uint32_little_endian(int fd, uint32_t *integer)
{
	return bool_read(fd, integer, sizeof *integer);
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
	return 1;
}

uint8_t read_boot_sector(int fd, boot_t *boot)
{
	//Can't just read directly from the file into boot because the struct
	//is padded. The padding could be removed using _attribute_((packed)),
	//but doing so could/would cause a performance degradation
	return READ_INTO_ARRAY(fd, boot->jmp_boot) &&
		READ_INTO_ARRAY(fd, boot->oem_name) &&
		read_uint16_little_endian(fd, &boot->bytes_per_sector) &&
		read_uint8(fd, &boot->sectors_per_cluster) &&
		read_uint16_little_endian(fd, &boot->reserved_sectors) &&
		read_uint8(fd, &boot->fat_copies) &&
		read_uint16_little_endian(fd, &boot->max_root_dir_entries) &&
		read_uint16_little_endian(fd, &boot->total_sectors) &&
		read_uint8(fd, &boot->media_type) &&
		read_uint16_little_endian(fd, &boot->sectors_per_fat) &&
		read_uint16_little_endian(fd, &boot->sectors_per_track) &&
		read_uint16_little_endian(fd, &boot->num_heads) &&
		read_uint16_little_endian(fd, &boot->num_hidden) &&
		READ_INTO_ARRAY(fd, boot->bootstrap) &&
		READ_INTO_ARRAY(fd, boot->signature) &&
		skip_remaining_boot_sector(fd, boot);
}

uint8_t skip_remaining_boot_sector(int fd, boot_t *boot)
{
	size_t boot_sector_remaining = boot->bytes_per_sector - BOOT_MEMBER_SIZE;
	return bool_seek(fd, boot_sector_remaining);
}

uint8_t read_fat12(int fd, fat12_t *fat)
{
	//free_fat will free these, so make sure they're set to NULL as a
	//precaution in case the function fails before they're allocated
	fat->fat_entries = NULL;
	fat->root_dir_entries = NULL;

	uint8_t result = read_boot_sector(fd, &fat->boot) &&
		skip_reserved_sectors(fd, &fat->boot) &&
		read_file_allocation_tables(fd, fat);

	if (!result)
	{
		return 0;
	}

	fat->root_dir_entries = malloc(fat->boot.max_root_dir_entries *
		sizeof *fat->root_dir_entries);
	if (fat->root_dir_entries == NULL)
	{
		return 0;
	}

	result = read_root_directory(fd, fat);

	return result;
}

uint8_t skip_reserved_sectors(int fd, boot_t *boot)
{
	//Minus 1 because of the boot sector. Use uint32 because each is 16 bits,
	//so could end up with 32 bits through multiplication
	uint32_t reserved_sector_bytes = (boot->reserved_sectors - 1) *
		boot->bytes_per_sector;
	return bool_seek(fd, reserved_sector_bytes);
}

uint8_t read_file_allocation_tables(int fd, fat12_t *fat)
{
	boot_t *boot = &fat->boot;
	uint16_t tmp[2];
	//read media descriptor and eof_marker
	if (!read_twelve_bits_twice(fd, tmp))
	{
		return 0;
	}
	fat->media_descriptor = tmp[0];
	fat->eof_marker = tmp[1];

	uint32_t bytes_per_fat = boot->sectors_per_fat * boot->bytes_per_sector;
	//Multiply by the number of entries per byte (2 for every 3 bytes), then
	//subtract out 2 for the media_descriptor and the eof_marker
	uint32_t num_fat_entries = bytes_per_fat * 2 / 3 - 2;
	fat->fat_entries = malloc(num_fat_entries * sizeof *fat->fat_entries);
	if (fat->fat_entries == NULL)
	{
		return 0;
	}
	
	int i;
	for (i = 0; i < num_fat_entries; i += 2)
	{
		if (!read_twelve_bits_twice(fd, fat->fat_entries + i))
		{
			return 0;
		}
	}

	//Skip any remaining FAT copies
	return bool_seek(fd, (boot->fat_copies - 1) * bytes_per_fat);
}

uint8_t read_directory_entry(int fd, direntry_t *entry)
{
	return READ_INTO_ARRAY(fd, entry->filename) &&
		READ_INTO_ARRAY(fd, entry->extension) &&
		read_uint8(fd, &entry->attributes) &&
		READ_INTO_ARRAY(fd, entry->reserved_bytes) &&
		read_uint16_little_endian(fd, &entry->time) &&
		read_uint16_little_endian(fd, &entry->date) &&
		read_uint16_little_endian(fd, &entry->start_cluster) &&
		read_uint32_little_endian(fd, &entry->filesize);
}

uint8_t read_root_directory(int fd, fat12_t *fat)
{
	direntry_t *root_dir_entries = fat->root_dir_entries;
	uint16_t max = fat->boot.max_root_dir_entries;
	int i;
	for (i = 0; i < max; i++)
	{
		read_directory_entry(fd, root_dir_entries + i);
		unsigned char *filename = root_dir_entries[i].filename;
		if (filename[0] != ENTRY_FREE && filename[0] != FILE_DELETED)
		{
			PRINT_ARRAY_NL(root_dir_entries[i].filename);
		}

	}
}

void free_fat12(fat12_t *fat)
{
	free(fat->fat_entries);
	free(fat->root_dir_entries);
}
