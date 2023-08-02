#ifndef _SD_H
#define _SD_H

#include "peripherials.h"
#include "types.h"


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


typedef enum {
  SDECommandTimeout,
  SDECommandCrc,
  SDECommandEndBit,
  SDECommandIndex,
  SDEDataTimeout,
  SDEDataCrc,
  SDEDataEndBit,
  SDECurrentLimit,
  SDEAutoCmd12,
  SDEADma,
  SDETuning,
  SDERsvd
} sdErrorE;

typedef enum {
  RTNone,
  RT136,
  RT48,
  RT48Busy
} emmcCmdRespTypeE;

typedef enum {
  CTGoIdle = 0,
  CTSendCide = 2,
  CTSendRelativeAddr = 3,
  CTIOSetOpCond = 5,
  CTSelectCard = 7,
  CTSendIfCond = 8,
  CTSetBlockLen = 16,
  CTReadBlock = 17,
  CTReadMultiple = 18,
  CTWriteBlock = 24,
  CTWriteMultiple = 25,
  CTOcrCheck = 41,
  CTSendSCR = 51,
  CTApp = 55
} emmcCmdTypeE;

typedef struct {
  u8 resp_a : 1;
  u8 block_count : 1;
  u8 auto_command : 2;
  u8 direction : 1;
  u8 multiblock : 1;
  u16 resp_b : 10;
  u8 response_type : 2;
  u8 res0 : 1;
  u8 crc_enable : 1;
  u8 idx_enable : 1;
  u8 is_data : 1;
  u8 type : 2;
  u8 index : 6;
  u8 res1 : 2;
} emmcCmdS;

typedef struct {
  u32 scr[2];
  u32 bus_widths;
  u32 version;
} emmcScrRegisterS;

typedef struct {
  bool last_success;
  u32 transfer_blocks;
  emmcCmdS last_command;
  reg32 last_command_value;
  u32 block_size;
  u32 last_response[4];
  bool sdhc;
  u16 ocr;
  u32 rca;
  u64 offset;
  void *buffer;
  u32 base_clock;
  u32 last_error;
  u32 last_interrupt;
  emmcScrRegisterS scr;
} emmcDeviceS;


bool sd_init(void);
void sd_seek(u64 offset);
int sd_read(void* buffer, u32 size);
int sd_write(void* buffer, u32 size);

#endif /* _SD_H */