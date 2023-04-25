#ifndef  _MM_H
#define  _MM_H

#define STACK_ADDRESS 1 << 22   /*  4MB  */

#ifndef __ASSEMBLER__

#include "types.h"

void memzero(u64 src, u64 n);

#endif

#endif  /*_MM_H */