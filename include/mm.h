#ifndef	_MM_H
#define	_MM_H

#define STACK_ADDRESS 1 << 22   /*  4MB  */

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned long n);

#endif

#endif  /*_MM_H */