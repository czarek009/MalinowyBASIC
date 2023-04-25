#ifndef  _UTILS_H
#define  _UTILS_H

#include "types.h"

void delay(u64 val);
void put32(u64 addr, u32 val);
u32 get32(u64 addr);
u32 get_el(void);

#endif  /*_UTILS_H */