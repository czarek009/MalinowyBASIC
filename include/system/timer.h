#ifndef _TIMER_H
#define _TIMER_H

#include "types.h"


#define SYS_TIMER_FREQUENCY 1000000 // 1MHz
#define TIMER_CS_M0      (1 << 0)
#define TIMER_CS_M1      (1 << 1)
#define TIMER_CS_M2      (1 << 2)
#define TIMER_CS_M3      (1 << 3)

void timer_init();
void handle_timer_1();
void delay_ms(u64 ms);
void delay_us(u64 us);

#endif  /*_TIMER_H */