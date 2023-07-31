#include "mailbox.h"
#include "peripherials.h"
#include "utils.h"
#include "mm.h"
#include "printf.h"

#define MBOX_EMPTY 0x40000000
#define MBOX_FULL  0x80000000

#define MBOX_DATA_SIZE sizeof(u32)

enum mailboxStatusE {
  MBOX_STATUS_REQUEST = 0,
  MBOX_STATUS_SUCCESS = 0x80000000,
  MBOX_STATUS_ERROR   = 0x80000001
} typedef mailboxStatusE;

enum channelE {
  MBOX_CH_POWER,
  MBOX_CH_FB,
  MBOX_CH_VUART,
  MBOX_CH_VCHIQ,
  MBOX_CH_LEDS,
  MBOX_CH_BUTTONS,
  MBOX_CH_TOUCH,
  MBOX_CH_COUNT,
  MBOX_CH_TAGS,
  MBOX_CH_NOT_DECLARED
} typedef channelE;

static u32 mailbox_data[8192] __attribute__((aligned(16)));

static void mailbox_write(channelE ch, u32 data) {
  while(MBX_REGS->status & MBOX_FULL);
  MBX_REGS->write = ((data & 0xFFFFFFF0) | (ch & 0xF));
}

static u32 mailbox_read(channelE ch) {
  channelE read_ch = MBOX_CH_NOT_DECLARED;
  u32 data;
  while(read_ch != ch) {
    while(MBX_REGS->status & MBOX_EMPTY);
    data = MBX_REGS->read;
    read_ch = (data & 0xF);
  }
  return data & 0xFFFFFFF0;
}

void mailbox_process(mailboxTagS *tag, u32 data_size) {
  u32 buffer_size = data_size + 12;

  memcpy((mailbox_data + 2), tag, data_size);

  mailbox_data[0] = buffer_size;
  mailbox_data[1] = MBOX_STATUS_REQUEST;
  mailbox_data[(buffer_size / MBOX_DATA_SIZE) - 1] = MBOX_TAG_END;

  mailbox_write(MBOX_CH_TAGS, (u32)(u64)mailbox_data);

  mailbox_read(MBOX_CH_TAGS);

  memcpy(tag, (mailbox_data + 2), data_size);
}