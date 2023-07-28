#ifndef _DEBUG_H
#define _DEBUG_H

#include "printf.h"

// #define DEBUG_BASIC
#ifdef DEBUG_BASIC
#define DEBUG(...)  printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

// #define DEBUG_MEMORY
#ifdef DEBUG_MEMORY
#define DEBUG_MEM(...)  printf(__VA_ARGS__)
#else
#define DEBUG_MEM(...)
#endif

#define INFO(...)   printf(__VA_ARGS__)
#define ERROR(...)  printf(__VA_ARGS__)

#endif /* _DEBUG_H */