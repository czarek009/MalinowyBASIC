#ifndef  _MM_H
#define  _MM_H


#define malloc(...) mmalloc(__VA_ARGS__)
#define free(...) ffree(__VA_ARGS__)

#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

/* STACK GROWS DOWNWARD (ADDRESSES DECREASE) */
#define STACK_SIZE 0x400000   /*  4MB  */
#define STACK_ADDRESS (STACK_SIZE)

#define HEAP_SIZE 0x40000000 /*  1GB  */
#define HEAP_ADDRESS (STACK_ADDRESS + 0x40)
#define HEAP_END_ADDRESS (HEAP_ADDRESS + HEAP_SIZE)

#define ALIGNMENT 4

/*
  +--------------+ <== 0x00000000
  |              |
  |              |      4MB
  |    STACK     |
  +--------------+ <== 0x00400040
  |              |
  |     HEAP     |      1GB
  |              |
  +--------------+ <== 0x40400040
  |              |
  |              |
  +--------------+
*/

#ifndef __ASSEMBLER__

#include "types.h"

void memzero(u64 src, u64 n);
void mem_init(void);
void *mmalloc(size_t size);
void ffree(void *ptr);
void print_memory_map(void);
void* memcpy(void *dest, const void *src, size_t len);
void *memset (void *dest, int val, size_t len);

bool lickitung_check(void);

#endif

#endif  /*_MM_H */