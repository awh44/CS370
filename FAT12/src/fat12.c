#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fat12.h"

//Need a macro (and not a function) to make use of sizeof on the array
#define READ_INTO_ARRAY(fd, array)\
	bool_read(fd, array, sizeof(array))

#define PRINT_ARRAY(array)\
	printf("%.*s", (int) sizeof(array), (char *) array)

#define PRINT_ARRAY_NL(array)\
	do\
	{\
		PRINT_ARRAY(array);\
		printf("\n");\
	} while (0)\

#define FILE_DELETED 0xe5
#define ENTRY_FREE 0

#define READ_ONLY 1
#define HIDDEN 2
#define SYSTEM_FILE 4
#define VOLUME_LABEL 8
#define SUBDIRECTORY 16
#define ARCHIVE 32

#define SECONDS_BITS_TO_RIGHT 0
#define SECONDS_MASK 0x1f
#define MINUTES_BITS_TO_RIGHT 5
#define MINUTES_MASK (0x2f << MINUTES_BITS_TO_RIGHT)
#define HOURS_BITS_TO_RIGHT 11
#define HOURS_MASK (0x1f << HOURS_BITS_TO_RIGHT)

#define BASE_YEAR 1980
#define DAY_BITS_TO_RIGHT 0
#define DAY_MASK 0x1f
#define MONTH_BITS_TO_RIGHT 5
#define MONTH_MASK (0xf << MONTH_BITS_TO_RIGHT)
#define YEAR_BITS_TO_RIGHT 9
#define YEAR_MASK (0x3f << YEAR_BITS_TO_RIGHT)

#define MASK_AND_SHIFT(expr, mask)\
	(((expr) & mask##_MASK) >> mask##_BITS_TO_RIGHT)

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
uint8_t read_directory_entry(int fd, direntry_t *entry);
uint8_t read_root_directory(int fd, fat12_t *fat);
uint8_t is_volume_label(direntry_t *entry);
uint8_t is_entry_free(direntry_t *entry);
uint8_t is_entry_deleted(direntry_t *entry);
uint8_t is_regular_entry(direntry_t *entry);

//General fat12 functions
uint8_t skip_reserved_sectors(int fd, boot_t *boot);
uint8_t read_file_allocation_tables(int fd, fat12_t *fat);
void print_volume_data(fat12_t *fat);
void print_file_data(fat12_t *fat);
void print_single_file_data(direntry_t *entry);
void extract_single_file(int fd, fat12_t *fat, uint16_t i, char *full_name, size_t dirlen);
void write_cluster(int fatFd, int outFd, uint32_t base_address, uint16_t
	cluster, uint32_t cluster_size, uint32_t write_size);

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

	return read_boot_sector(fd, &fat->boot) &&
		skip_reserved_sectors(fd, &fat->boot) &&
		read_file_allocation_tables(fd, fat) &&
		read_root_directory(fd, fat);
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
	fat->root_dir_entries = malloc(fat->boot.max_root_dir_entries *
		sizeof *fat->root_dir_entries);
	if (fat->root_dir_entries == NULL)
	{
		return 0;
	}

	direntry_t *root_dir_entries = fat->root_dir_entries;
	uint16_t max = fat->boot.max_root_dir_entries;
	int i;
	for (i = 0; i < max; i++)
	{
		direntry_t *current_entry = root_dir_entries + i;
		if (!read_directory_entry(fd, current_entry))
		{
			return 0;
		}

		//If the current entry is free, then all following entries must be as
		//well
		if (is_entry_free(current_entry))
		{
			break;
		}

		if (is_volume_label(current_entry))
		{
			fat->volume_label = current_entry;
		}
	}

	return 1;
}

uint8_t is_volume_label(direntry_t *entry)
{
	return entry->attributes & VOLUME_LABEL;
}

uint8_t is_entry_free(direntry_t *entry)
{
	return entry->filename[0] == ENTRY_FREE;
}

uint8_t is_entry_deleted(direntry_t *entry)
{
	return entry->filename[0] == FILE_DELETED;
}

uint8_t is_regular_entry(direntry_t *entry)
{
	return !is_volume_label(entry) && !is_entry_deleted(entry);
}

void print_fat12(fat12_t *fat)
{
	print_volume_data(fat);
	printf("\n");
	print_file_data(fat);
}

void print_volume_data(fat12_t *fat)
{
	printf("Volume name is ");
	PRINT_ARRAY(fat->volume_label->filename);
	PRINT_ARRAY_NL(fat->volume_label->extension);
}

void print_file_data(fat12_t *fat)
{
	uint32_t total_files = 0, total_size = 0;
	int i;
	for (i = 0;
	     i < fat->boot.max_root_dir_entries &&
	     !is_entry_free(fat->root_dir_entries + i);
	     i++)
	{
		direntry_t *entry = fat->root_dir_entries + i;
		if (is_regular_entry(entry))
		{
			print_single_file_data(entry);
			total_files++;
			total_size += entry->filesize;
		}
	}

	printf("\n%u file(s), %u bytes\n", total_files, total_size);
}

void print_single_file_data(direntry_t *entry)
{
	PRINT_ARRAY(entry->filename);
	printf(".");
	PRINT_ARRAY(entry->extension);

	uint16_t date = entry->date, time = entry->time;
	printf(" %10u %02u-%02u-%02u %02u:%02u:%02u\n",
		entry->filesize,
		MASK_AND_SHIFT(date, MONTH),
		MASK_AND_SHIFT(date, DAY),
		MASK_AND_SHIFT(date, YEAR) + BASE_YEAR,
		MASK_AND_SHIFT(time, HOURS),
		MASK_AND_SHIFT(time, MINUTES),
		MASK_AND_SHIFT(time, SECONDS));
}

void free_fat12(fat12_t *fat)
{
	free(fat->fat_entries);
	free(fat->root_dir_entries);
}

void extract_files(int fd, fat12_t *fat, char *out_dir)
{
	//plus one for '/'
	size_t dir_len = strlen(out_dir) + 1;
	//directory name, file name, '.', extension, '\0'
	size_t full_len = dir_len + MAX_FILE_NAME + 1 + MAX_EXTENSION + 1;
	char *full_name = malloc(full_len * sizeof *full_name);
	if (full_name == NULL)
	{
		printf("Could not allocate memory.\n");
		return;
	}
	strcpy(full_name, out_dir);
	full_name[dir_len - 1] = '/'; //in case the user forgot to add one
	full_name[dir_len] = '\0';

	uint16_t i = 0;
	while (fat->boot.max_root_dir_entries &&
	       !is_entry_free(fat->root_dir_entries + i))
	{
		if (is_regular_entry(fat->root_dir_entries + i))
		{
			extract_single_file(fd, fat, i, full_name, dir_len);
		}
		i++;
	}

	free(full_name);
}

void extract_single_file(int fd, fat12_t *fat, uint16_t i, char *full_name, size_t dirlen)
{
	direntry_t *entry = fat->root_dir_entries + i;

	unsigned int permissions;
	if (entry->attributes & READ_ONLY)
	{
		permissions = 0444;
	}
	else
	{
		permissions = 0644;
	}

	const size_t filename_size = sizeof(entry->filename);
	const size_t extension_size = sizeof(entry->extension);
	char *file_start = full_name + dirlen;

	size_t last_nonspace = filename_size - 1;
	while (last_nonspace >= 0 && entry->filename[last_nonspace] == ' ')
	{
		last_nonspace--;
	}
	size_t nospace_name_size = last_nonspace + 1;
	memcpy(file_start, entry->filename, nospace_name_size);

	char empty_extension[extension_size];
	memset(empty_extension, ' ', extension_size);

	if (memcmp(entry->extension, empty_extension, extension_size) != 0)
	{
		memcpy(file_start + nospace_name_size, ".", 1);
		memcpy(file_start + nospace_name_size + 1, entry->extension, extension_size);
		file_start[nospace_name_size + 1 + extension_size] = '\0';
	}
	else
	{
		file_start[nospace_name_size] = '\0';
	}

	int outFd = open(full_name, O_CREAT | O_TRUNC | O_WRONLY, permissions);
	if (outFd < 0)
	{
		printf("Could not create file with name %s\n", full_name);
		return;
	}

	uint16_t cluster = entry->start_cluster;
	if (cluster != 0)
	{
		boot_t *boot = &fat->boot;
		uint32_t base_address =
			(boot->reserved_sectors + boot->sectors_per_fat * boot->fat_copies) * boot->bytes_per_sector +
			boot->max_root_dir_entries * DIR_ENTRY_MEMBER_SIZE;

		uint32_t cluster_size = boot->bytes_per_sector * boot->sectors_per_cluster;

		uint8_t last_cluster;
		do
		{
			uint16_t next_cluster = fat->fat_entries[cluster - 2];
			last_cluster = next_cluster == fat->eof_marker;
			uint32_t write_size = last_cluster ? entry->filesize % cluster_size : cluster_size;

			write_cluster(fd, outFd, base_address, cluster, cluster_size, write_size);

			cluster = next_cluster;
		} while (!last_cluster);
	}
}

void write_cluster(int fatFd, int outFd, uint32_t base_address, uint16_t
	cluster, uint32_t cluster_size, uint32_t write_size)
{
	lseek(fatFd, base_address + cluster_size * (cluster - 2), SEEK_SET);
	char *buff = malloc(cluster_size * sizeof *buff);
	read(fatFd, buff, cluster_size);
	write(outFd, buff, write_size);
}
