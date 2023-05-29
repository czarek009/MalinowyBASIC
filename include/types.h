#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stdbool.h>

#define NULL ((void *)0)

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u64 size_t;
typedef volatile u32 reg32;

#endif /* _TYPES_H */