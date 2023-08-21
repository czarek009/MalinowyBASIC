#include "types.h"
#include "fs.h"
#include "sd.h"
#include "debug.h"
#include "mm.h"
#include "butils.h"
#include "printf.h"
#include "startup.h"


#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

static fileS file_table[NUM_OF_FILES] = {0};
static mbrS mbr = {0};
static partitionEntryS data_partition = {0};

/* PRIVATE FUNCTIONS DECLARATIONS */
u64 write_data(u64 offset, void* buf, u64 len);
u64 read_data(u64 offset, void* buf, u64 len);
bool load_mbr(mbrS* dest);
bool save_file_table(void);

/* PUBLIC FUNCTIONS DEFINITIONS */
bool fs_init() {
  if (!load_mbr(&mbr)) {
    ERROR("[ERROR] Cannot get partition info\n");
    return false;
  }

  bool data_partition_found = false;
  for (int i = 1; i < 4; ++i) {
    if (mbr.partitions[i].type != 0 &&
        (u64)mbr.partitions[i].num_sectors >= (u64)((2UL<<30UL) / 512UL)) {
      data_partition = mbr.partitions[i];
      data_partition_found = true;
      break;
    }
  }
  if (!data_partition_found) {
    ERROR("[ERROR] Cannot find data partition\n");
    return false;
  }

  sd_seek(data_partition.first_lba_sector*512);
  int r = sd_read_block(file_table, sizeof(file_table));

  if (r != 512) {
    ERROR("[ERROR] Cannot read file table\n");
    return false;
  }

  if (file_table[0].id == 0 && file_table[0].type == 0x69) {
    INFO("[INFO] File system initialized\n");
    return true;
  }

  INFO("[INFO] Creating a file system...\n");

  file_table[0].id = 0;
  file_table[0].type = 0x69;
  file_table[0].size_in_bytes = data_partition.first_lba_sector;
  strncpy(file_table[0].name, "SPECIAL", 8);

  sd_seek(data_partition.first_lba_sector*512);
  r = sd_write_block(file_table, sizeof(file_table));

  if (r != 512) {
    ERROR("[ERROR] Cannot write file table\n");
    return false;
  }

  INFO("[INFO] File system initialized\n");
  return true;
}

void list_files() {
  for (int i = 0; i < NUM_OF_FILES; ++i) {
    fileS file = file_table[i];
    if (file.type != 0) {
      printf("File '");
      for (int j = 0; j < FILENAME_LEN; ++j) {
        if (file.name[j] == '\0') {
          for (int k = j; k < FILENAME_LEN; ++k) {
            printf(" ");
          }
          printf("'");
          break;
        }
        printf("%c", file.name[j]);
      }
      printf(" ID: %u  Size: %u  Type: %u\n", (u32)file.id, file.size_in_bytes, (u32)file.type);
    }
  }
}

fileS* open_file(char* name) {
  if (!strncmp(name, "SPECIAL", strlen(name))) {
    ERROR("[ERROR] Cannot open SPECIAL file\n");
    return NULL;
  }
  for (int i = 0; i < NUM_OF_FILES; ++i) {
    fileS file = file_table[i];
    if (file.type == 0) {
      continue;
    }
    if (!strncmp(name, file.name, strlen(name))) {
      return &(file_table[i]);
    }
  }

  ERROR("[ERROR] Cannot find file '%s'\n", name);
  return NULL;
}

fileS* create_file(char* name) {
  if (!strncmp(name, "SPECIAL", strlen(name))) {
    ERROR("[ERROR] Cannot create SPECIAL file\n");
    return NULL;
  }
  for (int i = 0; i < NUM_OF_FILES; ++i) {
    fileS file = file_table[i];
    if (file.type == 0) {
      continue;
    }
    if (!strncmp(name, file.name, strlen(name))) {
      ERROR("[ERROR] File '%s' already exists\n", name);
      return NULL;
    }
  }

  for (int i = 0; i < NUM_OF_FILES; ++i) {
    fileS* file = &(file_table[i]);
    if (file->type == 0) {
      file->size_in_bytes = 0;
      file->id = i;
      file->type = 1;
      strncpy(file->name, name, strlen(name)+1);
      file->name[strlen(name)] = '\0';

      save_file_table();

      return file;
    }
  }

  ERROR("[ERROR] Cannot create file '%s'\n", name);
  return NULL;
}

bool delete_file(char* name) {
  if (!strncmp(name, "SPECIAL", strlen(name))) {
    ERROR("[ERROR] Cannot delete SPECIAL file\n");
    return false;
  }
  for (int i = 0; i < NUM_OF_FILES; ++i) {
    fileS file = file_table[i];
    if (file.type == 0) {
      continue;
    }
    if (!strncmp(name, file.name, strlen(name))) {
      file_table[i].type = 0;
      save_file_table();
      return true;
    }
  }

  ERROR("[ERROR] Cannot find file '%s'\n", name);
  return false;
}

u64 write_to_file(fileS* file, void* buf, u64 len, bool append) {
  u64 bytes = 0;
  if (append) {
    u64 pos = data_partition.first_lba_sector*512 + file->id * (PARTITION_SIZE/NUM_OF_FILES) + file->size_in_bytes;
    bytes = write_data(pos, buf, len);
    file->size_in_bytes += bytes;
  } else {
    u64 pos = data_partition.first_lba_sector*512 + file->id * (PARTITION_SIZE/NUM_OF_FILES);
    bytes = write_data(pos, buf, len);
    file->size_in_bytes = bytes;
  }
  save_file_table();
  return bytes;
}

u64 read_from_file(fileS* file, void* buf, u64 len) {
  u64 pos = data_partition.first_lba_sector*512 + file->id * (PARTITION_SIZE/NUM_OF_FILES);
  return read_data(pos, buf, min(file->size_in_bytes, len));
}

void list_partitions(void) {
  for (int i = 0; i < 4; ++i) {
    printf("Partition %d:\n", i);
    printf("\t Type: %d\n", mbr.partitions[i].type);
    printf("\t NumSecs: %d\n", mbr.partitions[i].num_sectors);
    printf("\t Status: %d\n", mbr.partitions[i].status);
    printf("\t Start: 0x%X\n", mbr.partitions[i].first_lba_sector);
  }
}

void fs_startup_info(void) {
  STARTUP("[STARTUP]      Files Sytem initialized\n");
  STARTUP("[FILESYSTEM]   Partition Size: %ld\n", PARTITION_SIZE);
  STARTUP("[FILESYSTEM]   Partitions:\n");
  list_partitions();
}

/* PRIVATE FUNCTIONS DEFINITIONS */
u64 write_data(u64 offset, void* buf, u64 len) {
  if (buf == NULL) {
    ERROR("[ERROR] Buffer points to NULL\n");
    return 0;
  }
  u64 wrote = 0;
  u64 block = offset/512;
  u64 offset_in_block = offset%512;
  char sd_buf[512] = {0};

  while (len) {
    if (offset_in_block != 0) {
      sd_seek(block*512);
      sd_read_block(sd_buf, 512);

      memcpy(sd_buf+offset_in_block, buf, min(len, 512-offset_in_block));
      wrote += min(len, 512-offset_in_block);
      buf += min(len, 512-offset_in_block);
      len -= min(len, 512-offset_in_block);
      offset_in_block = 0;
      block++;

      sd_write_block(sd_buf, 512);
    } else {
      sd_seek(block*512);
      sd_read_block(sd_buf, 512);

      memcpy(sd_buf, buf, min(len, 512));
      wrote += min(len, 512);
      buf += min(len, 512);
      len -= min(len, 512);
      offset_in_block = 0;
      block++;

      sd_write_block(sd_buf, 512);
    }
  }

  return wrote;
}

u64 read_data(u64 offset, void* buf, u64 len) {
  if (buf == NULL) {
    ERROR("[ERROR] Buffer points to NULL\n");
    return 0;
  }
  u64 read = 0;
  u64 block = offset/512;
  u64 offset_in_block = offset%512;
  char sd_buf[512] = {0};

  while (len) {
    if (offset_in_block != 0) {
      sd_seek(block*512);
      sd_read_block(sd_buf, 512);

      memcpy(buf, sd_buf+offset_in_block, min(len, 512-offset_in_block));
      read += min(len, 512-offset_in_block);
      buf += min(len, 512-offset_in_block);
      len -= min(len, 512-offset_in_block);
      offset_in_block = 0;
      block++;

      sd_write_block(sd_buf, 512);
    } else {
      sd_seek(block*512);
      sd_read_block(sd_buf, 512);

      memcpy(buf, sd_buf, min(len, 512));
      read += min(len, 512);
      buf += min(len, 512);
      len -= min(len, 512);
      offset_in_block = 0;
      block++;

      sd_write_block(sd_buf, 512);
    }
  }

  return read;
}

bool load_mbr(mbrS* dest) {
  sd_seek(0);
  int r = sd_read_block(dest, sizeof(mbrS));

  if (r != 512) {
    ERROR("[ERROR] Cannot read master boot record from sd card\n");
    return false;
  }

  if (dest->bootSignature != BOOT_SIGNATURE) {
    ERROR("[ERROR] BAD BOOT SIGNATURE: %X\n", dest->bootSignature);
  }

  return true;
}

bool save_file_table(void) {
  sd_seek(data_partition.first_lba_sector*512);
  u64 r = sd_write_block(file_table, sizeof(file_table));

  if (r != 512) {
    ERROR("[ERROR] Cannot write file table\n");
    return false;
  }

  return true;
}