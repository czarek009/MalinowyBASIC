#ifndef  _STARTUP_H
#define  _STARTUP_H

#include "printf.h"

#define STARTUP(...)   printf(__VA_ARGS__)

void initialize_all_modules(void);
void print_startup_info(void);

#endif  /*_STARTUP_H */