#include "random.h"
#include "types.h"
#include "peripherials.h"
#include "utils.h"
#include "startup.h"


void rand_init() {
  #if RPI_VERSION == 3
  RNG_REGS->rng_status = 0x40000;
  RNG_REGS->rng_int_mask |= 1;
  RNG_REGS->rng_ctrl |= 1;
  #elif RPI_VERSION == 4
  RNG_REGS->rng_bit_count_threshold = 0x40000;
  RNG_REGS->rng_fifo_count = (2 << RNG_FIFO_COUNT_THRESHOLD_SHIFT);
  RNG_REGS->rng_ctrl = (3 << RNG_DIV_CTRL_SHIFT) | RNG_CTRL_RBGEN_MASK;
  #endif
}

void rand_startup_info() {
  STARTUP("Rand initialized\n");
}

u32 rand(u32 min, u32 max) {
  #if RPI_VERSION == 3
  while(!((RNG_REGS->rng_status)>>24));
  #elif RPI_VERSION == 4
  while(!(RNG_REGS->rng_fifo_count & RNG_FIFO_COUNT_MASK));
  #endif
  return RNG_REGS->rng_data % (max-min) + min;
}