#ifndef _FS_H
#define _FS_H

#include "types.h"

#define NUM_OF_FILES 32
#define FILENAME_LEN 10
#define PARTITION_SIZE 2<<30   // 1GB

#define BOOT_SIGNATURE 0xAA55

typedef struct __attribute__((__packed__)) {
  u8 head;
  u8 sector : 6;
  u8 cylinder_hi : 2;
  u8 cylinder_lo;
} chsAddrS;

typedef struct __attribute__((__packed__)) {
  u8 status;
  chsAddrS first_sector;
  u8 type;
  chsAddrS last_sector;
  u32 first_lba_sector;
  u32 num_sectors;
} partitionEntryS;

typedef struct __attribute__((__packed__)) {
  u8 bootCode[0x1BE];
  partitionEntryS partitions[4];
  u16 bootSignature;
} mbrS;

typedef struct {
  u32 size_in_bytes;
  u8 id;
  u8 type;
  char name[FILENAME_LEN];
} fileS;


bool fs_init(void);
fileS* open_file(char* name);
fileS* create_file(char* name);
void delete_file(char* name);
u64 write_to_file(fileS* file, void* buf, u64 len);
u64 read_from_file(fileS* file, void* buf, u64 len);
void list_files(void);

#endif /* _FS_H */