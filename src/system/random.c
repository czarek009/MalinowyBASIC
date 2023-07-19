#include "random.h"
#include "types.h"
#include "peripherials.h"
#include "utils.h"


// void rand_init() {
//   RNG_REGS->rng_status = 0x40000;
//   RNG_REGS->rng_int_mask |= 1;
//   RNG_REGS->rng_ctrl |= 1;
// }

void rand_init() {
  put32(RNG_TOTAL_BIT_COUNT_THRESHOLD, 0x40000);
  put32(RNG_FIFO_COUNT, 2 << RNG_FIFO_COUNT_RNG_FIFO_THRESHOLD__SHIFT);
  put32(ARM_HW_RNG200_BASE, (3 << RNG_CTRL_RNG_DIV_CTRL__SHIFT) | RNG_CTRL_RNG_RBGEN__MASK);
}

u32 rand(u32 min, u32 max) {
  while((get32(RNG_FIFO_COUNT) & RNG_FIFO_COUNT_RNG_FIFO_COUNT__MASK) == 0);
  return get32(RNG_FIFO_DATA);
  // while(!((RNG_REGS->rng_status)>>24));
  // return RNG_REGS->rng_data % (max-min) + min;
}