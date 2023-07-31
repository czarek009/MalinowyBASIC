#include "peripherials.h"
#include "timer.h"
#include "types.h"


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