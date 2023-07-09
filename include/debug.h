#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define DEBUG(...)  printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define ERROR(...)  printf(__VA_ARGS__)

#endif /* _DEBUG_H */