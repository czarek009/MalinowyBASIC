#include "dma.h"
#include "printf.h"
#include "mm.h"
#include "timer.h"


#define GPU_UNCACHED_BASE	0xC0000000
#define GPU_MEM_BASE	GPU_UNCACHED_BASE

#define BUS_ADDRESS(addr)	(((addr) & ~0xC0000000) | GPU_MEM_BASE)

#define DMA_BLOCK_ADDRESS 0x400000

dmaChannelS channels[15];
static u16 channel_map = 0x1F35;

static u16 allocate_channel(u32 channel) {
  if (!(channel & ~0x0F)) {
    if (channel_map & (1 << channel)) {
      channel_map &= ~(1 << channel);
      return channel;
    }
    return -1;
  }

  u16 i = channel == CT_NORMAL ? 6 : 12;

  for (; i >= 0; i--) {
    if (channel_map & (1 << i)) {
      channel_map &= ~(1 << i);
      return i;
    }
  }
  return CT_NONE;
}

dmaChannelS *dma_open_channel(u32 channel) {
  u32 _channel = allocate_channel(channel);

  if (_channel == CT_NONE) {
      printf("INVALID CHANNEL! %d\n", channel);
      return NULL;
  }

  dmaChannelS *dma = (dmaChannelS *)&channels[_channel];
  dma->channel = _channel;

  dma->block = (dmaControlBlock *)DMA_BLOCK_ADDRESS;
  dma->block->reserved[0] = 0;
  dma->block->reserved[1] = 0;

  DMA_REGS->enable |= (1 << dma->channel);
  delay_ms(3);
  DMA_REGS->channels[dma->channel].control |= CS_RESET;
  while((DMA_REGS->channels[dma->channel].control) & CS_RESET);

  return dma;
}


void dma_close_channel(dmaChannelS *channel) {
  channel_map |= (1 << channel->channel);
}

void dma_setup_mem_copy(dmaChannelS *channel, void *dest, void *src, u32 length, u32 burst_length) {
  channel->block->transfer_information = (burst_length << TI_BURST_LENGTH_SHIFT)
              | TI_SRC_WIDTH
              | TI_SRC_INC
              | TI_DEST_WIDTH
              | TI_DEST_INC;

  channel->block->source_address = (u32)(u64)src;
  channel->block->destination_address = (u32)(u64)dest;
  channel->block->transfer_length = length;
  channel->block->mode_stride = 0;
  channel->block->next_control_block_address = 0;
}

void dma_start(dmaChannelS *ch) {
  DMA_REGS->channels[ch->channel].control_block_address = BUS_ADDRESS((u64)ch->block);

  DMA_REGS->channels[ch->channel].control = CS_WAIT_FOR_OUTSTANDING_WRITES
              | (DEFAULT_PANIC_PRIORITY << CS_PANIC_PRIORITY_SHIFT)
              | (DEFAULT_PRIORITY << CS_PRIORITY_SHIFT)
              | CS_ACTIVE;
}

bool dma_wait(dmaChannelS *ch) {
  while(DMA_REGS->channels[ch->channel].control & CS_ACTIVE);
  ch->status = DMA_REGS->channels[ch->channel].control & CS_ERROR ? false : true;
  return ch->status;
}