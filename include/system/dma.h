#ifndef _DMA_H
#define _DMA_H

#include "peripherials.h"
#include "types.h"

struct dmaChannelS {
  u32 channel;
  dmaControlBlock *block;
  bool status;
  u8 fam[];
} typedef dmaChannelS;

enum dmaChannelE {
  CT_NONE = -1,
  CT_NORMAL = 0x81
} typedef dmaChannelE;

dmaChannelS *dma_open_channel(u32 channel);
void dma_close_channel(dmaChannelS *channel);
void dma_setup_mem_copy(dmaChannelS *channel, void *dest, void *src, u32 length, u32 burst_length);
void dma_start(dmaChannelS *ch);
bool dma_wait(dmaChannelS *ch);

#endif /* _DMA_H */