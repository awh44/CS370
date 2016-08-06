#ifndef __FAT_12_H__
#define __FAT_12_H__

#include <stdint.h>

#define BOOT_MEMBER_SIZE 512
typedef struct
{
	uint8_t jmp_boot[3]; //bytes 0-2 (3 bytes)
	unsigned char oem_name[8]; //bytes 3-10 (8 bytes)
	uint16_t bytes_per_sector; //bytes 11-12 (2 bytes)
	uint8_t sectors_per_cluster; //byte 13 (1 byte)
	uint16_t reserved_sectors; //bytes 14-15 (2 bytes); 1 for FAT12/16, 32 for FAT32
	uint8_t fat_copies; //byte 16
	uint16_t max_root_dir_entries; //bytes 17-18 (2 bytes)
	uint16_t total_sectors; //bytes 19-20 (2 bytes)
	uint8_t media_type; //byte 21
	uint16_t sectors_per_fat; //bytes 22-23 (2 bytes)
	uint16_t sectors_per_track; //bytes 24-25 (2 bytes)
	uint16_t num_heads; //bytes 26-27 (2 bytes)
	uint16_t num_hidden; //bytes 28-29 (2 bytes)
	uint8_t bootstrap[480]; //bytes 30-509 (480 bytes)
	uint8_t signature[2]; //bytes 510-511 (2 bytes)
} boot_t;

#define DIR_ENTRY_MEMBER_SIZE 32
#define MAX_FILE_NAME 8
#define MAX_EXTENSION 3
typedef struct
{
	unsigned char filename[MAX_FILE_NAME];
	unsigned char extension[MAX_EXTENSION];
	uint8_t attributes;
	uint8_t reserved_bytes[10];
	uint16_t time;
	uint16_t date;
	uint16_t start_cluster;
	uint32_t filesize;
} direntry_t;

typedef struct
{
	boot_t boot;
	uint8_t media_descriptor;
	uint16_t eof_marker;
	uint16_t *fat_entries;
	direntry_t *root_dir_entries;
	direntry_t *volume_label;
} fat12_t;

uint8_t read_fat12(int fd, fat12_t *fat);
void print_fat12(fat12_t *fat);
void extract_files(int fd, fat12_t *fat, char *out_dir);
void free_fat12(fat12_t *fat);

#endif
