#include "peripherials.h"
#include "timer.h"
#include "types.h"

// u64 cur_val_1 = 0;

// void timer_init() {
//   cur_val_1 = TIMER_REGS->timer_clo;
//   cur_val_1 += (SYS_TIMER_FREQUENCY / 2);
//   TIMER_REGS->timer_c1 = cur_val_1;
// }

// void handle_timer_1() {
//   cur_val_1 += (SYS_TIMER_FREQUENCY / 2);
//   TIMER_REGS->timer_c1 = cur_val_1;
//   TIMER_REGS->timer_cs |= TIMER_CS_M1;
// }

void delay_ms(u64 ms) {
  u64 currTicks = TIMER_REGS->timer_clo;
  u64 ms2ticks = ms * (SYS_TIMER_FREQUENCY/1000);
  u64 waitTicks = currTicks + ms2ticks;

  while (TIMER_REGS->timer_clo < waitTicks) {}
}

void delay_us(u64 us) {
  u64 currTicks = TIMER_REGS->timer_clo;
  u64 us2ticks = us * (SYS_TIMER_FREQUENCY/1000000);
  u64 waitTicks = currTicks + us2ticks;

  while (TIMER_REGS->timer_clo < waitTicks) {}
}