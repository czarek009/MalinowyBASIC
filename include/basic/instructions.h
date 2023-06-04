#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include "types.h"

void let_instr(void* env, char* cmd);
void print_instr(void* env, char* cmd);
void input_instr(void* env, char* cmd);

#endif /* _INSTRUCTIONS_H */