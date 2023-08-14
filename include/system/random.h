#ifndef _RANDOM_H
#define _RANDOM_H

#include "types.h"


void rand_init();
u32 rand(u32 min, u32 max);
void rand_startup_info();

#endif /* _RANDOM_H */