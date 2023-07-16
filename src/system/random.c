#include "random.h"
#include "types.h"
#include "peripherials.h"


void rand_init() {
  RNG_REGS->rng_status = 0x40000;
  RNG_REGS->rng_int_mask |= 1;
  RNG_REGS->rng_ctrl |= 1;
}

u32 rand(u32 min, u32 max) {
  while(!((RNG_REGS->rng_status)>>24));
  return RNG_REGS->rng_data % (max-min) + min;
}